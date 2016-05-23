#include "epiphansdk_video_source.h"

namespace gg
{

VideoSourceEpiphanSDK::VideoSourceEpiphanSDK(
    const std::string device_id, V2U_INT32 colour_space)
    :
      _frame_grabber(nullptr),
      _flags(0),
      _roi(nullptr)
{
    FrmGrab_Init();

    _frame_grabber = FrmGrabLocal_OpenSN(device_id.c_str());
    if (not _frame_grabber)
    {
        // TODO - exception GiftGrab#42
        std::cerr << "Could not open " << device_id << std::endl;
        return;
    }

    if (colour_space != V2U_GRABFRAME_FORMAT_I420 and colour_space != V2U_GRABFRAME_FORMAT_BGR24)
    {
        // TODO - exception GiftGrab#42
        std::cerr << "Colour space " << colour_space << " not supported" << std::endl;
        return;
    }
    _flags |= colour_space;
}

bool VideoSourceEpiphanSDK::get_frame_dimensions(int & width, int & height)
{
    // TODO
    return false;
}

bool VideoSourceEpiphanSDK::get_frame(VideoFrame_BGRA & frame)
{
    // TODO
    return false;
}

bool VideoSourceEpiphanSDK::get_frame(VideoFrame_I420 & frame)
{
    // TODO
    return false;
}

double VideoSourceEpiphanSDK::get_frame_rate()
{
    // TODO
    return -1;
}

void VideoSourceEpiphanSDK::set_sub_frame(int x, int y, int width, int height)
{
    // TODO
}

void VideoSourceEpiphanSDK::get_full_frame()
{
    // TODO
}

}
