#pragma once

#include "ivideotarget.h"

extern "C" {
#include "libavutil/opt.h"
#include "libavutil/imgutils.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h" // for BGRA to YUV420p conversion
}

namespace gg
{

class VideoTargetFFmpeg : public IVideoTarget
{
protected:
    //!
    //! \brief
    //!
    AVCodecID _codec_id;

    //!
    //! \brief
    //!
    AVCodec * _codec;

    //!
    //! \brief
    //!
    AVFrame * _frame;

    //!
    //! \brief
    //!
    int _framerate;

    //!
    //! \brief
    //!
    SwsContext * _sws_context;

    //!
    //! \brief
    //!
    AVFormatContext *_format_context;

    //!
    //! \brief
    //!
    AVStream * _stream;

    std::string _filepath;

    int64_t _frame_index;


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
