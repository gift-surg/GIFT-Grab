#include "ffmpeg_video_source.h"

namespace gg
{

VideoSourceFFmpeg::VideoSourceFFmpeg()
{
    // nop
}


VideoSourceFFmpeg::VideoSourceFFmpeg(std::string source_path,
                                     enum ColourSpace colour_space)
    : IVideoSource(colour_space)
{
    // TODO
}


VideoSourceFFmpeg::~VideoSourceFFmpeg()
{
    // TODO
}


bool VideoSourceFFmpeg::get_frame_dimensions(int & width, int & height)
{
    // TODO
    return false;
}


bool VideoSourceFFmpeg::get_frame(VideoFrame & frame)
{
    // TODO
    return false;
}


double VideoSourceFFmpeg::get_frame_rate()
{
    // TODO
    return 0.0;
}


void VideoSourceFFmpeg::set_sub_frame(int x, int y, int width, int height)
{
    // TODO
}


void VideoSourceFFmpeg::get_full_frame()
{
    // TODO
}

}
