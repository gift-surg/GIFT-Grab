#include "factory.h"
#include "opencv_video_source.h"

namespace gg {

IVideoSource * Factory::_sources[2] = { NULL, NULL };

IVideoSource * Factory::connect(enum Device type) {
    int device_id = -1; // default value makes no sense

    switch (type) {
    case DVI2PCIeDuo_DVI:
        device_id = 0; // always /dev/video0
        break;
    case DVI2PCIeDuo_SDI:
        device_id = 1; // always /dev/video1
        break;
    default:
        std::string msg;
        msg.append("Device ")
           .append(std::to_string(type))
           .append(" not recognised");
        throw DeviceNotFound(msg);
    }

    if (_sources[(int) type] == NULL)
    {
        IVideoSource * src = new VideoSourceOpenCV(device_id);

        // check querying frame dimensions
        int width = -1, height = -1;
        if (not src->get_frame_dimensions(width, height))
        {
            std::string error;
            error.append("Device ")
                 .append(std::to_string(device_id))
                 .append(" connected, but ")
                 .append(" does not return frame dimensions.");
            throw DeviceOffline(error);
        }

        // check meaningful frame dimensions
        if (width <= 0 or height <= 0)
        {
            std::string error;
            error.append("Device ")
                 .append(std::to_string(device_id))
                 .append(" connected, but ")
                 .append(" returns meaningless frame dimensions.");
            throw DeviceOffline(error);
        }

        // check querying frames
        VideoFrame_BGRA frame;
        if (not src->get_frame(frame))
        {
            std::string error;
            error.append("Device ")
                 .append(std::to_string(device_id))
                 .append(" connected, but ")
                 .append(" does not return frames.");
            throw DeviceOffline(error);
        }

        // if no exception raised up to here, glory be to GiftGrab
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

}
