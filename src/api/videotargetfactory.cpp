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
    switch (codec)
    {
    case File_XviD:
#ifdef USE_OPENCV
        return new VideoTargetOpenCV("XVID");
#endif
    case File_HEVC:
#ifdef USE_FFMPEG
        return new VideoTargetFFmpeg("HEVC");
#endif
    case File_VP9:
#ifdef USE_FFMPEG
        return new VideoTargetFFmpeg("VP9");
#endif
    default:
        std::string msg;
        msg.append("Video target codec ")
           .append(std::to_string(codec))
           .append(" not supported");
        throw VideoTargetError(msg);
    }
}

}
