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
    , _source_path(source_path)
    , _avformat_context(nullptr)
    , _avstream_idx(-1)
    , _use_refcount(use_refcount)
    , _avframe(nullptr)
    , _avframe_converted(nullptr)
    , _sws_context(nullptr)
    , _daemon(nullptr)
{
    int ret = 0;
    std::string error_msg = "";

    av_register_all();

    open_source();

    ret = open_codec_context(&_avstream_idx, _avformat_context,
                             AVMEDIA_TYPE_VIDEO, error_msg);
    if (ret < 0)
    {
        error_msg.append(get_ffmpeg_error_desc(ret));
        throw VideoSourceError(error_msg);
    }

    if (_avformat_context->streams[_avstream_idx] == nullptr)
        throw VideoSourceError("Could not find video stream in source");

    /* allocate image where the decoded image will be put */
    _avframe = av_frame_alloc();
    if (_avframe == nullptr)
        throw VideoSourceError("Could not allocate frame");

    /* initialize packet, set data to NULL, let the demuxer fill it */
    av_init_packet(&_avpacket);
    _avpacket.data = nullptr;
    _avpacket.size = 0;

    AVPixelFormat target_avpixel_format;
    int sws_flags = 0;
    switch(_colour)
    {
    case BGRA:
        target_avpixel_format = AV_PIX_FMT_BGRA;
        break;
    case I420:
        target_avpixel_format = AV_PIX_FMT_YUV420P;
        break;
    case UYVY:
        target_avpixel_format = AV_PIX_FMT_UYVY422;
        break;
    default:
        throw VideoSourceError("Target colour space not supported");
    }
    if (_avformat_context->streams[_avstream_idx]->codec->pix_fmt != target_avpixel_format)
    {
        _avframe_converted = av_frame_alloc();
        if (_avframe_converted == nullptr)
            throw VideoSourceError("Could not allocate conversion frame");
        _avframe_converted->format = target_avpixel_format;
        _avframe_converted->width  = _avformat_context->streams[_avstream_idx]->codec->width;
        _avframe_converted->height = _avformat_context->streams[_avstream_idx]->codec->height;
        int pixel_depth;
        switch(target_avpixel_format)
        {
        case AV_PIX_FMT_BGRA:
            pixel_depth = 32; // bits-per-pixel
            break;
        case AV_PIX_FMT_YUV420P:
            pixel_depth = 12; // bits-per-pixel
            break;
        case AV_PIX_FMT_UYVY422:
            pixel_depth = 16; // bits-per-pixel
            break;
        default:
            throw VideoSourceError("Colour space not supported");
        }
        ret = av_frame_get_buffer(_avframe_converted, pixel_depth);
        if (ret < 0)
        {
            error_msg.append("Could not allocate conversion buffer")
                     .append(get_ffmpeg_error_desc(ret));
            throw VideoSourceError(error_msg);
        }

        _sws_context = sws_getContext(
                    _avformat_context->streams[_avstream_idx]->codec->width,
                    _avformat_context->streams[_avstream_idx]->codec->height,
                    _avformat_context->streams[_avstream_idx]->codec->pix_fmt,
                    _avformat_context->streams[_avstream_idx]->codec->width,
                    _avformat_context->streams[_avstream_idx]->codec->height,
                    target_avpixel_format,
                    sws_flags, nullptr, nullptr, nullptr);
        if (_sws_context == nullptr)
            throw VideoSourceError("Could not allocate Sws context");
    }

    ret = av_image_alloc(_data_buffer, _data_buffer_linesizes,
                         _avformat_context->streams[_avstream_idx]->codec->width,
                         _avformat_context->streams[_avstream_idx]->codec->height,
                         target_avpixel_format, 1);
    if (ret < 0)
    {
        error_msg.append("Could not allocate raw video buffer")
                 .append(get_ffmpeg_error_desc(ret));
        throw VideoSourceError(error_msg);
    }
    _data_buffer_length = ret;

    _daemon = new gg::BroadcastDaemon(this);
    _daemon->start(get_frame_rate());
}


VideoSourceFFmpeg::~VideoSourceFFmpeg()
{
    delete _daemon;

    if (_sws_context != nullptr)
        sws_freeContext(_sws_context);
    avcodec_close(_avformat_context->streams[_avstream_idx]->codec);
    avformat_close_input(&_avformat_context);
    av_frame_free(&_avframe);
    if (_avframe_converted != nullptr)
        av_frame_free(&_avframe_converted);
    av_free(_data_buffer[0]);
    _data_buffer_length = 0;
}


bool VideoSourceFFmpeg::get_frame_dimensions(int & width, int & height)
{
    if (_avformat_context->streams[_avstream_idx]->codec->width > 0
        and _avformat_context->streams[_avstream_idx]->codec->height > 0)
    {
        width = _avformat_context->streams[_avstream_idx]->codec->width;
        height = _avformat_context->streams[_avstream_idx]->codec->height;
        return true;
    }
    else
        return false;
}


