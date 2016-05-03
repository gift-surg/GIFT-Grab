#include "ffmpeg_video_target.h"
#include "except.h"

#ifdef USE_BOOST_TIMER
#include <boost/timer/timer.hpp>
#endif

namespace gg
{

VideoTargetFFmpeg::VideoTargetFFmpeg(const std::string codec) :
    _codec(NULL),
    _codec_name(""),
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
#ifdef FFMPEG_HWACCEL
    _codec_name = "nvenc_hevc";
#else
    _codec_name = "libx265";
#endif

    av_register_all();
}

void VideoTargetFFmpeg::init(const std::string filepath, const float framerate)
{
    if (framerate <= 0)
        throw VideoTargetError("Negative fps does not make sense");
    if (framerate - (int) framerate > 0)
        throw VideoTargetError("Only integer framerates are supported");
    _framerate = (int) framerate;

    check_filetype_support(filepath, "mp4");

    _filepath = filepath;

    /* allocate the output media context */
    avformat_alloc_output_context2(&_format_context, NULL, NULL, _filepath.c_str());
    if (_format_context == NULL)
        // Use MP4 as default if context cannot be deduced from file extension
        avformat_alloc_output_context2(&_format_context, NULL, "mp4", NULL);
    if (_format_context == NULL)
        throw VideoTargetError("Could not allocate output media context");

    _codec = avcodec_find_encoder_by_name(_codec_name.c_str());
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
#ifdef USE_BOOST_TIMER
#ifndef timer_format_str
#define timer_format_str \
        std::string(", %w, %u, %s, %t, %p" \
                    ", wall (s), user (s), system (s), user+system (s), CPU (pc)\n")
#endif
        boost::timer::auto_cpu_timer t("a) _frame == NULL" + timer_format_str);
#endif
        // TODO - is _codec_context ever being modified after first frame?
        /* TODO: using default reduces filesize
         * but should we set it nonetheless?
         */
//        _stream->codec->bit_rate = 400000;
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

        switch (_stream->codec->codec_id)
        {
        case AV_CODEC_ID_H264:
        case AV_CODEC_ID_HEVC:
#ifdef FFMPEG_HWACCEL
            // nop
            ret = 0;
#else
            /* TODO will this work in real-time with a framegrabber ?
             * "slow" produces 2x larger file compared to "ultrafast",
             * but with a substantial visual quality degradation
             * (judged using the coloured chessboard pattern)
             * "fast" is a trade-off: visual quality looks similar
             * while file size is reasonable
             */
            ret = av_opt_set(_stream->codec->priv_data, "preset", "fast", 0);
#endif
            if (ret != 0)
                throw VideoTargetError("Could not set codec-specific options");

            /* Resolution must be a multiple of two, as required
             * by H264 and H265. Introduce a one-pixel padding for
             * non-complying dimension(s).
             */
            _stream->codec->width +=
                    _stream->codec->width % 2 == 0 ? 0 : 1;
            _stream->codec->height +=
                    _stream->codec->height % 2 == 0 ? 0 : 1;
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
        // TODO #25 what influence does 32 have on performance?
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

        // Packet to be used when writing frames
        av_init_packet(&_packet);
        // TODO #25 data gets allocated each time?
        _packet.data = NULL;    // packet data will be allocated by the encoder
        _packet.size = 0;
    }

    { // START auto_cpu_timer scope
#ifdef USE_BOOST_TIMER
    boost::timer::auto_cpu_timer t("a) av_frame_make_writable" + timer_format_str);
#endif

    /* when we pass a frame to the encoder, it may keep a reference to it
     * internally;
     * make sure we do not overwrite it here
     */
    // TODO #25 av_buffer_is_writable(_frame) ?= 1
    ret = av_frame_make_writable(_frame);
    // TODO #25 av_buffer_is_writable(_frame) ?= 1, also why not only once?
    if (ret < 0)
        throw VideoTargetError("Could not make frame writeable");

    } // END auto_cpu_timer scope

    { // START auto_cpu_timer scope
#ifdef USE_BOOST_TIMER
    boost::timer::auto_cpu_timer t("a) sws_scale" + timer_format_str);
#endif

    /* convert pixel format */
    // TODO #25 allocate once
    const uint8_t * src_data_ptr[1] = { frame.data() }; // BGRA has one plane
    // TODO #25 allocate once
    int bgra_stride[1] = { 4*frame.cols() };
    sws_scale(_sws_context,
              src_data_ptr, bgra_stride,
              0, frame.rows(),
              // #25 TODO _frame->data getting allocated here?
              _frame->data, _frame->linesize
              );

    _frame->pts = _frame_index++;

    } // END auto_cpu_timer scope

    { // START auto_cpu_timer scope
#ifdef USE_BOOST_TIMER
    boost::timer::auto_cpu_timer t("a) encode_and_write" + timer_format_str);
#endif

    /* encode the image */
    encode_and_write(_frame, got_output);

    } // END auto_cpu_timer scope
}

void VideoTargetFFmpeg::encode_and_write(AVFrame * frame, int & got_output)
{
    int ret;

    {
#ifdef USE_BOOST_TIMER
    boost::timer::auto_cpu_timer t("aa) avcodec_encode_video2" + timer_format_str);
#endif
    ret = avcodec_encode_video2(_stream->codec, &_packet, frame, &got_output);
    }

    if (ret < 0)
        throw VideoTargetError("Error encoding frame");

    if (got_output)
    {
        {
    #ifdef USE_BOOST_TIMER
        boost::timer::auto_cpu_timer t("aa) av_packet_rescale_ts" + timer_format_str);
    #endif
        /* rescale output packet timestamp values from codec to stream timebase */
        av_packet_rescale_ts(&_packet, _stream->codec->time_base, _stream->time_base);
        // TODO - above time bases are the same, or not?
        _packet.stream_index = _stream->index;
        }

        {
    #ifdef USE_BOOST_TIMER
        boost::timer::auto_cpu_timer t("aa) av_interleaved_write_frame" + timer_format_str);
    #endif
        /* Write the compressed frame to the media file. */
        int ret = av_interleaved_write_frame(_format_context, &_packet);
        }

        if (ret < 0)
            throw VideoTargetError("Could not interleaved write frame");
//        av_packet_unref(&packet); taken care of by av_interleaved_write_frame
    }
}

void VideoTargetFFmpeg::finalise()
{
    /* get the delayed frames */
    for (int got_output = 1; got_output; )
        encode_and_write(NULL, got_output);

    /* Write the trailer, if any. The trailer must be written before you
     * close the CodecContexts open when you wrote the header; otherwise
     * av_write_trailer() may try to use memory that was freed on
     * av_codec_close(). */
    av_write_trailer(_format_context);

    if (_stream->codec) avcodec_close(_stream->codec);
    if (_frame) av_frame_free(&_frame);
//  av_freep(&_frame->data[0]); no need for this because _frame never manages its own data
    if (_sws_context) sws_freeContext(_sws_context);

    if (!(_format_context->oformat->flags & AVFMT_NOFILE))
        /* Close the output file. */
        avio_closep(&_format_context->pb);
    /* free the stream */
    avformat_free_context(_format_context);

    // default values, for next init
    _codec = NULL;
    _codec_name = "";
    _frame = NULL;
    _framerate = -1;
    _sws_context = NULL;
    _format_context = NULL;
    _stream = NULL;
    _frame_index = 0;
}

}
