#include "ivideosource.h"
#include "broadcastdaemon.h"
extern "C"
{
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
    //! \brief Where returned frame width
    //! starts, defining the bounding box
    //! e.g. when cropping
    //!
    int _x;

    //!
    //! \brief Where returned frame height
    //! starts, defining the bounding box
    //! e.g. when cropping
    //!
    int _y;

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
    //! \brief This is for converting pixel format
    //! (e.g. from BGRA to YUV420p)
    //!
    SwsContext * _sws_context;

    //!
    //! \brief FFmpeg frame to be used when
    //! decoding frames
    //!
    AVFrame * _avframe_original;

    //!
    //! \brief FFmpeg frame to be used when video
    //! source has a different colour format
    //! (chroma) than the requested one
    //! \sa _colour
    //!
    AVFrame * _avframe_processed;

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

    //!
    //! \brief Guards the data buffers when reading
    //! frames or re-/setting cropping
    //!
    std::mutex _buffer_lock;

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
    //! \brief Try to open requested video source
    //! and perform sanity checks on it
    //! \throw VideoSourceError with a detailed
    //! message if this step fails for any reason
    //! \sa _source_path
    //!
    void ffmpeg_open_source();

    //!
    //! \brief Try to find an appropriate decoder
    //! after opening video source, and open it.
    //! This function sets the frame bounding box,
    //! so it should be used after locking the
    //! mutex guard
    //! \throw VideoSourceError with a detailed
    //! message if this step fails for any reason
    //! \sa ffmpeg_open_source
    //! \sa _x
    //! \sa _y
    //! \sa _width
    //! \sa _height
    //! \sa _buffer_lock
    //!
    void ffmpeg_open_decoder();

    //!
    //! \brief Open a colour space (pixel format)
    //! converter in case colour space of source
    //! video is different from what the caller
    //! wants, after successfully opening the
    //! video decoder
    //! \throw VideoSourceError with a detailed
    //! message if this step fails for any reason
    //! \sa ffmpeg_open_decoder
    //! \sa _colour
    //!
    void ffmpeg_open_converter();

    //!
    //! \brief Allocate all buffers and memory
    //! that will be needed for reading frames
    //! from video source
    //! \throw VideoSourceError with a detailed
    //! message if this step fails for any reason
    //!
    void ffmpeg_allocate_buffers();

    //!
    //! \brief Decode current FFmpeg packet of
    //! current FFmpeg frame
    //! \return number of decoded bytes on success,
    //! a negative value otherwise
    //!
    int ffmpeg_decode_packet();

    //!
    //! \brief Create a standard error description
    //! from \c ffmpeg_error_code, e.g. to be
    //! appended to exception details
    //! \param ffmpeg_error_code
    //! \return
    //!
    std::string get_ffmpeg_error_desc(int ffmpeg_error_code);

    //!
    //! \brief Get equivalent of \c colour in FFmpeg
    //! API
    //! \param colour
    //! \return
    //! \throw VideoSourceError if \c colour is not
    //! supported
    //!
    AVPixelFormat get_ffmpeg_pixel_format(enum ColourSpace colour);
};

}
