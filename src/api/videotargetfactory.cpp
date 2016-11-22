#include "videotargetfactory.h"
#ifdef USE_OPENCV
#include "opencv_video_target.h"
#endif
#ifdef USE_FFMPEG
#include "ffmpeg_video_target.h"
#endif

namespace gg
{

VideoTargetFactory VideoTargetFactory::_factory_singleton;

VideoTargetFactory::VideoTargetFactory()
{
    // nop
}

VideoTargetFactory::~VideoTargetFactory()
{
    // nop
}

VideoTargetFactory & VideoTargetFactory::get_instance()
{
    return _factory_singleton;
}

IVideoTarget * VideoTargetFactory::create_file_writer(enum Codec codec,
                                                      const std::string filename,
                                                      const float frame_rate)
{
    IVideoTarget * target = nullptr;
    switch (codec)
    {
    case Xvid:
#ifdef USE_OPENCV
        target = new VideoTargetOpenCV("XVID", filename, frame_rate);
        break;
#endif
    case HEVC:
#ifdef USE_FFMPEG
        target = new VideoTargetFFmpeg("HEVC", filename, frame_rate);
        break;
#endif
    case VP9:
#ifdef USE_FFMPEG
        target = new VideoTargetFFmpeg("VP9", filename, frame_rate);
        break;
#endif
    default:
        std::string msg;
        msg.append("Video target codec ")
           .append(std::to_string(codec))
           .append(" not supported");
        throw VideoTargetError(msg);
    }

    return target;
}

}
