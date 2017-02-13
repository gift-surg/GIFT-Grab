#include "ivideosource.h"
#include "broadcastdaemon.h"
extern "C" {
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

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
    //! \brief Broadcast daemon that will keep
    //! reading frames from the video source
    //! and updating all observers
    //!
    gg::BroadcastDaemon * _daemon;

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

    //!
    //! \brief Flag indicating whether the
    //! reference counting frame system is to
    //! be used so as to keep a copy of
    //! obtained data for longer than a single
    //! decoding cycle
    //!
    bool _use_refcount;

    //!
    //! \brief FFmpeg codec context to be used
    //! for decoding frames from video source
    //! being read
    //!
    AVCodecContext * _avcodec_context;

    //!
    //! \brief This is for converting pixel format
    //! (e.g. from BGRA to YUV420p)
    //!
    SwsContext * _sws_context;

    //!
    //! \brief FFmpeg frame to be used when
    //! decoding frames
    //!
    AVFrame * _avframe;

    //!
    //! \brief FFmpeg frame to be used when video
    //! source has a different colour format
    //! (chroma) than the requested one
    //! \sa _colour
    //!
    AVFrame * _avframe_converted;

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
    //! \param use_refcount whether to use
    //! reference counting to be able to keep a
    //! copy of obtained data for longer than a
    //! single decoding cycle
    //! \throw VideoSourceError with a detailed
    //! error message if \c source_path cannot be
    //! opened
    //!
    VideoSourceFFmpeg(std::string source_path,
                      enum ColourSpace colour_space,
                      bool use_refcount = false);

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
                           AVFormatContext * fmt_ctx,
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

    //!
    //! \brief Create a standard error description
    //! from \c ffmpeg_error_code, e.g. to be
    //! appended to exception details
    //! \param ffmpeg_error_code
    //! \return
    //!
    std::string get_ffmpeg_error_desc(int ffmpeg_error_code);
};

}
