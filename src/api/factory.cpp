#include "factory.h"
#ifdef USE_OPENCV
#include "opencv_video_source.h"
#include "opencv_video_target.h"
#endif // USE_OPENCV
#ifdef USE_FFMPEG
#include "ffmpeg_video_target.h"
#endif
#ifdef USE_EPIPHANSDK
#include "epiphansdk_video_source.h"
#endif
#ifdef USE_LIBVLC
#include "vlc_video_source.h"
#endif

namespace gg {

IVideoSource * Factory::_sources[2] = { NULL, NULL };

IVideoSource * Factory::connect(enum Device type, enum ColourSpace colour) {
    // if already connected, return
    if (_sources[(int) type] != nullptr) return _sources[(int) type];

    // otherwise, connect
    IVideoSource * src = nullptr;
    switch (type)
    {

    // Epiphan DVI2PCIe Duo DVI ========================================
    case DVI2PCIeDuo_DVI:
        switch (colour)
        {

        // BGRA ========================================
        case BGRA:
#ifdef USE_OPENCV
            src = new VideoSourceOpenCV(0);
#else
            throw VideoSourceError(
                "BGRA colour space on Epiphan DVI2PCIe Duo supported only with OpenCV");
#endif
            break;

        // I420 ========================================
        case I420:
#ifdef USE_LIBVLC
            try
            {
                src = new VideoSourceVLC("v4l2:///dev/video0:chroma=I420");
            }
            catch (VideoSourceError & e)
            {
                throw DeviceNotFound(e.what());
            }
#elif defined(USE_EPIPHANSDK)
#ifdef EpiphanSDK_DVI
            try
            {
                src = new VideoSourceEpiphanSDK(EpiphanSDK_DVI,
                                                V2U_GRABFRAME_FORMAT_I420);
            }
            catch (VideoSourceError & e)
            {
                throw DeviceNotFound(e.what());
            }
#else
            throw DeviceNotFound("EpiphanSDK_DVI macro not defined");
#endif
#else
            throw VideoSourceError(
                "I420 colour space on Epiphan DVI2PCIe Duo supported only with Epiphan SDK or libVLC");
#endif
            break;

        // unsupported colour space ========================================
        default:
            throw VideoSourceError("Colour space not supported");
        }
        break;

    // Epiphan DVI2PCIe Duo SDI ========================================
    case DVI2PCIeDuo_SDI:
        switch (colour)
        {

        // BGRA ========================================
        case BGRA:
#ifdef USE_OPENCV
            src = new VideoSourceOpenCV(1);
#else
            throw VideoSourceError(
                "BGRA colour space on Epiphan DVI2PCIe Duo supported only with OpenCV");
#endif
            break;

        // I420 ========================================
        case I420:
#ifdef USE_LIBVLC
            try
            {
                src = new VideoSourceVLC("v4l2:///dev/video1:chroma=I420");
            }
            catch (VideoSourceError & e)
            {
                throw DeviceNotFound(e.what());
            }
#elif defined(USE_EPIPHANSDK)
#ifdef EpiphanSDK_SDI
            try
            {
                src = new VideoSourceEpiphanSDK(EpiphanSDK_SDI,
                                                V2U_GRABFRAME_FORMAT_I420);
            }
            catch (VideoSourceError & e)
            {
                throw DeviceNotFound(e.what());
            }
#else
            throw DeviceNotFound("EpiphanSDK_SDI macro not defined");
#endif
#else
            throw VideoSourceError(
                "I420 colour space on Epiphan DVI2PCIe Duo supported only with Epiphan SDK or libVLC");
#endif
            break;

        // unsupported colour space ========================================
        default:
            throw VideoSourceError("Colour space not supported");
        }
        break;

    // unsupported device ========================================
    default:
        std::string msg;
        msg.append("Device ")
           .append(std::to_string(type))
           .append(" not recognised");
        throw DeviceNotFound(msg);
    }

    if (src != nullptr)
    {
        // check querying frame dimensions
        int width = -1, height = -1;
        if (not src->get_frame_dimensions(width, height))
        {
            throw DeviceOffline(
                "Device connected but does not return frame dimensions");
        }

        // check meaningful frame dimensions
        if (width <= 0 or height <= 0)
        {
            throw DeviceOffline(
                "Device connected but returns meaningless frame dimensions");
        }

        // check querying frames
        VideoFrame frame(colour, true);
        if (not src->get_frame(frame))
        {
            throw DeviceOffline(
                "Device connected does not return frames");
        }

        // if no exception raised up to here, glory be to GIFT-Grab
        _sources[(int) type] = src;
    }

    return _sources[(int) type];
}

void Factory::disconnect(enum Device type) {
    switch (type) {
    case DVI2PCIeDuo_DVI:
    case DVI2PCIeDuo_SDI:
        break; // everything up to here is recognised
    default:
        std::string msg;
        msg.append("Device ")
           .append(std::to_string(type))
           .append(" not recognised");
        throw DeviceNotFound(msg);
    }

    if (_sources[(int) type])
    {
        delete _sources[(int) type];
        _sources[(int) type] = NULL;
    }
}

IVideoTarget * Factory::writer(Storage type)
{
    switch (type)
    {
    case File_XviD:
#ifdef USE_OPENCV
        return new VideoTargetOpenCV("XVID");
#endif // USE_OPENCV
    case File_HEVC:
#ifdef USE_FFMPEG
        return new VideoTargetFFmpeg("HEVC");
#else
        // nop, see default below
#endif
    case File_VP9:
#ifdef USE_FFMPEG
        return new VideoTargetFFmpeg("VP9");
#else
        // nop, see default below
#endif
    default:
        std::string msg;
        msg.append("Video target type ")
           .append(std::to_string(type))
           .append(" not supported");
        throw VideoTargetError(msg);
    }
}

}
