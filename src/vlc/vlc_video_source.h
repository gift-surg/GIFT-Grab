/**
    \file vlc_video_source.h

    \brief   video stream device: uses VLC to import video streams in our video pipeline
             the video stream can come from a file (mp4, avi, ...) or from the network
             (see at the end of this file for the supported formats)
*/

#ifndef __VLC_DATA_SOURCE_H__
#define __VLC_DATA_SOURCE_H__

#include "ivideosource.h"
#include "macros.h"

// VLC includes
#include <vlc/vlc.h>

/**
 * VLC video source including streaming.
 */
class VideoSourceVLC: public IVideoSource
{
public:
    VideoSourceVLC(const char * path);
    ~VideoSourceVLC();

    bool get_frame_dimensions( int & width, int & height );
    bool get_frame( VideoFrame_BGRA & frame );
    double get_frame_rate();
    void set_sub_frame( int x, int y, int width, int height );

private:
    // the following functions are the callbacks called by VLC
    // they must be static, but a pointer on "this" is passed as parameter and echoed as "p_video_data"
    static void prepareRender( VideoSourceVLC *  p_video_data, uint8_t ** pp_pixel_buffer , int size );

    static void handleStream( VideoSourceVLC * p_video_data, uint8_t * p_pixel_buffer, int width,
			      int height, int pixel_pitch, int size, int64_t pts );

    int                                m_size;
    uint32_t                           m_pixWidth;
    uint32_t                           m_pixHeight;
    uint8_t *                          m_videoBuffer; // has to be allocated before use !!

    libvlc_media_player_t *            m_mp;

    void mInitSource( const char * path );
    void mRunSource();

    DISALLOW_COPY_AND_ASSIGNMENT(VideoSourceVLC);

    //the source file can be in one of the following formats (perhaps a little bit outdated)
    //[file://]filename              Plain media file
    //http://ip:port/file            HTTP URL
    //ftp://ip:port/file             FTP URL
    //mms://ip:port/file             MMS URL
    //screen://                      Screen capture
    //[dvd://][device][@raw_device]  DVD device
    //[vcd://][device]               VCD device
    //[cdda://][device]              Audio CD device
    //udp:[[<source address>]@[<bind address>][:<bind port>]]
};

#endif
