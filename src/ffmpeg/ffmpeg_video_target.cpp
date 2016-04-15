#include "ffmpeg_video_target.h"
#include "except.h"

namespace gg
{

VideoTargetFFmpeg::VideoTargetFFmpeg(const std::string codec) :
    _codec(NULL),
    _file_handle(NULL),
    _codec_id(AV_CODEC_ID_NONE),
    _frame(NULL),
    _framerate(-1),
    _sws_context(NULL),
    _format_context(NULL),
    _stream(NULL),
    _frame_index(0)
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

    av_register_all();
}

void VideoTargetFFmpeg::init(const std::string filepath, const float framerate)
{
    if (framerate <= 0)
        throw VideoTargetError("Negative fps does not make sense");
    if (framerate - (int) framerate > 0)
        throw VideoTargetError("Only integer framerates are supported");
    _framerate = (int) framerate;

    if (filepath.empty())
        throw VideoTargetError("Empty filepath specified");
    _filepath = filepath;

    /* allocate the output media context */
    avformat_alloc_output_context2(&_format_context, NULL, NULL, _filepath.c_str());
    if (_format_context == NULL)
        // Use MP4 as default if context cannot be deduced from file extension
        avformat_alloc_output_context2(&_format_context, NULL, "mp4", NULL);
    if (_format_context == NULL)
        throw VideoTargetError("Could not allocate output media context");

    _codec = avcodec_find_encoder(_codec_id);
    if (not _codec)
        throw VideoTargetError("Codec not found");

    _stream = avformat_new_stream(_format_context, _codec);
    if (_stream == NULL)
        throw VideoTargetError("Could not allocate stream");
    _stream->id = _format_context->nb_streams-1; // TODO isn't this wrong?
}

void VideoTargetFFmpeg::append(const VideoFrame_BGRA & frame)
{
    if (_framerate <= 0)
        throw VideoTargetError("Video target not initialised");

    // return value buffers
    int ret, got_output;

    // if first frame, initialise
    if (_frame == NULL)
    {
        // TODO - is _codec_context ever being modified after first frame?
        _stream->codec->codec_id = _codec_id;
        /* TODO: using default reduces filesize
         * but should we set it nonetheless?
         */
//        _stream->codec->bit_rate = 400000;
        /* TODO - resolution must be a multiple of two
         * Why, because it's used that way in the sample
         * code? (Dzhoshkun Shakir)
         */
        _stream->codec->width = frame.cols();
        _stream->codec->height = frame.rows();
        _stream->time_base = (AVRational){ 1, _framerate };
        _stream->codec->time_base = _stream->time_base;
        _stream->codec->gop_size = 12;
        /* TODO emit one intra frame every twelve frames at most */
        _stream->codec->pix_fmt = AV_PIX_FMT_YUV420P;
        /* Some formats want stream headers to be separate. */
        if (_format_context->oformat->flags & AVFMT_GLOBALHEADER)
            _stream->codec->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

        switch (_codec_id)
        {
        case AV_CODEC_ID_H264:
        case AV_CODEC_ID_HEVC:
            /* TODO will this work in real-time with a framegrabber ?
             * "slow" produces 2x larger file compared to "ultrafast",
             * but with a substantial visual quality degradation
             * (judged using the coloured chessboard pattern)
             * "fast" is a trade-off: visual quality looks similar
             * while file size is reasonable
             */
            av_opt_set(_stream->codec->priv_data, "preset", "fast", 0);
            break;
        default:
            // nop
            break;
        }

        /* open it */
        if (avcodec_open2(_stream->codec, _codec, NULL) < 0)
            throw VideoTargetError("Could not open codec");

        _frame = av_frame_alloc();
        if (not _frame)
            throw VideoTargetError("Could not allocate video frame");
        _frame->format = _stream->codec->pix_fmt;
        _frame->width  = _stream->codec->width;
        _frame->height = _stream->codec->height;
        /* allocate the buffers for the frame data */
        ret = av_frame_get_buffer(_frame, 32);
        if (ret < 0)
            throw VideoTargetError("Could not allocate frame data");

        /* Now that all the parameters are set, we can open the audio and
         * video codecs and allocate the necessary encode buffers. */
        av_dump_format(_format_context, 0, _filepath.c_str(), 1);
        /* open the output file, if needed */
        if (!(_format_context->oformat->flags & AVFMT_NOFILE))
        {
            ret = avio_open(&_format_context->pb, _filepath.c_str(), AVIO_FLAG_WRITE);
            if (ret < 0)
            {
                std::string msg;
                msg.append("File ")
                   .append(_filepath)
                   .append(" could not be opened");
                throw VideoTargetError(msg);
            }
        }

        /* Write the stream header, if any. */
        ret = avformat_write_header(_format_context, NULL);
        if (ret < 0)
            throw VideoTargetError("Could not write header to file");

        /* Open context for converting BGRA pixels to YUV420p */
        _sws_context = sws_getContext(
                    frame.cols(), frame.rows(), AV_PIX_FMT_BGRA,
                    frame.cols(), frame.rows(), _stream->codec->pix_fmt,
                    0, NULL, NULL, NULL);
        if (_sws_context == NULL)
            throw VideoTargetError("Could not allocate Sws context");

        // To be incremented for each frame, used as pts
        _frame_index = 0;
    }

    /* when we pass a frame to the encoder, it may keep a reference to it
     * internally;
     * make sure we do not overwrite it here
     */
    ret = av_frame_make_writable(_frame);
    if (ret < 0)
        throw VideoTargetError("Could not make frame writeable");

    /* convert pixel format */
    const uint8_t * src_data_ptr[1] = { frame.data() }; // BGRA has one plane
    int bgra_stride[1] = { 4*frame.cols() };
    sws_scale(_sws_context,
              src_data_ptr, bgra_stride,
              0, frame.rows(),
              _frame->data, _frame->linesize
              );

    _frame->pts = _frame_index++;

    /* encode the image */
    // TODO - make packet member variable to avoid memory allocation / deallocation ?
    AVPacket packet;
    av_init_packet(&packet);
    packet.data = NULL;    // packet data will be allocated by the encoder
    packet.size = 0;

    ret = avcodec_encode_video2(_stream->codec, &packet, _frame, &got_output);
    if (ret < 0)
        throw VideoTargetError("Error encoding frame");

    if (got_output)
    {
        /* rescale output packet timestamp values from codec to stream timebase */
        av_packet_rescale_ts(&packet, _stream->codec->time_base, _stream->time_base);
        // TODO - above time bases are the same, or not?
        packet.stream_index = _stream->index;

        /* Write the compressed frame to the media file. */
        int ret = av_interleaved_write_frame(_format_context, &packet);
        if (ret < 0)
            throw VideoTargetError("Could not interleaved write frame");
//        av_packet_unref(&packet); taken care of by av_interleaved_write_frame
    }

    /* get the delayed frames */
    /* TODO what about delayed frames ?
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
    */
}

