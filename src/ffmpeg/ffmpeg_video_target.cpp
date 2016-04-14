#include "ffmpeg_video_target.h"
#include "except.h"

namespace gg
{

VideoTargetFFmpeg::VideoTargetFFmpeg(const std::string codec) :
    _codec(NULL),
    _codec_context(NULL),
    _file_handle(NULL),
    _codec_id(AV_CODEC_ID_NONE),
    _frame(NULL),
    _framerate(-1),
    _sws_context(NULL)
{
    if (codec != "H265")
    {
        std::string msg;
        msg.append("Codec ")
           .append(codec)
           .append(" not recognised");
        throw VideoTargetError(msg);
    }
    _codec_id = AV_CODEC_ID_HEVC;

    avcodec_register_all();
}

void VideoTargetFFmpeg::init(const std::string filepath, const float framerate)
{
    if (framerate <= 0)
        throw VideoTargetError("Negative fps does not make sense");
    if (framerate - (int) framerate > 0)
        throw VideoTargetError("Only integer framerates are supported");

    _framerate = (int) framerate;

    _file_handle = fopen(filepath.c_str(), "wb");
    if (not _file_handle)
    {
        std::string msg;
        msg.append("File ")
           .append(filepath)
           .append(" could not be opened");
        throw VideoTargetError(msg);
    }
}

void VideoTargetFFmpeg::append(const VideoFrame_BGRA & frame)
{
    if (_framerate <= 0 or not _file_handle)
        throw VideoTargetError("Video target not initialised");

    // return value buffers
    int ret, got_output;

    // if first frame, initialise
    if (_codec == NULL or _codec_context == NULL or _frame == NULL)
    {
        /* find the mpeg1 video encoder */
        _codec = avcodec_find_encoder(_codec_id);
        if (not _codec)
            throw VideoTargetError("Codec not found");

        _codec_context = avcodec_alloc_context3(_codec);
        if (not _codec_context)
            throw VideoTargetError("Could not allocate video codec context");

        /* TODO put sample parameters */
        _codec_context->bit_rate = 400000;
        /* TODO - resolution must be a multiple of two */
        _codec_context->width = frame.cols();
        _codec_context->height = frame.rows();
        /* frames per second */
        _codec_context->time_base = (AVRational){1, _framerate};
        /* TODO
         * emit one intra frame every ten frames
         * check frame pict_type before passing frame
         * to encoder, if frame->pict_type is AV_PICTURE_TYPE_I
         * then gop_size is ignored and the output of encoder
         * will always be I frame irrespective to gop_size
         */
        _codec_context->gop_size = 10;
        _codec_context->max_b_frames = 1; // TODO
        _codec_context->pix_fmt = AV_PIX_FMT_YUV420P;

        switch (_codec_id)
        {
        case AV_CODEC_ID_H264:
        case AV_CODEC_ID_HEVC:
            av_opt_set(_codec_context->priv_data, "preset", "slow", 0);
            break;
        default:
            // nop
            break;
        }

        /* open it */
        if (avcodec_open2(_codec_context, _codec, NULL) < 0)
            throw VideoTargetError("Could not open codec");

        _frame = av_frame_alloc();
        if (not _frame)
            throw VideoTargetError("Could not allocate video frame");
        _frame->format = _codec_context->pix_fmt;
        _frame->width  = _codec_context->width;
        _frame->height = _codec_context->height;
        _frame->pts = 1; // i.e. only one frame

        /* TODO
         * the image can be allocated by any means and av_image_alloc() is
         * just the most convenient way if av_malloc() is to be used */
        ret = av_image_alloc(_frame->data, _frame->linesize,
                             _codec_context->width, _codec_context->height,
                             _codec_context->pix_fmt, 32);
        if (ret < 0)
            throw VideoTargetError("Could not allocate raw picture buffer");

        _sws_context = sws_getContext(
                    frame.cols(), frame.rows(), AV_PIX_FMT_BGRA,
                    frame.cols(), frame.rows(), _codec_context->pix_fmt,
                    0, NULL, NULL, NULL);
        if (_sws_context == NULL)
            throw VideoTargetError("Could not allocate Sws context");
    }

    /* convert pixel format if necessary */
    const uint8_t * src_data_ptr[1] = { frame.data() }; // BGRA has one plane
    int bgra_stride[1] = { 4*frame.cols() };
    sws_scale(_sws_context,
              src_data_ptr, bgra_stride,
              0, frame.rows(),
              _frame->data, _frame->linesize
              );

    /* encode the image */
    // TODO - make packet member variable to avoid memory allocation / deallocation ?
    AVPacket packet;
    av_init_packet(&packet);
    packet.data = NULL;    // packet data will be allocated by the encoder
    packet.size = 0;

    ret = avcodec_encode_video2(_codec_context, &packet, _frame, &got_output);
    if (ret < 0)
        throw VideoTargetError("Error encoding frame");

    if (got_output)
    {
        if (fwrite(packet.data, 1, packet.size, _file_handle) < packet.size)
            throw VideoTargetError("Could not write packet data");
        av_packet_unref(&packet);
    }

    /* get the delayed frames */
    for (got_output = 1; got_output; ) {
        ret = avcodec_encode_video2(_codec_context, &packet, NULL, &got_output);
        if (ret < 0)
            throw VideoTargetError("Error encoding frame");

        if (got_output)
        {
            if (fwrite(packet.data, 1, packet.size, _file_handle) < packet.size)
                throw VideoTargetError("Could not write packet data (delayed frames)");
            av_packet_unref(&packet);
        }
    }

    // TODO

    // TODO - exceptions when write/open/close fails!!!
}

void VideoTargetFFmpeg::finalise()
{
    /* add sequence end code to have a real mpeg file */
    uint8_t endcode[] = { 0, 0, 1, 0xb7 };
    if (fwrite(endcode, 1, sizeof(endcode), _file_handle) < sizeof(endcode))
        throw VideoTargetError("Could not write sequence end");
    if (fclose(_file_handle) != 0)
        throw VideoTargetError("Could not close file");

    if (_codec_context) avcodec_close(_codec_context);
    if (_codec_context) av_free(_codec_context);
//  av_freep(&_frame->data[0]); no need for this because _frame never manages its own data
    if (_frame) av_frame_free(&_frame);
    if (_sws_context) sws_freeContext(_sws_context);

    // default values, for next init
    _codec = NULL;
    _codec_context = NULL;
    _file_handle = NULL;
    _codec_id = AV_CODEC_ID_NONE;
    _frame = NULL;
    _framerate = -1;
    _sws_context = NULL;
}

}
