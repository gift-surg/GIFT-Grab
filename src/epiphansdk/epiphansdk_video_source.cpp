#include "epiphansdk_video_source.h"

namespace gg
{

VideoSourceEpiphanSDK::VideoSourceEpiphanSDK(
    const std::string device_id, V2U_INT32 colour_space)
{
    // TODO
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
