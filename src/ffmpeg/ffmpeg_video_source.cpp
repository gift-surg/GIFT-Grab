#include "ffmpeg_video_source.h"
extern "C" {
#include <libavutil/imgutils.h>
#include <libavutil/timestamp.h>
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
    , src_filename(nullptr)
    , fmt_ctx(nullptr)
    , video_stream_idx(-1)
    , refcount(0)
    , video_stream(nullptr)
    , video_dec_ctx(nullptr)
    , _avframe(nullptr)
    , video_frame_count(0)
    , _data(nullptr)
    , _data_length(0)
{
    int ret = 0;
    std::string error_msg = "";

    src_filename = const_cast<char *>(source_path.c_str());

    av_register_all();

    if (avformat_open_input(&fmt_ctx, src_filename, nullptr, nullptr) < 0)
    {
        error_msg.append("Could not open video source ")
                 .append(source_path);
        throw VideoSourceError(error_msg);
    }

    if (avformat_find_stream_info(fmt_ctx, nullptr) < 0)
        throw VideoSourceError("Could not find stream information");

    if (open_codec_context(&video_stream_idx, fmt_ctx,
                           AVMEDIA_TYPE_VIDEO, error_msg) >= 0)
    {
        video_stream = fmt_ctx->streams[video_stream_idx];
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

    // TODO
    av_dump_format(fmt_ctx, 0, src_filename, 0);

    if (video_stream == nullptr)
        throw VideoSourceError("Could not find video stream in source");

    _avframe = av_frame_alloc();
    if (_avframe == nullptr)
        throw VideoSourceError("Could not allocate frame");

    /* initialize packet, set data to NULL, let the demuxer fill it */
    av_init_packet(&pkt);
    pkt.data = nullptr;
    pkt.size = 0;
}


VideoSourceFFmpeg::~VideoSourceFFmpeg()
{
    // TODO: is this part needed?
    int got_frame;
    /* flush cached frames */
    pkt.data = nullptr;
    pkt.size = 0;
    std::string error_msg = "";
    do
        decode_packet(&got_frame, 1, error_msg);
    while (got_frame);

    avcodec_close(video_dec_ctx);
    avformat_close_input(&fmt_ctx);
    av_frame_free(&_avframe);
    av_free(video_dst_data[0]);

    if (_data_length > 0)
    {
        free(_data);
        _data_length = 0;
    }
}


bool VideoSourceFFmpeg::get_frame_dimensions(int & width, int & height)
{
    // TODO
    return false;
}


bool VideoSourceFFmpeg::get_frame(VideoFrame & frame)
{
    // TODO
    return false;
}


double VideoSourceFFmpeg::get_frame_rate()
{
    // TODO
    return 0.0;
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
    // TODO
    return -1;

}

}
