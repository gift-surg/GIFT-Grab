#include "vlc_video_source.h"
#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <thread>

namespace gg
{

VideoSourceVLC::VideoSourceVLC(const std::string path)
    : _vlc_inst(nullptr)
    , _vlc_mp(nullptr)
    , _video_buffer(nullptr)
    , _data_length(0)
    , _cols(0)
    , _rows(0)
    , _sub(nullptr)
    , _path(path)
{
    this->init_vlc();
    this->run_vlc();
}


VideoSourceVLC::~VideoSourceVLC()
{
    stop_vlc();
    release_vlc();
}


bool VideoSourceVLC::get_frame_dimensions(int & width, int & height)
{
    ///\todo mutex

    if(this->_cols == 0 || this->_rows == 0)
        return false;

    width = this->_cols;
    height = this->_rows;
    return true;
}


bool VideoSourceVLC::get_frame(VideoFrame_BGRA & frame)
{
    throw VideoSourceError("VLC video source supports only I420 colour space");

    // TODO

    ///\todo mutex

    if (_cols == 0 || _rows == 0 || _video_buffer == nullptr)
        return false;

    // allocate and fill the image
    if (_cols * _rows * 4 == this->_data_length)
        frame = VideoFrame_BGRA(this->_video_buffer, this->_cols, this->_rows);
    else
        throw VideoSourceError("VLC video source does not support padded images");

    return true;
}


bool VideoSourceVLC::get_frame(VideoFrame_I420 & frame)
{
    if (_data_length > 0)
    {
        // TODO manage data?
        frame = VideoFrame_I420(_video_buffer, _data_length, _cols, _rows, false);
        return true;
    }
    else
        return false;

    // TODO #86
}


double VideoSourceVLC::get_frame_rate()
{
    //return libvlc_media_player_get_rate( m_mp );
    return libvlc_media_player_get_fps(_vlc_mp);
    //throw std::runtime_error("get_frame_rate to be implemented");
    //return 0.0;
}


void VideoSourceVLC::set_sub_frame(int x, int y, int width, int height)
{
    // TODO mutex?

    if (x >= _full.x and x + width <= _full.x + _full.width and
        y >= _full.y and y + height <= _full.y + _full.height)
    {
        stop_vlc();
        release_vlc();
        if (_sub == nullptr) _sub = new FrameBox;
        _sub->x = x;
        _sub->y = y;
        _sub->width = width;
        _sub->height = height;
        init_vlc();
        run_vlc();
    }
}


void VideoSourceVLC::get_full_frame()
{
    // TODO mutex?
    stop_vlc();
    release_vlc();
    if (_sub)
    {
        delete _sub;
        _sub = nullptr;
    }
    init_vlc();
    run_vlc();
}


void VideoSourceVLC::init_vlc()
{
    // VLC pointers
    libvlc_media_t * vlc_media = nullptr;

    // VLC options
    char smem_options[512];

    sprintf(smem_options, "#");
    if (_sub != nullptr)
    {
        unsigned int croptop = _sub->y,
                     cropbottom = _full.height - (_sub->y + _sub->height),
                     cropleft = _sub->x,
                     cropright = _full.width - (_sub->x + _sub->width);
        sprintf(smem_options,
                "%stranscode{vcodec=I420,vfilter=croppadd{",
                smem_options);
        sprintf(smem_options,
                "%scroptop=%u,cropbottom=%u,cropleft=%u,cropright=%u}}:",
                smem_options,
                croptop, cropbottom, cropleft, cropright);
    }
    sprintf(smem_options,
            "%ssmem{video-data=%lld,video-prerender-callback=%lld,video-postrender-callback=%lld}",
            smem_options,
            (long long int)(intptr_t)(void*) this,
            (long long int)(intptr_t)(void*) &VideoSourceVLC::prepareRender,
            (long long int)(intptr_t)(void*) &VideoSourceVLC::handleStream );

    const char * const vlc_args[] = {
        "-I", "dummy", // Don't use any interface
        "--ignore-config", // Don't use VLC's config
        "--extraintf=logger", // Log anything
        // TODO - what about the options below?
        //"--verbose=2", // Be much more verbose then normal for debugging purpose
        //"--clock-jitter=0",
        //"--file-caching=150",
        "--no-audio",
        "--sout", smem_options // Stream to memory
    };

    // We launch VLC
    _vlc_inst = libvlc_new(sizeof(vlc_args) / sizeof(vlc_args[0]), vlc_args);
    if (_vlc_inst == nullptr)
        throw VideoSourceError("Could not create VLC engine");

    // If path contains a colon (:), it will be treated as a
    // URL. Else, it will be considered as a local path.
    if (_path.find(":") == std::string::npos)
        vlc_media = libvlc_media_new_path(_vlc_inst, _path.c_str());
    else
        vlc_media = libvlc_media_new_location(_vlc_inst, _path.c_str());
    if (vlc_media == nullptr)
        throw VideoSourceError(std::string("Could not open ").append(_path));

    libvlc_media_add_option(vlc_media, ":noaudio");
    libvlc_media_add_option(vlc_media, ":no-video-title-show");

    // Create a media player playing environement
    _vlc_mp = libvlc_media_player_new_from_media(vlc_media);
    if (_vlc_mp == nullptr)
        throw VideoSourceError("Could not create VLC media player");

    // No need to keep the media now
    libvlc_media_release( vlc_media );
}


void VideoSourceVLC::run_vlc()
{
    // play the media_player
    if (libvlc_media_player_play(_vlc_mp) != 0)
        throw VideoSourceError("Could not start VLC media player");

    // empirically determined value that allows for initialisation
    // to succeed before any API functions are called on this object
    std::this_thread::sleep_for(std::chrono::milliseconds(350));
    unsigned int width, height;
    if (libvlc_video_get_size(_vlc_mp, 0, &width, &height) != 0)
        throw VideoSourceError("Could not get video dimensions");

    _full.x = 0;
    _full.y = 0;
    _full.width = width;
    _full.height = height;
}


void VideoSourceVLC::stop_vlc()
{
    if (libvlc_media_player_is_playing(_vlc_mp) == 1)
        // stop playing
        libvlc_media_player_stop(_vlc_mp);
}


void VideoSourceVLC::release_vlc()
{
    // free media player
    if (_vlc_mp)
        libvlc_media_player_release(_vlc_mp);

    // free engine
    if (_vlc_inst)
        libvlc_release(_vlc_inst);

    // free buffer
    if (_video_buffer != nullptr)
        delete[] _video_buffer;
    _data_length = 0;
    _cols = 0;
    _rows = 0;

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
    ///\todo create mutex guard

    unsigned int width, height;
    if (libvlc_video_get_size(p_video_data->_vlc_mp, 0, &width, &height) != 0)
        return;
    p_video_data->_cols = width;
    p_video_data->_rows = height;

    if (size > p_video_data->_data_length)
        p_video_data->_video_buffer = reinterpret_cast<uint8_t *>(
                    realloc(p_video_data->_video_buffer, size * sizeof(uint8_t))
                    );
    p_video_data->_data_length = size;

    *pp_pixel_buffer = p_video_data->_video_buffer;
}


void VideoSourceVLC::handleStream(VideoSourceVLC * p_video_data,
                                  uint8_t * p_pixel_buffer,
                                  size_t cols,
                                  size_t rows,
                                  size_t colour_depth,
                                  size_t size)
{
    // TODO: explain how data should be handled (see #86)
    // TODO: Unlock the mutex
}


std::string VideoSourceVLC::encode_psz_geometry(int x, int y, int width, int height)
{
    std::string psz_geometry;
    psz_geometry.append(std::to_string(width)).append("x")
                .append(std::to_string(height))
                .append("+").append(std::to_string(x))
                .append("+").append(std::to_string(y));
    return psz_geometry;
}

}
