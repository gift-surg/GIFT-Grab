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
    char * src_filename;
    AVFormatContext * fmt_ctx;
    int video_stream_idx;
    int refcount;
    AVStream * video_stream;
    AVCodecContext * video_dec_ctx;
    int width;
    int height;
    enum AVPixelFormat pix_fmt;
    AVFrame * _avframe;
    AVPacket pkt;
    uint8_t * video_dst_data[4] = {nullptr};
    int video_dst_linesize[4];
    int video_dst_bufsize;
    int video_frame_count;

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
    bool get_frame_dimensions(int & width, int & height);

    bool get_frame(VideoFrame & frame);

    double get_frame_rate();

    void set_sub_frame(int x, int y, int width, int height);

    void get_full_frame();

protected:
    int open_codec_context(int * stream_idx,
                           AVFormatContext * fmt_ctx,
                           enum AVMediaType type,
                           std::string & error_msg);

    int decode_packet(int * got_frame, int cached);
};

}
