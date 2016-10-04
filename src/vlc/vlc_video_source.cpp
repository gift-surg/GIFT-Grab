#include "vlc_video_source.h"
#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <thread>

namespace gg
{

VideoSourceVLC::VideoSourceVLC(const std::string path)
    : _vlc_inst(nullptr)
    , _vlc_media(nullptr)
    , _vlc_mp(nullptr)
    , _running(false)
    , _video_buffer(nullptr)
    , _data_length(0)
    , _cols(0)
    , _rows(0)
    , _sub(nullptr)
    , _path(path)
{
    init_vlc();
    run_vlc();
    determine_full();
}


VideoSourceVLC::~VideoSourceVLC()
{
    stop_vlc();
    release_vlc();
    clear();
}


bool VideoSourceVLC::get_frame_dimensions(int & width, int & height)
{
    std::lock_guard<std::mutex> data_lock_guard(_data_lock);
    if(this->_cols == 0 || this->_rows == 0)
        return false;

    width = this->_cols;
    height = this->_rows;
    return true;
}


bool VideoSourceVLC::get_frame(VideoFrame & frame)
{
    if (frame.colour() != ColourSpace::I420)
        return false;

    std::lock_guard<std::mutex> data_lock_guard(_data_lock);
    if (_data_length > 0)
    {
        frame.init_from_specs(_video_buffer, _data_length, _cols, _rows);
        return true;
    }
    else
        return false;

    // TODO #86
}


double VideoSourceVLC::get_frame_rate()
{
    return libvlc_media_player_get_fps(_vlc_mp);
}


void VideoSourceVLC::set_sub_frame(int x, int y, int width, int height)
{
    if (x >= _full.x and x + width <= _full.x + _full.width and
        y >= _full.y and y + height <= _full.y + _full.height)
    {
        stop_vlc();
        set_crop(x, y, width, height);
        run_vlc();
    }
}


void VideoSourceVLC::get_full_frame()
{
    set_sub_frame(_full.x, _full.y, _full.width, _full.height);
}


void VideoSourceVLC::init_vlc()
{
    // VLC global options (valid for the lifetime of the program)
    // see `vlc --help` for the difference between `--option` and `:option`
    const char * const vlc_args[] = {
        "-I", "dummy", // Don't use any interface
        "--ignore-config", // Don't use VLC's config
        "--file-logging",
        //"--verbose=2", // Be much more verbose then normal for debugging purpose
        "--no-audio",
        "--no-video-title-show"
    };

    // We launch VLC
    _vlc_inst = libvlc_new(sizeof(vlc_args) / sizeof(vlc_args[0]), vlc_args);
    if (_vlc_inst == nullptr)
        throw VideoSourceError("Could not create VLC engine");

    // If path contains a colon (:), it will be treated as a
    // URL. Else, it will be considered as a local path.
    if (_path.find(":") == std::string::npos)
        _vlc_media = libvlc_media_new_path(_vlc_inst, _path.c_str());
    else
        _vlc_media = libvlc_media_new_location(_vlc_inst, _path.c_str());
    if (_vlc_media == nullptr)
        throw VideoSourceError(std::string("Could not open ").append(_path));

    // Create a media player playing environement
    _vlc_mp = libvlc_media_player_new_from_media(_vlc_media);
    if (_vlc_mp == nullptr)
        throw VideoSourceError("Could not create VLC media player");
}


void VideoSourceVLC::run_vlc()
{
    // compose the processing pipeline description
    char pipeline[512];
    sprintf(pipeline, "#");
    // colour space specification
    sprintf(pipeline, "%stranscode{vcodec=I420", pipeline);
    if (_sub != nullptr) // cropping video?
    {
        unsigned int croptop = _sub->y,
                     cropbottom = _full.height - (_sub->y + _sub->height),
                     cropleft = _sub->x,
                     cropright = _full.width - (_sub->x + _sub->width);
        sprintf(pipeline, "%s,vfilter=croppadd{", pipeline);
        if (croptop > 0)
        {
            sprintf(pipeline, "%scroptop=%u", pipeline, croptop);
            if (cropbottom > 0 or cropleft > 0 or cropright > 0)
                sprintf(pipeline, "%s,", pipeline);
        }
        if (cropbottom > 0)
        {
            sprintf(pipeline, "%scropbottom=%u", pipeline, cropbottom);
            if (cropleft > 0 or cropright > 0)
                sprintf(pipeline, "%s,", pipeline);
        }
        if (cropleft > 0)
        {
            sprintf(pipeline, "%scropleft=%u", pipeline, cropleft);
            if (cropright > 0)
                sprintf(pipeline, "%s,", pipeline);
        }
        if (cropright > 0)
        {
            sprintf(pipeline, "%scropright=%u", pipeline, cropright);
        }
        sprintf(pipeline, "%s}", pipeline);
    }
    sprintf(pipeline, "%s}:", pipeline);
    // callbacks
    sprintf(pipeline,
            "%ssmem{video-data=%lld,video-prerender-callback=%lld,video-postrender-callback=%lld}",
            pipeline,
            (long long int)(intptr_t)(void*) this,
            (long long int)(intptr_t)(void*) &VideoSourceVLC::prepareRender,
            (long long int)(intptr_t)(void*) &VideoSourceVLC::handleStream );

    { // artificial scope for the mutex guard below
        std::lock_guard<std::mutex> data_lock_guard(_data_lock);

        // activate pipeline in VLC media
        char sout_options[1024];
        sprintf(sout_options, ":sout=%s", pipeline);
        libvlc_media_add_option(_vlc_media, sout_options);

        // play the media_player
        _running = true;
        if (libvlc_media_player_play(_vlc_mp) != 0)
            throw VideoSourceError("Could not start VLC media player");
    }

    // empirically determined value that allows for initialisation
    // to succeed before any API functions are called on this object
    std::this_thread::sleep_for(std::chrono::milliseconds(350));
}


void VideoSourceVLC::stop_vlc()
{
    // stop playing
    libvlc_media_player_stop(_vlc_mp);

    std::lock_guard<std::mutex> data_lock_guard(_data_lock);

    _running = false;
}


void VideoSourceVLC::release_vlc()
{
    // free media
    libvlc_media_release(_vlc_media);

    // free media player
    libvlc_media_player_release(_vlc_mp);

    // free engine
    libvlc_release(_vlc_inst);
}


void VideoSourceVLC::clear()
{
    // free buffer
    std::lock_guard<std::mutex> data_lock_guard(_data_lock);
    if (_video_buffer != nullptr)
    {
        delete[] _video_buffer;
        _video_buffer = nullptr;
    }
    _data_length = 0;
    _cols = 0;
    _rows = 0;

    reset_crop();
}


void VideoSourceVLC::determine_full()
{
    unsigned int width, height;
    if (libvlc_video_get_size(_vlc_mp, 0, &width, &height) != 0)
        throw VideoSourceError("Could not get video dimensions");

    _full.x = 0;
    _full.y = 0;
    _full.width = width;
    _full.height = height;
}


void VideoSourceVLC::set_crop(unsigned int x, unsigned int y,
                              unsigned int width, unsigned int height)
{
    if (_sub == nullptr) _sub = new FrameBox;
    _sub->x = x;
    _sub->y = y;
    _sub->width = width;
    _sub->height = height;
}


void VideoSourceVLC::reset_crop()
{
    // free sub-frame
    if (_sub != nullptr)
    {
        delete _sub;
        _sub = nullptr;
    }
}


void VideoSourceVLC::prepareRender(VideoSourceVLC * p_video_data,
                                   uint8_t ** pp_pixel_buffer,
                                   size_t size)
{
    std::lock_guard<std::mutex> data_lock_guard(p_video_data->_data_lock);
    if (p_video_data->_running)
    {
        if (size > p_video_data->_data_length)
        {
            if (p_video_data->_data_length == 0)
                p_video_data->_video_buffer = reinterpret_cast<uint8_t *>(
                            malloc(size * sizeof(uint8_t))
                            );
            else
                p_video_data->_video_buffer = reinterpret_cast<uint8_t *>(
                            realloc(p_video_data->_video_buffer, size * sizeof(uint8_t))
                            );
        }
        p_video_data->_data_length = size;

        *pp_pixel_buffer = p_video_data->_video_buffer;
    }
}


void VideoSourceVLC::handleStream(VideoSourceVLC * p_video_data,
                                  uint8_t * p_pixel_buffer,
                                  size_t cols,
                                  size_t rows,
                                  size_t colour_depth,
                                  size_t size)
{
    std::lock_guard<std::mutex> data_lock_guard(p_video_data->_data_lock);
    // TODO: explain how data should be handled (see #86)
    if (p_video_data->_running)
    {
        p_video_data->_cols = cols;
        p_video_data->_rows = rows;
    }
}

}
