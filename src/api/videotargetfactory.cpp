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

IVideoTarget * VideoTargetFactory::create_file_writer(enum Storage codec,
                                                      const std::string filename)
{
    IVideoTarget * target = nullptr;
    switch (codec)
    {
    case File_XviD:
#ifdef USE_OPENCV
        target = new VideoTargetOpenCV("XVID");
        break;
#endif
    case File_HEVC:
#ifdef USE_FFMPEG
        target = new VideoTargetFFmpeg("HEVC");
        break;
#endif
    case File_VP9:
#ifdef USE_FFMPEG
        target = new VideoTargetFFmpeg("VP9");
        break;
#endif
    default:
        std::string msg;
        msg.append("Video target codec ")
           .append(std::to_string(codec))
           .append(" not supported");
        throw VideoTargetError(msg);
    }

    if (target != nullptr)
        // TODO #34
        target->init(filename, 30.0);

    return target;
}

}
