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
    _framerate(-1)
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

    // TODO
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

        /* TODO
         * the image can be allocated by any means and av_image_alloc() is
         * just the most convenient way if av_malloc() is to be used */
        ret = av_image_alloc(_frame->data, _frame->linesize,
                             _codec_context->width, _codec_context->height,
                             _codec_context->pix_fmt, 32);
        if (ret < 0)
            throw VideoTargetError("Could not allocate raw picture buffer");
    }

    // TODO
    AVPacket pkt;
    // TODO
//    int i, ret, x, y, got_output;

    /* encode 1 second of video */
//    for (i = 0; i < 10*25; i++) {
        av_init_packet(&pkt);
        pkt.data = NULL;    // packet data will be allocated by the encoder
        pkt.size = 0;

        fflush(stdout);
        cv::Mat buffer_yuv420p_tmp, buffer_yuv420p;

        cv::Mat cv_frame_bgra(frame.rows(), frame.cols(),
                              CV_8UC4,
                              const_cast<unsigned char *>(frame.data()));
        cv::cvtColor(cv_frame_bgra, buffer_yuv420p_tmp, CV_BGRA2BGR);
        cv::cvtColor(buffer_yuv420p_tmp, buffer_yuv420p,
                     CV_RGB2YUV); // see https://github.com/Itseez/opencv/issues/4946

        std::vector<cv::Mat> ycbcr_planes;
        cv::split(buffer_yuv420p, ycbcr_planes);
        _frame->data[0] = ycbcr_planes[0].data;
        _frame->data[1] = ycbcr_planes[1].data;
        _frame->data[2] = ycbcr_planes[2].data;
        /* prepare a dummy image * /
        /* Y * /
        for (y = 0; y < _codec_context->height; y++) {
            for (x = 0; x < _codec_context->width; x++) {
                _frame->data[0][y * _frame->linesize[0] + x] = x + y + i * 3;
            }
        }

        /* Cb and Cr * /
        for (y = 0; y < _codec_context->height/2; y++) {
            for (x = 0; x < _codec_context->width/2; x++) {
                _frame->data[1][y * _frame->linesize[1] + x] = 128 + y + i * 2;
                _frame->data[2][y * _frame->linesize[2] + x] = 64 + x + i * 5;
            }
        }
        */

//        _frame->pts = i;
        _frame->pts = 1; // i.e. only one frame

        /* encode the image */
        // TODO - allocate pkt.data ?
        ret = avcodec_encode_video2(_codec_context, &pkt, _frame, &got_output);
        if (ret < 0)
            throw VideoTargetError("Error encoding frame");

        if (got_output)
        {
//            printf("Write frame %3d (size=%5d)\n", i, pkt.size);
            if (fwrite(pkt.data, 1, pkt.size, _file_handle) < pkt.size)
                throw VideoTargetError("Could not write packet data");
            av_packet_unref(&pkt);
        }
//    }

    /* get the delayed frames */
//    for (got_output = 1; got_output; i++) {
    for (got_output = 1; got_output; ) {
        fflush(stdout);

        ret = avcodec_encode_video2(_codec_context, &pkt, NULL, &got_output);
        if (ret < 0)
            throw VideoTargetError("Error encoding frame");

        if (got_output)
        {
//            printf("Write frame %3d (size=%5d)\n", i, pkt.size);
            if (fwrite(pkt.data, 1, pkt.size, _file_handle) < pkt.size)
                throw VideoTargetError("Could not write packet data (delayed frames)");
            av_packet_unref(&pkt);
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

    // default values, for next init
    _codec = NULL;
    _codec_context = NULL;
    _file_handle = NULL;
    _codec_id = AV_CODEC_ID_NONE;
    _frame = NULL;
    _framerate = -1;
}

}
