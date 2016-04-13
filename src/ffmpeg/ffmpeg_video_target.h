#pragma once

#include "ivideotarget.h"

extern "C" {
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
#include <libavformat/avformat.h>
}

namespace gg
{

class VideoTargetFFmpeg : public IVideoTarget
{
public:
    //!
    //! \brief Constructor defining what \c codec to
    //! use for encoding
    //! \param codec
    //!
    VideoTargetFFmpeg(const std::string codec = "H265");

public:
    void init(const std::string filepath, const float framerate);

    void append(const VideoFrame_BGRA & frame);

    void finalise();
};

}
