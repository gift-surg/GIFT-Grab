#pragma once
#include "v2u_defs.h"
#include "dvi2pcieduo_defs.h"
#include "ivideosource.h"
#include "macros.h"

namespace gg
{

class VideoSourceEpiphanSDK : public IVideoSource
{
public:
    //!
    //! \brief Connects to specified port of an Epiphan
    //! frame grabber
    //! \param device_id defines port of frame grabber,
    //! as \c #define'd in Epiphan device properties
    //! header
    //! \param colour_space \c V2U_GRABFRAME_FORMAT_I420
    //! or \c V2U_GRABFRAME_FORMAT_BGR24
    //! \throw VideoSourceError if connection attempt
    //! fails, with a detailed error message
    //!
    VideoSourceEpiphanSDK(const std::string device_id,
                          V2U_INT32 colour_space);

public:
    bool get_frame_dimensions(int & width, int & height);

    bool get_frame(VideoFrame_BGRA & frame);

    bool get_frame(VideoFrame_I420 & frame);

    double get_frame_rate();

    void set_sub_frame(int x, int y, int width, int height);

    void get_full_frame();

    DISALLOW_COPY_AND_ASSIGNMENT(VideoSourceEpiphanSDK);
};

}