void VideoTargetFFmpeg::finalise()
{
    /* Write the trailer, if any. The trailer must be written before you
     * close the CodecContexts open when you wrote the header; otherwise
     * av_write_trailer() may try to use memory that was freed on
     * av_codec_close(). */
    av_write_trailer(_format_context);

//    /* add sequence end code to have a real mpeg file */
//    uint8_t endcode[] = { 0, 0, 1, 0xb7 };
//    if (fwrite(endcode, 1, sizeof(endcode), _file_handle) < sizeof(endcode))
//        throw VideoTargetError("Could not write sequence end");
//    if (fclose(_file_handle) != 0)
//        throw VideoTargetError("Could not close file");

    if (_stream->codec) avcodec_close(_stream->codec);
    if (_stream->codec) av_free(_stream->codec); // TODO this is not in muxing
    if (_frame) av_frame_free(&_frame);
//  av_freep(&_frame->data[0]); no need for this because _frame never manages its own data
    if (_sws_context) sws_freeContext(_sws_context);

    if (!(_format_context->oformat->flags & AVFMT_NOFILE))
        /* Close the output file. */
        avio_closep(&_format_context->pb);
//    avformat_free_context(_format_context);
    /* TODO above line was causing
     * "corrupted double-linked list: 0x0000000000e46e40 ***"
     */

    // default values, for next init
    _codec = NULL;
    _file_handle = NULL;
    _codec_id = AV_CODEC_ID_NONE;
    _frame = NULL;
    _framerate = -1;
    _sws_context = NULL;

    /* free the stream */
//    avformat_free_context(_format_context);
    // TODO - above was giving "double free or corruption (!prev): 0x0000000001a319c0"
    _format_context = NULL; // TODO - risk of dangling
    _stream = NULL; // TODO - risk of dangling
    _frame_index = 0;
}

}
