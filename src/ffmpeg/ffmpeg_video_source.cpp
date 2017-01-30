#include "ffmpeg_video_source.h"
extern "C" {
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
    , _avstream(nullptr)
    , _avcodec_context(nullptr)
    , _avframe(nullptr)
    , _avframe_converted(nullptr)
    , _sws_context(nullptr)
    , _daemon(nullptr)
{
    int ret = 0;
    std::string error_msg = "";

    av_register_all();

    if (avformat_open_input(&_avformat_context, _source_path.c_str(),
                            nullptr, nullptr) < 0)
    {
        error_msg.append("Could not open video source ")
                 .append(_source_path);
        throw VideoSourceError(error_msg);
    }

    if (avformat_find_stream_info(_avformat_context, nullptr) < 0)
        throw VideoSourceError("Could not find stream information");

    if (open_codec_context(&_avstream_idx, _avformat_context,
                           AVMEDIA_TYPE_VIDEO, error_msg) >= 0)
    {
        _avstream = _avformat_context->streams[_avstream_idx];
        _avcodec_context = _avstream->codec;

        /* allocate image where the decoded image will be put */
        _width = _avcodec_context->width;
        _height = _avcodec_context->height;
        _avpixel_format = _avcodec_context->pix_fmt;
        ret = av_image_alloc(_data_buffer, _data_buffer_linesizes,
                             _width, _height, _avpixel_format, 1);
        if (ret < 0)
            throw VideoSourceError("Could not allocate"
                                   " raw video buffer");
        _data_buffer_length = ret;
    }
    else
        throw VideoSourceError(error_msg);

    if (_avstream == nullptr)
        throw VideoSourceError("Could not find video stream in source");

    _avframe = av_frame_alloc();
    if (_avframe == nullptr)
        throw VideoSourceError("Could not allocate frame");

    /* initialize packet, set data to NULL, let the demuxer fill it */
    av_init_packet(&_avpacket);
    _avpacket.data = nullptr;
    _avpacket.size = 0;

    AVPixelFormat target_avpixel_format;
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
    if (_avpixel_format != target_avpixel_format)
    {
        switch(_avpixel_format)
        {
        case AV_PIX_FMT_BGRA:
            _stride[0] = 4 * _width;
            break;
        case AV_PIX_FMT_UYVY422:
            _stride[0] = 2 * _width;
            break;
        case AV_PIX_FMT_YUV420P:
            // TODO: 1) is this correct? 2) odd width?
            _stride[0] = 1.5 * _width;
            break;
        default:
            throw VideoSourceError("Source colour space not supported");
        }
        _avframe_converted = av_frame_alloc();
        if (_avframe_converted == nullptr)
            throw VideoSourceError("Could not allocate conversion frame");
        _avframe_converted->format = target_avpixel_format;
        _avframe_converted->width  = _width;
        _avframe_converted->height = _height;
        int pixel_depth;
        switch(target_avpixel_format)
        {
        case AV_PIX_FMT_BGRA:
            pixel_depth = 32; // bits-per-pixel
            break;
        case AV_PIX_FMT_YUV420P:
            pixel_depth = 12; // bits-per-pixel
            /* TODO #54 - alignment by _pixel_depth causes problems
             * due to buffer size mismatches between EpiphanSDK and
             * FFmpeg, hence manually filling in linesizes (based on
             * debugging measurements), in conj. with
             * av_image_fill_pointers below
             */
            _avframe_converted->linesize[0] = _avframe_converted->width;
            _avframe_converted->linesize[1] = _avframe_converted->width / 2;
            _avframe_converted->linesize[2] = _avframe_converted->width / 2;
            break;
        case AV_PIX_FMT_UYVY422:
            pixel_depth = 16; // bits-per-pixel
            break;
        default:
            throw VideoSourceError("Colour space not supported");
        }
        ret = av_frame_get_buffer(_avframe_converted, pixel_depth);
        if (ret < 0)
            throw VideoSourceError("Could not allocate conversion buffer");

        _sws_context = sws_getContext(
                    _width, _height, _avpixel_format,
                    _width, _height, target_avpixel_format,
                    0, nullptr, nullptr, nullptr);
        if (_sws_context == nullptr)
            throw VideoSourceError("Could not allocate Sws context");
    }

    _daemon = new gg::BroadcastDaemon(this);
    _daemon->start(get_frame_rate());
}


VideoSourceFFmpeg::~VideoSourceFFmpeg()
{
    delete _daemon;

    if (_sws_context != nullptr)
        sws_freeContext(_sws_context);
    avcodec_close(_avcodec_context);
    avformat_close_input(&_avformat_context);
    av_frame_free(&_avframe);
    if (_avframe_converted != nullptr)
        av_frame_free(&_avframe_converted);
    av_free(_data_buffer[0]);
    _data_buffer_length = 0;
}


bool VideoSourceFFmpeg::get_frame_dimensions(int & width, int & height)
{
    if (this->_width > 0 and this->_height > 0)
    {
        width = this->_width;
        height = this->_height;
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
    size_t passes = 0;
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

        // need to convert pixel format?
        AVFrame * avframe_ptr = nullptr;
        if (_sws_context != nullptr)
        {
            ret = sws_scale(_sws_context,
                            _avframe->data, _stride,
                            0, _height,
                            _avframe_converted->data, _avframe_converted->linesize
                            );
            if (ret <= 0)
            {
                success = false;
                break;
            }

            avframe_ptr = _avframe_converted;
        }
        else
        {
            avframe_ptr = _avframe;
        }

        /* copy decoded frame to destination buffer:
         * this is required since rawvideo expects non aligned data */
        av_image_copy(_data_buffer, _data_buffer_linesizes,
                      const_cast<const uint8_t **>(avframe_ptr->data), avframe_ptr->linesize,
                      _avpixel_format, _width, _height);

        passes++;
    }
    while (_avpacket.size > 0);
    av_packet_unref(&orig_pkt);

    if (not success)
        return false;

    // TODO - when are there multiple passes?
    if (passes != 1)
        return false;

    frame.init_from_specs(_data_buffer[0], _data_buffer_length, _width, _height);
    return true;
}


double VideoSourceFFmpeg::get_frame_rate()
{
    int num = _avstream->avg_frame_rate.num;
    int den = _avstream->avg_frame_rate.den;
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


int VideoSourceFFmpeg::open_codec_context(
        int * stream_idx, AVFormatContext * fmt_ctx,
        enum AVMediaType type, std::string & error_msg)
{
    int ret, stream_index;
    AVStream * st;
    AVCodecContext * dec_ctx = nullptr;
    AVCodec * dec = nullptr;
    AVDictionary * opts = nullptr;

    ret = av_find_best_stream(fmt_ctx, type, -1, -1, nullptr, 0);
    if (ret < 0)
    {
        error_msg.append("Could not find ")
                 .append(av_get_media_type_string(type))
                 .append(" stream in source ")
                 .append(_source_path);
        return ret;
    }
    else
    {
        stream_index = ret;
        st = fmt_ctx->streams[stream_index];

        /* find decoder for the stream */
        dec_ctx = st->codec;
        dec = avcodec_find_decoder(dec_ctx->codec_id);
        if (!dec)
        {
            error_msg.append("Failed to find ")
                     .append(av_get_media_type_string(type))
                     .append(" codec");
            return AVERROR(EINVAL);
        }

        /* Init the decoders, with or without reference counting */
        av_dict_set(&opts, "refcounted_frames", _use_refcount ? "1" : "0", 0);
        if ((ret = avcodec_open2(dec_ctx, dec, &opts)) < 0)
        {
            error_msg.append("Failed to open ")
                     .append(av_get_media_type_string(type))
                     .append(" codec");
            return ret;
        }
        *stream_idx = stream_index;
    }

    return 0;
}


int VideoSourceFFmpeg::decode_packet(int * got_frame, int cached)
{
    int ret = 0;
    int decoded = _avpacket.size;
    *got_frame = 0;
    if (_avpacket.stream_index == _avstream_idx)
    {
        /* decode video frame */
        ret = avcodec_decode_video2(_avcodec_context, _avframe, got_frame, &_avpacket);
        if (ret < 0)
            return ret;

        if (*got_frame)
        {
            if (_avframe->width != _width or _avframe->height != _height or
                _avframe->format != _avpixel_format)
                return -1;
        }
    }
    /* If we use frame reference counting, we own the data and need
     * to de-reference it when we don't use it anymore */
    if (*got_frame and _use_refcount)
        av_frame_unref(_avframe);
    return decoded;
}

}
