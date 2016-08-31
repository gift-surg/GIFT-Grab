#pragma once

#include "ivideosource.h"
#include "macros.h"
#include <vlc/vlc.h>

namespace gg
{

//!
//! \brief Nicely encapsulates video frame dimensions for use
//! when (re)setting region of interest.
//!
struct FrameBox
{
    unsigned int x;
    unsigned int y;
    unsigned int width;
    unsigned int height;
};

//!
//! \brief This class uses VLC to import video streams that can
//! come from files, devices, network, etc., pointed to by a
//! supported media resource locator.
//!
//! For a list of the supported media resource locators, see:
//! https://wiki.videolan.org/Media_resource_locator/
//!
class VideoSourceVLC : public IVideoSource
{
protected:
    //!
    //! \brief VLC engine
    //!
    libvlc_instance_t * _vlc_inst;

    //!
    //! \brief Actual VLC player (streamer)
    //!
    libvlc_media_player_t * _vlc_mp;

    //!
    //! \brief Buffer for frames, has to be allocated before use
    //!
    uint8_t * _video_buffer;

    //!
    //! \brief Total amount of memory allocated for frames
    //! \sa _cols
    //! \sa _rows
    //!
    size_t _data_length;

    //!
    //! \brief Frame width
    //!
    uint32_t _cols;

    //!
    //! \brief Frame height
    //!
    uint32_t _rows;

    //!
    //! \brief This keeps the full frame box specs
    //! \sa set_sub_frame
    //! \sa get_full_frame
    //!
    FrameBox _full;

public:
    //!
    //! \brief Initialise video source using given \c path
    //! and start streaming
    //!
    //! \param path as in list provided in class description
    //!
    //! \throw gg::VideoSourceError with a detailed message
    //! in case \c path cannot be opened
    //!
    VideoSourceVLC(const std::string path);

    //!
    //! \brief Stop streaming and finalise current video
    //! source, freeing all used resources
    //!
    ~VideoSourceVLC();

    bool get_frame_dimensions(int & width, int & height);

    bool get_frame(VideoFrame_BGRA & frame);

    bool get_frame(VideoFrame_I420 & frame);

    double get_frame_rate();

    void set_sub_frame(int x, int y, int width, int height);

    void get_full_frame();

protected:
    //!
    //! \brief \c video-prerender-callback called by VLC.
    //!
    //! This needs to be static, but a pointer to \c this is passed
    //! as parameter.
    //!
    //! \param p_video_data
    //! \param pp_pixel_buffer
    //! \param size
    //!
    static void prepareRender(VideoSourceVLC * p_video_data,
                              uint8_t ** pp_pixel_buffer,
                              size_t size);

    //!
    //! \brief \c video-prerender-callback called by VLC.
    //!
    //! This needs to be static, but a pointer to \c this is passed
    //! as parameter.
    //!
    //! \param p_video_data
    //! \param p_pixel_buffer
    //! \param cols
    //! \param rows
    //! \param colour_depth
    //! \param size
    //!
    static void handleStream(VideoSourceVLC * p_video_data,
                             uint8_t * p_pixel_buffer,
                             size_t cols,
                             size_t rows,
                             size_t colour_depth,
                             size_t size);

    //!
    //! \brief Initialise VLC low-level configuration for opening
    //! video source specified in \c path
    //!
    //! \param path as in list provided in class description
    //!
    //! \throw gg::VideoSourceError with a detailed message in case
    //! initialisation fails
    //!
    void init_vlc(const char * path);

    //!
    //! \brief Start streaming from previously configured video
    //! source
    //!
    //! \throw gg::VideoSourceError with a detailed message in case
    //! launch of streaming fails
    //!
    void run_vlc();

    //!
    //! \brief Generate a string representation of a video frame's
    //! dimensions, as specified in https://wiki.videolan.org/Crop/
    //! \param x
    //! \param y
    //! \param width
    //! \param height
    //! \return
    //!
    std::string encode_psz_geometry(int x, int y, int width, int height);

    DISALLOW_COPY_AND_ASSIGNMENT(VideoSourceVLC);
};

}
