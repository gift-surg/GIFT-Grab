#include "videosourcefactory.h"
#ifdef USE_OPENCV
#include "opencv_video_source.h"
#endif
#ifdef USE_EPIPHANSDK
#include "epiphansdk_video_source.h"
#endif
#ifdef USE_LIBVLC
#include "vlc_video_source.h"
#endif
#ifdef USE_BLACKMAGICSDK
#include "blackmagicsdk_video_source.h"
#endif
#ifdef USE_FFMPEG
#include "ffmpeg_video_source.h"
#endif

namespace gg
{

VideoSourceFactory VideoSourceFactory::_factory_singleton;

VideoSourceFactory::VideoSourceFactory()
{
    for (size_t i = 0; i < NUM_DEVICES; i++)
        _devices[i] = nullptr;
}

VideoSourceFactory::~VideoSourceFactory()
{
    free_device(DVI2PCIeDuo_DVI);
    free_device(DVI2PCIeDuo_SDI);
}

VideoSourceFactory & VideoSourceFactory::get_instance()
{
    return _factory_singleton;
}

IVideoSource * VideoSourceFactory::get_device(Device device,
                                              ColourSpace colour)
{
    // if already connected, return
    if (_devices[(int) device] != nullptr)
    {
        if (_device_colours[(int) device] != colour)
            throw DeviceAlreadyConnected(
                "Device already connected using another colour space"
            );
        else
            return _devices[(int) device];
    }

    // otherwise, connect
    IVideoSource * src = nullptr;
    switch (device)
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
                src = new VideoSourceVLC("v4l2:///dev/video0:chroma=I420", 60);
            }
            catch (VideoSourceError & e)
            {
                throw DeviceNotFound(e.what());
            }
#elif defined(USE_EPIPHANSDK)
#ifdef Epiphan_DVI2PCIeDuo_DVI
            try
            {
                src = new VideoSourceEpiphanSDK(Epiphan_DVI2PCIeDuo_DVI,
                                                V2U_GRABFRAME_FORMAT_I420);
            }
            catch (VideoSourceError & e)
            {
                throw DeviceNotFound(e.what());
            }
#else
            throw DeviceNotFound("Epiphan_DVI2PCIeDuo_DVI macro not defined");
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
                src = new VideoSourceVLC("v4l2:///dev/video1:chroma=I420", 60);
            }
            catch (VideoSourceError & e)
            {
                throw DeviceNotFound(e.what());
            }
#elif defined(USE_EPIPHANSDK)
#ifdef Epiphan_DVI2PCIeDuo_SDI
            try
            {
                src = new VideoSourceEpiphanSDK(Epiphan_DVI2PCIeDuo_SDI,
                                                V2U_GRABFRAME_FORMAT_I420);
            }
            catch (VideoSourceError & e)
            {
                throw DeviceNotFound(e.what());
            }
#else
            throw DeviceNotFound("Epiphan_DVI2PCIeDuo_SDI macro not defined");
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

    // Blackmagic DeckLink SDI 4K ========================================
    case DeckLinkSDI4K:
#ifdef USE_BLACKMAGICSDK
        switch (colour)
        {

        // BGRA ========================================
        case BGRA:
        // UYVY ========================================
        case UYVY:
            break;

        // I420 ========================================
        case I420:
        default:
            throw VideoSourceError("Colour space not supported for"
                                   " Blackmagic DeckLink SDI 4K");
        }
        src = new VideoSourceBlackmagicSDK(0, colour);
#else
        throw VideoSourceError(
            "Blackmagic DeckLink SDI 4K supported only through"
            " Blackmagic Desktop Video SDK");
#endif
        break;

    // unsupported device ========================================
    default:
        std::string msg;
        msg.append("Device ")
           .append(std::to_string(device))
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
        _devices[(int) device] = src;
        _device_colours[(int) device] = colour;
    }

    return _devices[(int) device];
}

void VideoSourceFactory::free_device(Device device)
{
    switch (device)
    {
    case DVI2PCIeDuo_DVI:
    case DVI2PCIeDuo_SDI:
    case DeckLinkSDI4K:
        break; // everything up to here is recognised
    default:
        std::string msg;
        msg.append("Device ")
           .append(std::to_string(device))
           .append(" not recognised");
        throw DeviceNotFound(msg);
    }

    if (_devices[(int) device] != nullptr)
        delete _devices[(int) device];
    _devices[(int) device] = nullptr;
}


IVideoSource * VideoSourceFactory::connect_network_source(
        std::string address, enum ColourSpace colour)
{
    IVideoSource * source = nullptr;
    switch (colour)
    {
    case I420:
#ifdef USE_LIBVLC
        try
        {
            source = new gg::VideoSourceVLC(address);
        }
        catch (VideoSourceError & e)
        {
            throw NetworkSourceUnavailable(e.what());
        }
#else
        throw VideoSourceError(
            "I420 colour space on network sources supported only with libVLC"
        );
#endif
        break;
    case BGRA:
#ifdef USE_OPENCV
        try
        {
            source = new VideoSourceOpenCV(address.c_str());
        }
        catch (VideoSourceError & e)
        {
            throw NetworkSourceUnavailable(e.what());
        }
        break;
#else
        throw VideoSourceError(
            "BGRA colour space on network sources supported only with OpenCV"
        );
#endif
    }

    return source;
}


IVideoSource * VideoSourceFactory::create_file_reader(
        const std::string filepath,
        enum ColourSpace colour_space)
{
#ifdef USE_FFMPEG
    return new VideoSourceFFmpeg(filepath, colour_space);
#else
    throw VideoSourceError("Reading video files supported only"
                           " with FFmpeg");
#endif
}

}
