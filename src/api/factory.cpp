#include "factory.h"
#ifdef USE_OPENCV
#include "opencv_video_target.h"
#endif // USE_OPENCV
#ifdef USE_FFMPEG
#include "ffmpeg_video_target.h"
#endif

namespace gg {

IVideoTarget * Factory::writer(Storage type)
{
    switch (type)
    {
    case File_XviD:
#ifdef USE_OPENCV
        return new VideoTargetOpenCV("XVID");
#endif // USE_OPENCV
    case File_HEVC:
#ifdef USE_FFMPEG
        return new VideoTargetFFmpeg("HEVC");
#else
        // nop, see default below
#endif
    case File_VP9:
#ifdef USE_FFMPEG
        return new VideoTargetFFmpeg("VP9");
#else
        // nop, see default below
#endif
    default:
        std::string msg;
        msg.append("Video target type ")
           .append(std::to_string(type))
           .append(" not supported");
        throw VideoTargetError(msg);
    }
}

}