bool VideoSourceFFmpeg::get_frame(VideoFrame & frame)
{
    if (av_read_frame(_avformat_context, &_avpacket) < 0)
        return false;

    int ret = 0, got_frame;
    bool success = true;
    AVPacket orig_pkt = _avpacket;
    do
    {
        ret = decode_packet(&got_frame, 0);
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

    // need to convert pixel format?
    AVFrame * avframe_ptr = nullptr;
    if (_sws_context != nullptr)
    {
        ret = sws_scale(_sws_context,
                        _avframe->data, _avframe->linesize,
                        0, _avformat_context->streams[_avstream_idx]->codec->height,
                        _avframe_converted->data, _avframe_converted->linesize
                        );
        if (ret <= 0)
            success = false;

        avframe_ptr = _avframe_converted;
    }
    else
    {
        avframe_ptr = _avframe;
    }

    /* copy decoded frame to destination buffer:
     * this is required since rawvideo expects non aligned data */
    av_image_copy(_data_buffer, _data_buffer_linesizes,
                  const_cast<const uint8_t **>(avframe_ptr->data),
                  avframe_ptr->linesize,
                  static_cast<AVPixelFormat>(avframe_ptr->format),
                  _avformat_context->streams[_avstream_idx]->codec->width,
                  _avformat_context->streams[_avstream_idx]->codec->height);

    if (not success)
        return false;

    frame.init_from_specs(
            _data_buffer[0], _data_buffer_length,
            _avformat_context->streams[_avstream_idx]->codec->width,
            _avformat_context->streams[_avstream_idx]->codec->height);
    return true;
}


double VideoSourceFFmpeg::get_frame_rate()
{
    int num = _avformat_context->streams[_avstream_idx]->avg_frame_rate.num;
    int den = _avformat_context->streams[_avstream_idx]->avg_frame_rate.den;
    if (num == 0)
        return 0.0;
    return static_cast<double>(num) / den;
}


void VideoSourceFFmpeg::set_sub_frame(int x, int y, int width, int height)
{
    // TODO
}


void VideoSourceFFmpeg::get_full_frame()
{
    // TODO
}


void VideoSourceFFmpeg::open_source()
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
}


int VideoSourceFFmpeg::open_codec_context(
        int * stream_idx, AVFormatContext * fmt_ctx,
        enum AVMediaType type, std::string & error_msg)
{
    int ret;

    // find stream
    ret = av_find_best_stream(fmt_ctx, type, -1, -1, nullptr, 0);
    if (ret < 0)
    {
        error_msg.append("Could not find ")
                 .append(av_get_media_type_string(type))
                 .append(" stream in source ")
                 .append(_source_path);
        return ret;
    }
    int stream_index = ret;

    // find codec
    AVCodec * avcodec = nullptr;
    avcodec = avcodec_find_decoder(fmt_ctx->streams[stream_index]->codec->codec_id);
    if (avcodec == nullptr)
    {
        error_msg.append("Could not find ")
                 .append(av_get_media_type_string(type))
                 .append(" codec");
        return AVERROR(EINVAL);
    }

    // open codec
    AVDictionary * opts = nullptr;
    av_dict_set(&opts, "refcounted_frames", _use_refcount ? "1" : "0", 0);
    ret = avcodec_open2(fmt_ctx->streams[stream_index]->codec, avcodec, &opts);
    if (ret < 0)
    {
        error_msg.append("Failed to open ")
                 .append(av_get_media_type_string(type))
                 .append(" codec");
        return ret;
    }

    // set stream index to found one
    *stream_idx = stream_index;

    return ret;
}


int VideoSourceFFmpeg::decode_packet(int * got_frame, int cached)
{
    int ret = 0;
    int decoded = 0;
    *got_frame = 0;
    if (_avpacket.stream_index == _avstream_idx)
    {
        /* decode video frame */
        ret = avcodec_decode_video2(
                    _avformat_context->streams[_avstream_idx]->codec,
                    _avframe, got_frame, &_avpacket);
        if (ret < 0)
            return ret;

        if (*got_frame)
        {
            if (_avframe->width != _avformat_context->streams[_avstream_idx]->codec->width
                or _avframe->height != _avformat_context->streams[_avstream_idx]->codec->height
                or _avframe->format != _avformat_context->streams[_avstream_idx]->codec->pix_fmt)
                return -1;
            decoded = ret;
        }
    }
    /* If we use frame reference counting, we own the data and need
     * to de-reference it when we don't use it anymore */
    if (*got_frame and _use_refcount)
        av_frame_unref(_avframe);
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

}
