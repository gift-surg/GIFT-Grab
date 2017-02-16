#include "ffmpeg_video_source.h"
extern "C"
{
#include <libavutil/imgutils.h>
}


namespace gg
{

VideoSourceFFmpeg::VideoSourceFFmpeg()
{
    // nop
}


VideoSourceFFmpeg::VideoSourceFFmpeg(std::string source_path,
                                     enum ColourSpace colour_space,
                                     bool use_refcount)
    : IVideoSource(colour_space)
    , _x(0)
    , _y(0)
    , _source_path(source_path)
    , _avformat_context(nullptr)
    , _avstream_idx(-1)
    , _use_refcount(use_refcount)
    , _avframe_original(nullptr)
    , _avframe_processed(nullptr)
    , _daemon(nullptr)
{
    av_register_all();

    ffmpeg_open_source();
    ffmpeg_open_decoder();
    ffmpeg_alloc_read_buffers();
    int width = _avframe_original->width;
    int height = _avframe_original->height;
    if (ffmpeg_realloc_proc_buffers(width, height))
        ffmpeg_reset_pipeline(0, 0, width, height);

    _daemon = new gg::BroadcastDaemon(this);
    _daemon->start(get_frame_rate());
}


VideoSourceFFmpeg::~VideoSourceFFmpeg()
{
    delete _daemon;

    avcodec_close(_avformat_context->streams[_avstream_idx]->codec);
    avformat_close_input(&_avformat_context);
    av_frame_free(&_avframe_original);
    av_frame_free(&_avframe_processed);
    av_free(_data_buffer[0]);
    _data_buffer_length = 0;
}


bool VideoSourceFFmpeg::get_frame_dimensions(int & width, int & height)
{
    std::lock_guard<std::mutex> buffer_lock_guard(_buffer_lock);

    if (_avframe_processed->width > 0 and _avframe_processed->height > 0)
    {
        width = _avframe_processed->width;
        height = _avframe_processed->height;
        return true;
    }
    else
        return false;
}


bool VideoSourceFFmpeg::get_frame(VideoFrame & frame)
{
    std::lock_guard<std::mutex> buffer_lock_guard(_buffer_lock);

    int ret = 0;

    // read frame
    ret = av_read_frame(_avformat_context, &_avpacket);
    if (ret < 0)
        return false;

    // decode frame
    bool success = true;
    AVPacket orig_pkt = _avpacket;
    do
    {
        ret = ffmpeg_decode_packet(_avpacket,
                                   _avframe_original,
                                   _use_refcount);
        if (ret < 0)
        {
            success = false;
            break;
        }
        _avpacket.data += ret;
        _avpacket.size -= ret;
    }
    while (_avpacket.size > 0);
    av_packet_unref(&orig_pkt);
    if (not success)
        return false;

    // TODO _avframe_original => _avframe_processed PIPELINE

    // get non-aligned data from decoded frame
    av_image_copy(_data_buffer, _data_buffer_linesizes,
                  const_cast<const uint8_t **>(_avframe_processed->data),
                  _avframe_processed->linesize,
                  static_cast<AVPixelFormat>(_avframe_processed->format),
                  _avframe_processed->width,
                  _avframe_processed->height);

    // initialise GIFT-Grab frame from flat data
    frame.init_from_specs(_data_buffer[0], _data_buffer_length,
                          _avframe_processed->width, _avframe_processed->height);
    return true;
}


double VideoSourceFFmpeg::get_frame_rate()
{
    int num = _avformat_context->streams[_avstream_idx]->avg_frame_rate.num;
    int den = _avformat_context->streams[_avstream_idx]->avg_frame_rate.den;
    if (den == 0 or num == 0)
        return 0.0;
    return static_cast<double>(num) / den;
}


void VideoSourceFFmpeg::set_sub_frame(int x, int y, int width, int height)
{
    std::lock_guard<std::mutex> buffer_lock_guard(_buffer_lock);

    try
    {
        if (ffmpeg_reset_pipeline(x, y, width, height))
        {
            if (ffmpeg_realloc_proc_buffers(width, height))
            {
                _x = x;
                _y = y;
            }
        }
    }
    catch (VideoSourceError & e)
    {
        // nop
    }
}


void VideoSourceFFmpeg::get_full_frame()
{
    std::lock_guard<std::mutex> buffer_lock_guard(_buffer_lock);

    try
    {
        int width = _avformat_context->streams[_avstream_idx]->codec->width;
        int height = _avformat_context->streams[_avstream_idx]->codec->height;
        if (ffmpeg_realloc_proc_buffers(width, height))
        {
            if (ffmpeg_reset_pipeline(0, 0, width, height))
            {
                _x = 0;
                _y = 0;
            }
        }
    }
    catch (VideoSourceError & e)
    {
        // nop
    }
}


void VideoSourceFFmpeg::ffmpeg_open_source()
{
    int ret = 0;
    std::string error_msg = "";

    ret = avformat_open_input(&_avformat_context, _source_path.c_str(),
                              nullptr, nullptr);
    if (ret < 0)
    {
        error_msg.append("Could not open video source ")
                 .append(_source_path)
                 .append(get_ffmpeg_error_desc(ret));
        throw VideoSourceError(error_msg);
    }

    ret = avformat_find_stream_info(_avformat_context, nullptr);
    if (ret < 0)
    {
        error_msg.append("Could not find stream information")
                 .append(get_ffmpeg_error_desc(ret));
        throw VideoSourceError(error_msg);
    }

    ret = av_find_best_stream(_avformat_context, AVMEDIA_TYPE_VIDEO,
                              -1, -1, nullptr, 0);
    if (ret < 0)
    {
        error_msg.append("Could not find video stream in source ")
                 .append(_source_path)
                 .append(get_ffmpeg_error_desc(ret));
        throw VideoSourceError(error_msg);
    }
    if (_avformat_context->streams[ret] == nullptr)
        throw VideoSourceError("Found stream index is empty");

    _avstream_idx = ret;
}


void VideoSourceFFmpeg::ffmpeg_open_decoder()
{
    int ret = 0;
    std::string error_msg = "";

    // find codec
    AVCodec * avcodec = nullptr;
    avcodec = avcodec_find_decoder(
        _avformat_context->streams[_avstream_idx]->codec->codec_id
    );
    if (avcodec == nullptr)
        throw VideoSourceError("Could not find video decoder");

    // open codec
    AVDictionary * opts = nullptr;
    av_dict_set(&opts, "refcounted_frames", _use_refcount ? "1" : "0", 0);
    ret = avcodec_open2(
        _avformat_context->streams[_avstream_idx]->codec, avcodec, &opts
    );
    if (ret < 0)
    {
        error_msg.append("Could not open video decoder")
                 .append(get_ffmpeg_error_desc(ret));
        throw VideoSourceError(error_msg);
    }

    _x = 0;
    _y = 0;
}


void VideoSourceFFmpeg::ffmpeg_alloc_read_buffers()
{
    int ret = 0;
    std::string error_msg = "";

    // decoded image will be put here
    _avframe_original = av_frame_alloc();
    if (_avframe_original == nullptr)
        throw VideoSourceError("Could not allocate frame");

    // packet for decoding frames: demuxer will fill it
    av_init_packet(&_avpacket);
    _avpacket.data = nullptr;
    _avpacket.size = 0;
}


bool VideoSourceFFmpeg::ffmpeg_reset_pipeline(
    int x, int y, int width, int height
)
{
    // TODO
    return false;
}


bool VideoSourceFFmpeg::ffmpeg_realloc_proc_buffers(
    int width, int height
)
{
    int ret = 0;
    std::string error_msg = "";

    if (width <= 0 or height <= 0)
        throw VideoSourceError("Frame width and height"
                               " must be positive");

    if (_avframe_processed != nullptr)
    {
        if (_avframe_processed->width == width and
            _avframe_processed->height == height)
            return false;
        av_frame_free(&_avframe_processed);
    }

    _avframe_processed = av_frame_alloc();
    if (_avframe_processed == nullptr)
        throw VideoSourceError("Could not allocate processing frame");
    AVPixelFormat pix_fmt = get_ffmpeg_pixel_format(_colour);
    _avframe_processed->format = pix_fmt;
    _avframe_processed->width  = width;
    _avframe_processed->height = height;
    int pixel_depth = 0;
    try
    {
        pixel_depth = VideoFrame::required_pixel_length(_colour);
    }
    catch (BasicException & e)
    {
        throw VideoSourceError(e.what());
    }

    ret = av_frame_get_buffer(_avframe_processed, pixel_depth);
    if (ret < 0)
    {
        error_msg.append("Could not allocate processing buffer")
                 .append(get_ffmpeg_error_desc(ret));
        throw VideoSourceError(error_msg);
    }

    // allocate actual data buffer
    if (_data_buffer[0] != nullptr)
    {
        av_free(_data_buffer[0]);
        _data_buffer_length = 0;
    }

    ret = av_image_alloc(_data_buffer, _data_buffer_linesizes,
                         _avframe_processed->width,
                         _avframe_processed->height,
                         pix_fmt, 1);
    if (ret < 0)
    {
        error_msg.append("Could not allocate raw processing buffer")
                 .append(get_ffmpeg_error_desc(ret));
        throw VideoSourceError(error_msg);
    }
    _data_buffer_length = ret;

    return true;
}


int VideoSourceFFmpeg::ffmpeg_decode_packet(AVPacket & avpacket,
                                            AVFrame * avframe,
                                            bool use_refcount)
{
    int ret = 0, got_frame = 0;
    int decoded = 0;
    if (avpacket.stream_index == _avstream_idx)
    {
        /* decode video frame */
        ret = avcodec_decode_video2(
                    _avformat_context->streams[_avstream_idx]->codec,
                    avframe, &got_frame, &avpacket);
        if (ret < 0)
            return ret;

        if (got_frame)
        {
            if (avframe->width != _avformat_context->streams[_avstream_idx]->codec->width
                or avframe->height != _avformat_context->streams[_avstream_idx]->codec->height
                or avframe->format != _avformat_context->streams[_avstream_idx]->codec->pix_fmt)
                return -1;
            decoded = ret;
        }
    }

    /* If we use frame reference counting, we own the data and need
     * to de-reference it when we don't use it anymore */
    if (got_frame and use_refcount)
        av_frame_unref(avframe);

    return decoded;
}

std::string VideoSourceFFmpeg::get_ffmpeg_error_desc(int ffmpeg_error_code)
{
    const size_t FFMPEG_ERR_MSG_SIZE = 2048;
    char ffmpeg_err_msg[FFMPEG_ERR_MSG_SIZE];
    std::string ffmpeg_error_desc = "";
    if (av_strerror(ffmpeg_error_code,
                    ffmpeg_err_msg,
                    FFMPEG_ERR_MSG_SIZE) == 0)
        ffmpeg_error_desc.append(" (FFmpeg error description: ")
                         .append(std::string(ffmpeg_err_msg))
                         .append(")");
    else
        ffmpeg_error_desc.append(" (No specific error description"
                                 " could be obtained from FFmpeg)");
    return ffmpeg_error_desc;
}


AVPixelFormat VideoSourceFFmpeg::get_ffmpeg_pixel_format(
    enum ColourSpace colour
)
{
    switch(colour)
    {
    case BGRA:
        return AV_PIX_FMT_BGRA;
    case I420:
        return AV_PIX_FMT_YUV420P;
    case UYVY:
        return AV_PIX_FMT_UYVY422;
    default:
        throw VideoSourceError("Colour space not supported");
    }
}

}
