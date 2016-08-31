#pragma once

#include "ivideosource.h"
#include "macros.h"
#include <vlc/vlc.h>

namespace gg
{

//!
//! \brief This class uses VLC to import video streams that can
//! come from files (mp4, avi, ...) or from the network.
//!
//! Supported formats are (perhaps a little bit outdated):
//!   * Plain media file: <tt> [file://]filename </tt>
//!   * HTTP URL: <tt> http://ip:port/file </tt>
//!   * FTP URL: <tt> ftp://ip:port/file </tt>
//!   * MMS URL: <tt> mms://ip:port/file </tt>
//!   * Screen capture: <tt> screen:// </tt>
//!   * DVD device: <tt> [dvd://][device][@raw_device] </tt>
//!   * VCD device: <tt> [vcd://][device] </tt>
//!   * Audio CD device: <tt> [cdda://][device] </tt>
//!   * <tt> udp:[[<source address>]@[<bind address>][:<bind port>]] </tt>
//!
class VideoSourceVLC: public IVideoSource
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

public:
    //!
    //! \brief Initialise video source using given \c path
    //! and start streaming
    //! \param path as in list provided in class description
    //!
    VideoSourceVLC(const std::string path);

    //!
    //! \brief Finalise current video source and stop streaming
    //!
    ~VideoSourceVLC();

    bool get_frame_dimensions( int & width, int & height );

    bool get_frame( VideoFrame_BGRA & frame );

    bool get_frame(VideoFrame_I420 & frame);

    double get_frame_rate();

    void set_sub_frame( int x, int y, int width, int height );

private:
    //!
    //! \brief Callback called by VLC.
    //!
    //! This needs to be static, but a pointer to \c this is passed
    //! as parameter and echoed as "p_video_data"
    //! \param p_video_data
    //! \param pp_pixel_buffer
    //! \param size
    //!
    static void prepareRender( VideoSourceVLC *  p_video_data, uint8_t ** pp_pixel_buffer , int size );

    //!
    //! \brief Callback called by VLC.
    //!
    //! This needs to be static, but a pointer to \c this is passed
    //! as parameter and echoed as "p_video_data"
    //! \param p_video_data
    //! \param p_pixel_buffer
    //! \param width
    //! \param height
    //! \param pixel_pitch
    //! \param size
    //! \param pts
    //!
    static void handleStream( VideoSourceVLC * p_video_data, uint8_t * p_pixel_buffer, int width,
                              int height, int pixel_pitch, int size, int64_t pts );

    //!
    //! \brief Wraps low-level calls needed by VLC for initialising
    //! video source specified in \c path
    //! \param path as in list provided in class description
    //! \throw std::runtime_error in case initialisation fails
    //!
    void mInitSource( const char * path );

    //!
    //! \brief Starts streaming
    //! \throw std::runtime_error in case launch of streaming fails
    //!
    void mRunSource();

    DISALLOW_COPY_AND_ASSIGNMENT(VideoSourceVLC);
};

}
