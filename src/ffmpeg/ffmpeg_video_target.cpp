#include "ffmpeg_video_target.h"
#include "except.h"

#ifdef GENERATE_PERFORMANCE_OUTPUT
#include <boost/timer/timer.hpp>
#ifndef timer_format_str
#define timer_format_str \
        std::string(", %w, %u, %s, %t, %p" \
                    ", wall (s), user (s), system (s), user+system (s), CPU (\%)\n")
#endif
#ifndef this_class_str
#define this_class_str std::string("VideoTargetFFmpeg-")
#endif
#endif

namespace gg
{

const std::string VideoTargetFFmpeg::_CODEC_NAME_HEVC_X265 = "libx265";

const std::string VideoTargetFFmpeg::_CODEC_NAME_HEVC_KVAZAAR = "libkvazaar";

const std::string VideoTargetFFmpeg::_CODEC_NAME_HEVC_NVENC = "nvenc_hevc";

const std::string VideoTargetFFmpeg::_CODEC_NAME_VP9_LIBVPX = "libvpx-vp9";


VideoTargetFFmpeg::VideoTargetFFmpeg(const std::string codec,
                                     const std::string filename,
                                     const float frame_rate) :
    _is_finaliseable(false),
    _codec(NULL),
    _codec_name(""),
    _frame(NULL),
    _pixel_depth(0),
    _first_frame(true),
    _framerate(-1),
    _sws_context(NULL),
    _format_context(NULL),
    _stream(NULL),
    _frame_index(0)
{
    if (codec == "HEVC")
    {
#ifdef USE_NVENC
        _codec_name = _CODEC_NAME_HEVC_NVENC;
#else
#ifdef USE_X265
        _codec_name = _CODEC_NAME_HEVC_X265;
#else
        _codec_name = _CODEC_NAME_HEVC_KVAZAAR;
#endif
#endif
    }
    else if (codec == "VP9")
    {
        _codec_name = _CODEC_NAME_VP9_LIBVPX;
    }
    else
    {
        std::string msg;
        msg.append("Codec ")
           .append(codec)
           .append(" not recognised");
        throw VideoTargetError(msg);
    }

    av_register_all();

    init(filename, frame_rate);
}

VideoTargetFFmpeg::~VideoTargetFFmpeg()
{
    finalise();
}

void VideoTargetFFmpeg::init(const std::string filepath, const float framerate)
{
    if (framerate <= 0)
        throw VideoTargetError("Negative fps does not make sense");
    _framerate = framerate;

    if (_codec_name == _CODEC_NAME_HEVC_X265 or
        _codec_name == _CODEC_NAME_HEVC_NVENC)
        check_filetype_support(filepath, "mp4");
    else if (_codec_name == _CODEC_NAME_VP9_LIBVPX)
        check_filetype_support(filepath, "webm");

    _filepath = filepath;

    /* allocate the output media context */
    avformat_alloc_output_context2(&_format_context, NULL, NULL, _filepath.c_str());
    if (_format_context == NULL)
        throw VideoTargetError("Could not allocate output media context");

    _codec = avcodec_find_encoder_by_name(_codec_name.c_str());
    if (not _codec)
        throw VideoTargetError("Codec not found");

    _stream = avformat_new_stream(_format_context, _codec);
    if (_stream == NULL)
        throw VideoTargetError("Could not allocate stream");
    _stream->id = _format_context->nb_streams-1; // TODO isn't this wrong?

    // allocate FFmpeg frame
    _frame = av_frame_alloc();
    if (not _frame)
        throw VideoTargetError("Could not allocate video frame");
    _first_frame = true;
}

void VideoTargetFFmpeg::append(const VideoFrame & frame)
{
    // Issue #130
    _is_finaliseable = true;

    { // START auto_cpu_timer scope
#ifdef GENERATE_PERFORMANCE_OUTPUT
    boost::timer::auto_cpu_timer t(this_class_str + "1-ffmpeg_frame" + timer_format_str);
#endif
    AVPixelFormat colour;
    switch (frame.colour())
    {
    case ColourSpace::BGRA:
        colour = AV_PIX_FMT_BGRA;
        break;
    case ColourSpace::I420:
        colour = AV_PIX_FMT_YUV420P;
        break;
    case ColourSpace::UYVY:
        colour = AV_PIX_FMT_UYVY422;
        break;
    default:
        throw VideoTargetError("Only BGRA and I420 colour spaces supported currently");
        break;
    }

    ffmpeg_frame(frame.data(), frame.data_length(),
                 frame.cols(), frame.rows(),
                 colour, _frame);
    } // END auto_cpu_timer scope

    { // START auto_cpu_timer scope
#ifdef GENERATE_PERFORMANCE_OUTPUT
    boost::timer::auto_cpu_timer t(this_class_str + "1-encode_and_write" + timer_format_str);
#endif

    /* encode the image */
    int got_output;
    encode_and_write(_frame, got_output);

    } // END auto_cpu_timer scope
}

void VideoTargetFFmpeg::ffmpeg_frame(const unsigned char * data,
                                     const size_t data_length,
                                     const int width, const int height,
                                     const AVPixelFormat colour_space,
                                     AVFrame * frame)
{
    if (_framerate <= 0)
        throw VideoTargetError("Video target not initialised");

    if (not frame)
        throw VideoTargetError("FFmpeg frame not initialised");

    // return value buffers
    int ret;

    // if first frame, initialise
    if (_first_frame)
    {
#ifdef GENERATE_PERFORMANCE_OUTPUT
        boost::timer::auto_cpu_timer t(this_class_str + "2-first-frame" + timer_format_str);
#endif
        // TODO - is _codec_context ever being modified after first frame?
        /* TODO: using default reduces filesize
         * but should we set it nonetheless?
         */
//        _stream->codec->bit_rate = 400000;
        _stream->codec->width = width;
        _stream->codec->height = height;
        _stream->time_base = (AVRational){ 1000, static_cast<int>(1000 * _framerate) };
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
#ifdef USE_NVENC
            // nop
            ret = 0;
#else
#ifdef USE_X265
            /* TODO will this work in real-time with a framegrabber ?
             * "slow" produces 2x larger file compared to "ultrafast",
             * but with a substantial visual quality degradation
             * (judged using the coloured chessboard pattern)
             * "fast" is a trade-off: visual quality looks similar
             * while file size is reasonable
             */
            ret = av_opt_set(_stream->codec->priv_data, "preset", "fast", 0);
#else
            // nop for kvazaar
            ret = 0;
#endif
#endif
            if (ret != 0)
                throw VideoTargetError("Could not set codec-specific options");

            /* Resolution must be a multiple of two, as required
             * by H264 and HEVC. Introduce a one-pixel padding for
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

        frame->format = _stream->codec->pix_fmt;
        frame->width  = _stream->codec->width;
        frame->height = _stream->codec->height;
        /* allocate the buffers for the frame data */
        switch(colour_space)
        {
        case AV_PIX_FMT_BGRA:
            _pixel_depth = 32; // bits-per-pixel
            break;
        case AV_PIX_FMT_YUV420P:
            _pixel_depth = 12; // bits-per-pixel
            /* TODO #54 - alignment by _pixel_depth causes problems
             * due to buffer size mismatches between EpiphanSDK and
             * FFmpeg, hence manually filling in linesizes (based on
             * debugging measurements), in conj. with
             * av_image_fill_pointers below
             */
            frame->linesize[0] = frame->width;
            frame->linesize[1] = frame->width / 2;
            frame->linesize[2] = frame->width / 2;
            break;
        case AV_PIX_FMT_UYVY422:
            _pixel_depth = 16; // bits-per-pixel
            break;
        default:
            throw VideoTargetError("Colour space not supported");
        }
        ret = av_frame_get_buffer(frame, _pixel_depth);
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

        /* TODO USE_I420 also implicitly provides
         * this check, but selective code compilation might make
         * code faster, due to not having the conditional checks
         */
        switch(colour_space)
        {
        case AV_PIX_FMT_BGRA:
        case AV_PIX_FMT_UYVY422:
            /* Open context for converting BGRA pixels to YUV420p */
            _sws_context = sws_getContext(
                        width, height, colour_space,
                        width, height, _stream->codec->pix_fmt,
                        0, NULL, NULL, NULL);
            if (_sws_context == NULL)
                throw VideoTargetError("Could not allocate Sws context");
            break;
        case AV_PIX_FMT_YUV420P:
            // nop
            break;
        default:
            throw VideoTargetError("Colour space not supported");
        }

        // To be incremented for each frame, used as pts
        _frame_index = 0;

        // Packet to be used when writing frames
        av_init_packet(&_packet);
        // TODO #25 data gets allocated each time?
        _packet.data = NULL;    // packet data will be allocated by the encoder
        _packet.size = 0;

        switch(colour_space)
        {
        case AV_PIX_FMT_BGRA:
            _stride[0] = 4 * width;
            break;
        case AV_PIX_FMT_UYVY422:
            _stride[0] = 2 * width;
            break;
        case AV_PIX_FMT_YUV420P:
        default:
            // nop
            break;
        }

        _first_frame = false;
    }

    { // START auto_cpu_timer scope
#ifdef GENERATE_PERFORMANCE_OUTPUT
    boost::timer::auto_cpu_timer t(this_class_str + "2-av_frame_make_writable" + timer_format_str);
#endif

    /* when we pass a frame to the encoder, it may keep a reference to it
     * internally;
     * make sure we do not overwrite it here
     */
    // TODO #25 why not only once?
    ret = av_frame_make_writable(frame);
    if (ret < 0)
        throw VideoTargetError("Could not make frame writeable");

    } // END auto_cpu_timer scope

    { // START auto_cpu_timer scope
#ifdef GENERATE_PERFORMANCE_OUTPUT
    boost::timer::auto_cpu_timer t(this_class_str + "2-sws_scale" + timer_format_str);
#endif

    /* TODO USE_I420 also implicitly provides
     * this check, but selective code compilation might make
     * code faster, due to not having the conditional checks
     */
    switch(colour_space)
    {
    case AV_PIX_FMT_BGRA:
    case AV_PIX_FMT_UYVY422:
        _src_data_ptr[0] = data;
        /* convert pixel format */
        sws_scale(_sws_context,
                  _src_data_ptr, _stride, // BGRA and UYVY have one plane each
                  0, height,
                  frame->data, frame->linesize
                  );
        break;
    case AV_PIX_FMT_YUV420P:
        /* TODO #54 - alignment by _pixel_depth causes problems
         * due to buffer size mismatches between EpiphanSDK and
         * FFmpeg, hence using this instead of commented out
         * code below
         */
        av_image_fill_pointers(frame->data, (AVPixelFormat) frame->format,
                               frame->height, const_cast<uint8_t *>(data), frame->linesize);
//        av_image_fill_arrays(frame->data, frame->linesize,
//                             data, (AVPixelFormat) frame->format,
//                             frame->width, frame->height, _pixel_depth);
        break;
    default:
        throw VideoTargetError("Colour space not supported");
    }

    frame->pts = _frame_index++;

    } // END auto_cpu_timer scope
}

void VideoTargetFFmpeg::encode_and_write(AVFrame * frame, int & got_output)
{
    int ret;

    { // START auto_cpu_timer scope
#ifdef GENERATE_PERFORMANCE_OUTPUT
    boost::timer::auto_cpu_timer t(this_class_str + "2-avcodec_encode_video2" + timer_format_str);
#endif
    ret = avcodec_encode_video2(_stream->codec, &_packet, frame, &got_output);
    } // END auto_cpu_timer scope

    if (ret < 0)
        throw VideoTargetError("Error encoding frame");

    if (got_output)
    {
        { // START auto_cpu_timer scope
#ifdef GENERATE_PERFORMANCE_OUTPUT
        boost::timer::auto_cpu_timer t(this_class_str + "2-av_packet_rescale_ts" + timer_format_str);
#endif
        /* rescale output packet timestamp values from codec to stream timebase */
        av_packet_rescale_ts(&_packet, _stream->codec->time_base, _stream->time_base);
        // TODO - above time bases are the same, or not?
        _packet.stream_index = _stream->index;
        } // END auto_cpu_timer scope

        { // START auto_cpu_timer scope
#ifdef GENERATE_PERFORMANCE_OUTPUT
        boost::timer::auto_cpu_timer t(this_class_str + "2-av_interleaved_write_frame" + timer_format_str);
#endif
        /* Write the compressed frame to the media file. */
        ret = av_interleaved_write_frame(_format_context, &_packet);
        }

        if (ret < 0)
            throw VideoTargetError("Could not interleaved write frame");
//        av_packet_unref(&packet); taken care of by av_interleaved_write_frame
    }
}

void VideoTargetFFmpeg::finalise()
{
    // Issue #130
    if (not _is_finaliseable)
        return;

    /* This condition means that append
     * has been called at least once
     * successfully (see Issue#36)
     */
    if (_frame)
    {
        /* get the delayed frames */
        for (int got_output = 1; got_output; )
            encode_and_write(NULL, got_output);

        /* Write the trailer, if any. The trailer must be written before you
         * close the CodecContexts open when you wrote the header; otherwise
         * av_write_trailer() may try to use memory that was freed on
         * av_codec_close(). */
        av_write_trailer(_format_context);
    }

    if (_stream->codec) avcodec_close(_stream->codec);
    if (_frame) av_frame_free(&_frame);
//  av_freep(&_frame->data[0]); no need for this because _frame never manages its own data
    if (_sws_context) sws_freeContext(_sws_context);

    if (!(_format_context->oformat->flags & AVFMT_NOFILE))
        if (_format_context->pb)
            /* Close the output file. */
            avio_closep(&_format_context->pb);
    /* free the stream */
    avformat_free_context(_format_context);

    // default values, for next init
    _codec = NULL;
    _frame = NULL;
    _pixel_depth = 0;
    _first_frame = true;
    _framerate = -1;
    _sws_context = NULL;
    _format_context = NULL;
    _stream = NULL;
    _frame_index = 0;
}

}
