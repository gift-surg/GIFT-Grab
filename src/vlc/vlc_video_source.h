#pragma once

#include "ivideosource.h"
#include "macros.h"
#include <vlc/vlc.h>
#include <mutex>

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
    //! \brief Resource locator path, as in the list provided
    //! within the class description
    //!
    std::string _path;

    //!
    //! \brief VLC engine
    //!
    libvlc_instance_t * _vlc_inst;

    //!
    //! \brief Actual VLC player (streamer)
    //!
    libvlc_media_player_t * _vlc_mp;

    //!
    //! \brief Flag indicating whether VLC player running
    //! \sa _vlc_mp
    //!
    bool _running;

    //!
    //! \brief This mutex will be locked when writing to or
    //! reading from the data buffer
    //! \sa _video_buffer
    //!
    std::mutex _data_lock;

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

    //!
    //! \brief If this is not null, it means video is
    //! being cropped
    //! \sa set_sub_frame
    //! \sa get_full_frame
    //!
    FrameBox * _sub;

    //!
    //! \brief The frame rate for devices or network
    //! streams
    //!
    unsigned int _frame_rate;

public:
    //!
    //! \brief Initialise video source using given \c path
    //! and start streaming
    //!
    //! \param path as in list provided in class description.
    //! Note that due to the way this class is architected,
    //! it is not suitable for use with files, but only with
    //! live streams.
    //!
    //! \sa get_full_frame
    //! \sa set_sub_frame
    //!
    //! \throw gg::VideoSourceError with a detailed message
    //! in case \c path cannot be opened
    //!
    VideoSourceVLC(const std::string path);

    //!
    //! \brief Open connection to a device or network
    //! stream specified by \c path. This constructor is not
    //! suitable for video files, where the \c frame_rate is
    //! encoded in the file. Apart from this, its behaviour
    //! is the same as VideoSourceVLC(const std::string path).
    //! \param path
    //! \param frame_rate
    //!
    //! \sa VideoSourceVLC(const std::string path)
    //!
    VideoSourceVLC(const std::string path,
                   const unsigned int frame_rate);

    //!
    //! \brief Stop streaming and finalise current video
    //! source, freeing all used resources
    //!
    virtual ~VideoSourceVLC();

    bool get_frame_dimensions(int & width, int & height);

    bool get_frame(VideoFrame & frame);

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
    //! \throw gg::VideoSourceError with a detailed message in case
    //! initialisation fails
    //!
    void init_vlc();

    //!
    //! \brief Start streaming from previously configured video
    //! source
    //!
    //! \throw gg::VideoSourceError with a detailed message in case
    //! launch of streaming fails
    //!
    void run_vlc();

    //!
    //! \brief Stop VLC engine, if already running
    //!
    //! \throw gg::VideoSourceError with a detailed message in case
    //! stopping of streaming fails
    //!
    void stop_vlc();

    //!
    //! \brief Release all resources related to a stopped VLC
    //! engine
    //!
    //! \throw gg::VideoSourceError with a detailed message in case
    //! release of VLC engine fails
    //!
    void release_vlc();

    //!
    //! \brief Release used resources, e.g. buffers
    //!
    void clear();

    //!
    //! \brief Determine and record full frame dimensions
    //!
    void determine_full();

    //!
    //! \brief Set video cropping region
    //! \param x
    //! \param y
    //! \param width
    //! \param height
    //!
    void set_crop(unsigned int x, unsigned int y,
                  unsigned int width, unsigned int height);

    //!
    //! \brief Reset video cropping, i.e. set full video frame
    //!
    void reset_crop();

    DISALLOW_COPY_AND_ASSIGNMENT(VideoSourceVLC);
};

}
