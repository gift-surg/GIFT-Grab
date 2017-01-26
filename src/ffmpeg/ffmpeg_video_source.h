#include "ivideosource.h"
extern "C" {
#include <libavformat/avformat.h>
}


namespace gg
{

//!
//! \brief Uses FFmpeg for streaming video from
//! compatible sources such as video files
//!
class VideoSourceFFmpeg : public IVideoSource
{
protected:
    //!
    //! \brief Path to the video source, e.g.
    //! video file
    //!
    std::string _source_path;

    //!
    //! \brief FFmpeg format context
    //!
    AVFormatContext * _avformat_context;

    //!
    //! \brief Index of used FFmpeg video
    //! stream
    //! \sa _avstream
    //!
    int _avstream_idx;
    int refcount;

    //!
    //! \brief FFmpeg video stream to be used
    //! for reading frames from video source
    //!
    AVStream * _avstream;

    //!
    //! \brief FFmpeg codec context to be used
    //! for decoding frames from video source
    //! being read
    //!
    AVCodecContext * _avcodec_context;

    //!
    //! \brief Width of frames obtained from
    //! video source being read
    //!
    int _width;

    //!
    //! \brief Height of frames obtained from
    //! video source being read
    //!
    int _height;

    //!
    //! \brief The pixel format of the video
    //! source being read
    //!
    enum AVPixelFormat _avpixel_format;

    //!
    //! \brief FFmpeg frame to be used when
    //! decoding frames
    //!
    AVFrame * _avframe;

    //!
    //! \brief FFmpeg packet to be used when
    //! decoding frames
    //!
    AVPacket _avpacket;

    //!
    //! \brief Data buffer to be used by
    //! FFmpeg routines
    //!
    uint8_t * _data_buffer[4] = {nullptr};

    //!
    //! \brief Linesizes to be used by FFmpeg
    //! routines
    //! \sa _data_buffer
    //!
    int _data_buffer_linesizes[4];

    //!
    //! \brief Length of allocated data buffer
    //! \sa _data_buffer
    //!
    int _data_buffer_length;

protected:
    //!
    //! \brief Default constructor that should not
    //! be publicly called
    //!
    VideoSourceFFmpeg();

public:
    //!
    //! \brief Open video source pointed to by \c
    //! source_path for acquiring video frames in
    //! specified \c colour_space
    //! \param source_path
    //! \param colour_space
    //! \throw VideoSourceError with a detailed
    //! error message if \c source_path cannot be
    //! opened
    //!
    VideoSourceFFmpeg(std::string source_path,
                      enum ColourSpace colour_space);

    //!
    //! \brief
    //!
    virtual ~VideoSourceFFmpeg();

public:
    bool get_frame_dimensions(int & _width, int & _height);

    bool get_frame(VideoFrame & frame);

    double get_frame_rate();

    void set_sub_frame(int x, int y, int _width, int _height);

    void get_full_frame();

protected:
    //!
    //! \brief Open the FFmpeg format context
    //! \param stream_idx
    //! \param fmt_ctx
    //! \param type
    //! \param error_msg
    //! \return non-negative value on success,
    //! negative value with a detailed \c error_msg
    //! otherwise
    //!
    int open_codec_context(int * stream_idx,
                           AVFormatContext * _avformat_context,
                           enum AVMediaType type,
                           std::string & error_msg);

    //!
    //! \brief Decode current FFmpeg packet of
    //! current FFmpeg frame
    //! \param got_frame
    //! \param cached
    //! \return number of decoded bytes on success,
    //! a negative value otherwise
    //!
    int decode_packet(int * got_frame, int cached);
};

}
