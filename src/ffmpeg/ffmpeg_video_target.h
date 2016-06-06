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
    std::string _filepath;

    //!
    //! \brief
    //!
    AVFormatContext *_format_context;

    //!
    //! \brief
    //!
    AVStream * _stream;

    //!
    //! \brief
    //!
    std::string _codec_name;

    //!
    //! \brief
    //!
    AVCodec * _codec;

    //!
    //! \brief
    //!
    AVFrame * _frame;

    //!
    //! \brief Pixel depth (bits-per-pixel)
    //!
    int _pixel_depth;

    //!
    //! \brief Flag indicating first frame being
    //! appended, i.e. to infer the frame params
    //! like width and height automatically
    //!
    bool _first_frame;

    //!
    //! \brief
    //!
    int64_t _frame_index;

    //!
    //! \brief
    //!
    int _framerate;

    //!
    //! \brief Packet for use when encoding and
    //! subsequently writing frames
    //!
    AVPacket _packet;

    //!
    //! \brief This is for converting pixel format
    //! (e.g. from BGRA to YUV420p)
    //!
    SwsContext * _sws_context;

    //!
    //! \brief
    //! \sa _sws_context
    //!
    const uint8_t * _src_data_ptr[1];

    //!
    //! \brief
    //! \sa _sws_context
    //!
    int _bgra_stride[1];

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

#ifdef USE_COLOUR_SPACE_I420
    void append(const VideoFrame_I420 & frame);
#endif

    void finalise();

protected:
    //!
    //! \brief Prepare frame data for encoding
    //! \param data
    //! \param data_length
    //! \param width
    //! \param height
    //! \param colour_space
    //! \param frame
    //! \sa encode_and_write
    //! \throw VideoTargetError with a detailed
    //! message on failure in any step
    //!
    void ffmpeg_frame(const unsigned char * data,
                      const size_t data_length,
                      const int width, const int height,
                      const AVPixelFormat colour_space,
                      AVFrame * frame);

    //!
    //! \brief Convenience function for code reuse when
    //! writing delayed frames
    //! \param frame leave \c NULL if writing delayed
    //! frames
    //! \param got_output used as \c got_packet_ptr
    //! parameter of \c avcodec_encode_video2() function
    //! of FFmpeg
    //! \throw VideoTargetError propagates any exception
    //! to caller
    //! \sa finalise
    //!
    void encode_and_write(AVFrame * frame, int & got_output);
};

}
