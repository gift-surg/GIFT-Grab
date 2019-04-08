#pragma once
#include "frmgrab.h"
#include "ivideosource.h"
#include "macros.h"
#include "broadcastdaemon.h"
#include "rgb_to_bgra_converter.h"

namespace gg
{

class VideoSourceEpiphanSDK : public IVideoSource
{
protected:
    //!
    //! \brief
    //!
    FrmGrabber * _frame_grabber;

    //!
    //! \brief Currently only colour space
    //!
    V2U_UINT32 _flags;

    //!
    //! \brief Full frame dimensions
    //!
    V2URect _full;

    //!
    //! \brief Region of interest, i.e. sub-frame
    //! \sa _full
    //!
    V2URect _roi;

    //!
    //! \brief Buffer for acquiring frame data
    //! \sa _frame_grabber
    //!
    V2U_GrabFrame2 * _buffer;

    //!
    //! \brief This mutex to be locked when
    //! accessing the buffer
    //! \sa _buffer
    //!
    std::mutex _buffer_lock;

    //!
    //! \brief
    //!
    gg::BroadcastDaemon * _daemon;

    //!
    //! \brief Buffer for converting from RGB to BGRA
    //!
    unsigned char *_bgra_data;

    //!
    //! \brief RGB to BGRA converter
    //!
    RgbToBgraConverter _rgb_to_bgra;

public:
    //!
    //! \brief Connects to specified port of an Epiphan
    //! frame grabber
    //! \param device_id defines port of frame grabber,
    //! as \c \#define'd in Epiphan device properties
    //! header
    //! \param colour_space \c V2U_GRABFRAME_FORMAT_I420
    //! or \c V2U_GRABFRAME_FORMAT_RGB24
    //! \throw VideoSourceError if connection attempt
    //! fails, with a detailed error message
    //!
    VideoSourceEpiphanSDK(const std::string device_id,
                          V2U_INT32 colour_space);

    //!
    //! \brief Release all allocated resources
    //!
    virtual ~VideoSourceEpiphanSDK();

public:
    bool get_frame_dimensions(int & width, int & height);

    bool get_frame(VideoFrame & frame);

    double get_frame_rate();

    void set_sub_frame(int x, int y, int width, int height);

    void get_full_frame();

protected:
    DISALLOW_COPY_AND_ASSIGNMENT(VideoSourceEpiphanSDK);
};

}
