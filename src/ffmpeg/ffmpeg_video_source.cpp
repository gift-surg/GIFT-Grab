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
                                     enum ColourSpace colour_space)
    : IVideoSource(colour_space)
    , _source_path(source_path)
    , _avformat_context(nullptr)
    , video_stream_idx(-1)
    , refcount(0)
    , video_stream(nullptr)
    , video_dec_ctx(nullptr)
    , _avframe(nullptr)
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

    if (open_codec_context(&video_stream_idx, _avformat_context,
                           AVMEDIA_TYPE_VIDEO, error_msg) >= 0)
    {
        video_stream = _avformat_context->streams[video_stream_idx];
        video_dec_ctx = video_stream->codec;

        /* allocate image where the decoded image will be put */
        width = video_dec_ctx->width;
        height = video_dec_ctx->height;
        pix_fmt = video_dec_ctx->pix_fmt;
        ret = av_image_alloc(video_dst_data, video_dst_linesize,
                             width, height, pix_fmt, 1);
        if (ret < 0)
            throw VideoSourceError("Could not allocate"
                                   " raw video buffer");
        video_dst_bufsize = ret;
    }
    else
        throw VideoSourceError(error_msg);

    if (video_stream == nullptr)
        throw VideoSourceError("Could not find video stream in source");

    _avframe = av_frame_alloc();
    if (_avframe == nullptr)
        throw VideoSourceError("Could not allocate frame");

    /* initialize packet, set data to NULL, let the demuxer fill it */
    av_init_packet(&_avpacket);
    _avpacket.data = nullptr;
    _avpacket.size = 0;
}


VideoSourceFFmpeg::~VideoSourceFFmpeg()
{
    avcodec_close(video_dec_ctx);
    avformat_close_input(&_avformat_context);
    av_frame_free(&_avframe);
    av_free(video_dst_data[0]);
    video_dst_bufsize = 0;
}


bool VideoSourceFFmpeg::get_frame_dimensions(int & width, int & height)
{
    if (this->width > 0 and this->height > 0)
    {
        width = this->width;
        height = this->height;
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

        /* copy decoded frame to destination buffer:
         * this is required since rawvideo expects non aligned data */
        av_image_copy(video_dst_data, video_dst_linesize,
                      (const uint8_t **)(_avframe->data), _avframe->linesize,
                      pix_fmt, width, height);

        passes++;
    }
    while (_avpacket.size > 0);
    av_packet_unref(&orig_pkt);

    if (not success)
        return false;

    // TODO - when are there multiple passes?
    if (passes != 1)
        return false;

    frame.init_from_specs(video_dst_data[0], video_dst_bufsize, width, height);
    return true;
}


double VideoSourceFFmpeg::get_frame_rate()
{
    int num = video_stream->time_base.num;
    int den = video_stream->time_base.den;
    if (num == 0)
        return 0.0;
    return static_cast<double>(den) / num;
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
        av_dict_set(&opts, "refcounted_frames", refcount ? "1" : "0", 0);
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
    if (_avpacket.stream_index == video_stream_idx)
    {
        /* decode video frame */
        ret = avcodec_decode_video2(video_dec_ctx, _avframe, got_frame, &_avpacket);
        if (ret < 0)
            return ret;

        if (*got_frame)
        {
            if (_avframe->width != width or _avframe->height != height or
                _avframe->format != pix_fmt)
                return -1;
        }
    }
    /* If we use frame reference counting, we own the data and need
     * to de-reference it when we don't use it anymore */
    if (*got_frame and refcount)
        av_frame_unref(_avframe);
    return decoded;
}

}
