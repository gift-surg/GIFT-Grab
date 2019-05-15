#include "epiphansdk_video_source.h"
#include <iostream>

namespace gg
{

VideoSourceEpiphanSDK::VideoSourceEpiphanSDK(
    const std::string device_id, V2U_INT32 colour_space)
    : IVideoSource()
    , _frame_grabber(nullptr)
    , _flags(0)
    , _daemon(nullptr)
    , _bgra_data(nullptr)
    , _buffer(nullptr)
{
    FrmGrab_Init();

    _frame_grabber = FrmGrabLocal_OpenSN(device_id.c_str());
    if (!_frame_grabber)
    {
        // TODO - exception GiftGrab#42
        std::cerr << "Could not open " << device_id << std::endl;
        return;
    }
    FrmGrab_SetMaxFps(_frame_grabber, 160);

    if (colour_space != V2U_GRABFRAME_FORMAT_I420 && colour_space != V2U_GRABFRAME_FORMAT_RGB24)
    {
        // TODO - exception GiftGrab#42
        std::cerr << "Colour space " << colour_space << " not supported" << std::endl;
        return;
    }
    else if (colour_space == V2U_GRABFRAME_FORMAT_I420)
        _colour = I420;
    else
    {
        _colour = BGRA;
#ifdef __linux__
        // for some reason on Linux systems, the image is flipped without this
        _flags |= V2U_GRABFRAME_BOTTOM_UP_FLAG;
#endif
    }
    _flags |= colour_space;

    VideoFrame frame(_colour);
    _full.x = 0;
    _full.y = 0;
    /* TODO - e.g. EpiphanSDK_MAX_RES_X and
     * EpiphanSDK_MAX_RES_Y after
     * EpiphanSDK#6
     */
    _full.width = 1920;
    _full.height = 1080;
    if (_colour == BGRA)
    {
        _bgra_data = reinterpret_cast<unsigned char*>(malloc(
            VideoFrame::required_data_length(_colour, _full.width, _full.height)
        ));
    }
    get_full_frame();
    // TODO - exception GiftGrab#42
    if (!get_frame(frame)) return;

    _daemon = new gg::BroadcastDaemon(this);
    _daemon->start(get_frame_rate());
}

VideoSourceEpiphanSDK::~VideoSourceEpiphanSDK()
{
    delete _daemon;
    if (_frame_grabber) FrmGrab_Close(_frame_grabber);
    FrmGrab_Deinit();
    if (_colour == BGRA && _bgra_data != nullptr)
    {
        free(_bgra_data);
        _bgra_data = nullptr;
    }
}

bool VideoSourceEpiphanSDK::get_frame_dimensions(int & width, int & height)
{
    width = _roi.width;
    height = _roi.height;
    return true;
}

bool VideoSourceEpiphanSDK::get_frame(VideoFrame & frame)
{
    if (frame.colour() != _colour)
        // TODO - exception GiftGrab#42
        return false;

    std::lock_guard<std::mutex> buffer_lock_guard(_buffer_lock);

    _buffer = FrmGrab_Frame(_frame_grabber, _flags, &_roi);
    if (_buffer)
    {
        unsigned char *data = static_cast<unsigned char*>(_buffer->pixbuf);
        size_t frame_data_length = _buffer->imagelen;
        /* TODO #54 specified _roi not always
         * respected by FrmGrab_Frame, hence
         * constructing with _buffer->crop
         * instead of _roi to avoid alignment
         * problems when saving to video files
         */
        size_t frame_width = _buffer->crop.width,
               frame_height = _buffer->crop.height;
        unsigned char *frame_data = nullptr;
        switch(_colour)
        {
        case I420:
            frame_data = data;
            break;
        case BGRA:
            _rgb_to_bgra.set_frame_dimensions(frame_width, frame_height);
            _rgb_to_bgra.convert(data, _bgra_data);
            frame_data_length = VideoFrame::required_data_length(_colour, frame_width, frame_height);
            frame_data = _bgra_data;
            break;
        default:
            // TODO
            break;
        }
        frame.init_from_specs(
            frame_data, frame_data_length,
            frame_width, frame_height
        );
        FrmGrab_Release(_frame_grabber, _buffer);
        _buffer = nullptr;
        return true;
    }
    else
        return false;
}

double VideoSourceEpiphanSDK::get_frame_rate()
{
    double frame_rate = -1;
    if (_frame_grabber)
    {
#if defined(Epiphan_DVI2PCIeDuo_DVI) && \
    defined(Epiphan_DVI2PCIeDuo_SDI) && \
    defined(Epiphan_DVI2PCIeDuo_DVI_MAX_FRAME_RATE) && \
    defined(Epiphan_DVI2PCIeDuo_SDI_MAX_FRAME_RATE)
        std::string port_id = FrmGrab_GetId(_frame_grabber);
        if (port_id == Epiphan_DVI2PCIeDuo_DVI)
            frame_rate = Epiphan_DVI2PCIeDuo_DVI_MAX_FRAME_RATE;
        else if (port_id == Epiphan_DVI2PCIeDuo_SDI)
            frame_rate = Epiphan_DVI2PCIeDuo_SDI_MAX_FRAME_RATE;
        if (frame_rate > 0)
        {
            /* The above max frame rates are defined for I420,
             * and are halved for BGRA, due to hardware bandwidth
             */
            if (_colour == BGRA)
                frame_rate /= 2.0;
            return frame_rate;
        }
#endif
    }

    // TODO - exception GiftGrab#42
    return 0;
}

void VideoSourceEpiphanSDK::set_sub_frame(int x, int y, int width, int height)
{
    if (x >= _full.x && x + width <= _full.x + _full.width &&
        y >= _full.y && y + height <= _full.y + _full.height)
    {
        _roi.x = x;
        _roi.y = y;
        _roi.width = width;
        _roi.height = height;
    }
    // TODO - exception GiftGrab#42
//    else
//        throw VideoSourceError("ROI " + std::to_string(x) + ", " +
//                               std::to_string(y) + ", " +
//                               std::to_string(width) + ", " +
//                               std::to_string(height) + ", " +
//                               "not within full frame");
}

void VideoSourceEpiphanSDK::get_full_frame()
{
    _roi.x = _full.x;
    _roi.y = _full.y;
    _roi.width = _full.width;
    _roi.height = _full.height;
}

}
