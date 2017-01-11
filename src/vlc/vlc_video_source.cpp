#include "vlc_video_source.h"
#ifdef BUILD_PYTHON
#include "gil.h"
#endif
#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <thread>

namespace gg
{

//!
//! \brief This is to be used by the
//! \c prepareRender and/or \c handleStream
//! functions as appropriate
//!
VideoFrame buffer_frame(I420, false);


VideoSourceVLC::VideoSourceVLC(const std::string path)
    : IVideoSource(gg::I420)
    , _vlc_inst(nullptr)
    , _vlc_mp(nullptr)
    , _running(false)
    , _video_buffer(nullptr)
    , _data_length(0)
    , _cols(0)
    , _rows(0)
    , _sub(nullptr)
    , _path(path)
    , _frame_rate(0)
{
    init_vlc();
    run_vlc();
    determine_full();
}

VideoSourceVLC::VideoSourceVLC(const std::string path,
                               const unsigned int frame_rate)
    : IVideoSource(gg::I420)
    , _vlc_inst(nullptr)
    , _vlc_mp(nullptr)
    , _running(false)
    , _video_buffer(nullptr)
    , _data_length(0)
    , _cols(0)
    , _rows(0)
    , _sub(nullptr)
    , _path(path)
    , _frame_rate(frame_rate)
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
    if(_cols == 0 || _rows == 0)
        return false;

    width = _cols;
    height = _rows;
    return true;
}


bool VideoSourceVLC::get_frame(VideoFrame & frame)
{
    if (frame.colour() != _colour)
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
    if (_frame_rate > 0)
        return _frame_rate;
    else
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
    stop_vlc();
    reset_crop();
    run_vlc();
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

    // create VLC media player
    _vlc_mp = libvlc_media_player_new(_vlc_inst);
    if (_vlc_mp == nullptr)
        throw VideoSourceError("Could not create VLC media player");
}


void VideoSourceVLC::run_vlc()
{
    libvlc_media_t * vlc_media = nullptr;

    // If path contains a colon (:), it will be treated as a
    // URL. Else, it will be considered as a local path.
    if (_path.find(":") == std::string::npos)
        vlc_media = libvlc_media_new_path(_vlc_inst, _path.c_str());
    else
        vlc_media = libvlc_media_new_location(_vlc_inst, _path.c_str());
    if (vlc_media == nullptr)
        throw VideoSourceError(std::string("Could not open ").append(_path));

    // compose the processing pipeline description
    char pipeline[512];
    // open transcode step
    sprintf(pipeline, "#transcode{");
    // cropping sub-frame
    unsigned int croptop = 0, cropbottom = 0, cropleft = 0, cropright = 0;
    if (_sub != nullptr) // cropping video?
    {
        croptop = std::max(_sub->y, croptop),
        cropbottom = std::max(_full.height - (_sub->y + _sub->height), cropbottom),
        cropleft = std::max(_sub->x, cropleft),
        cropright = std::max(_full.width - (_sub->x + _sub->width), cropright);
    }
    sprintf(pipeline, "%svfilter=croppadd{", pipeline);
    sprintf(pipeline, "%scroptop=%u,", pipeline, croptop);
    sprintf(pipeline, "%scropbottom=%u,", pipeline, cropbottom);
    sprintf(pipeline, "%scropleft=%u,", pipeline, cropleft);
    sprintf(pipeline, "%scropright=%u", pipeline, cropright);
    sprintf(pipeline, "%s}", pipeline);
    // colour space specification
    sprintf(pipeline, "%s,vcodec=I420", pipeline);
    // frame rate specification
    if (_frame_rate > 0)
        sprintf(pipeline, "%s,fps=%u", pipeline, _frame_rate);
    // close transcode step
    sprintf(pipeline, "%s}:", pipeline);
    // callbacks
    sprintf(pipeline,
            "%ssmem{video-data=%lld,video-prerender-callback=%lld,video-postrender-callback=%lld}",
            pipeline,
            (long long int)(intptr_t)(void*) this,
            (long long int)(intptr_t)(void*) &VideoSourceVLC::prepareRender,
            (long long int)(intptr_t)(void*) &VideoSourceVLC::handleStream );
    // activate pipeline in VLC media
    char sout_options[1024];
    sprintf(sout_options, ":sout=%s", pipeline);
    libvlc_media_add_option(vlc_media, sout_options);
    // set VLC media player's media
    libvlc_media_player_set_media(_vlc_mp, vlc_media);
    // release VLC media
    libvlc_media_release(vlc_media);

#ifdef BUILD_PYTHON
    ScopedPythonGILRelease gil_release;
#endif

    { // artificial scope for the mutex guard below
        std::lock_guard<std::mutex> data_lock_guard(_data_lock);
        // play the media_player
        if (libvlc_media_player_play(_vlc_mp) != 0)
            throw VideoSourceError("Could not start VLC media player");
        _running = true;
    }

    // empirically determined value that allows for initialisation
    // to succeed before any API functions are called on this object
    std::this_thread::sleep_for(std::chrono::milliseconds(350));

    // ensure no error!
    if (libvlc_media_player_get_state(_vlc_mp) == libvlc_Error)
        throw gg::VideoSourceError(std::string("Could not open ").append(_path));
}


void VideoSourceVLC::stop_vlc()
{
    // stop playing
    libvlc_media_player_stop(_vlc_mp);

    { // artificial scope for mutex guard below
        std::lock_guard<std::mutex> data_lock_guard(_data_lock);
        _running = false;
    }
}


void VideoSourceVLC::release_vlc()
{
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
    get_frame_dimensions((int&)_full.width, (int&)_full.height);
    _full.x = 0;
    _full.y = 0;
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
            p_video_data->_video_buffer = reinterpret_cast<uint8_t *>(
                        realloc(p_video_data->_video_buffer, size * sizeof(uint8_t))
                        );
        p_video_data->_data_length = p_video_data->_video_buffer != nullptr ? size : 0;

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
        buffer_frame.init_from_specs(p_pixel_buffer, size, cols, rows);
        p_video_data->notify(buffer_frame);
    }
}

}
