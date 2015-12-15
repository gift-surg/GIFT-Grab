#include "factory.h"
#include "opencv_video_source.h"

namespace gg {

IVideoSource * Factory::_sources[1] = { NULL };

IVideoSource * Factory::connect(enum Device type) {
    switch (type) {
    case DVI2PCIeDuo:
        if (_sources[Device::DVI2PCIeDuo] == NULL)
        {
            // error string, in case devices found that
            // disappoint us by returning meaningless data
            std::string error_device_checks = "";
            // check devices start through end (e.g. /dev/video1)
            int start = 0, end = 4;
            // flag indicating something found
            bool found = false;
            // device that was found
            int deviceId = -1;

            // go through devices from start to end
            for (deviceId = start; deviceId <= end; deviceId++)
            {
                try
                {
                    IVideoSource * src = new VideoSourceOpenCV(deviceId);
                    int width = -1, height = -1;
                    if (src->get_frame_dimensions(width, height))
                    {
                        if (width > 0 and height > 0) {
                            found = true;
                            _sources[Device::DVI2PCIeDuo] = src;
                            break;
                        }
                        else {
                            if (not error_device_checks.empty())
                                error_device_checks.append(", ");
                            error_device_checks.append(std::to_string(deviceId));
                        }
                    }
                }
                catch (DeviceNotFound & dnf)
                {
                    continue;
                }
            }

            // no glory
            if (not found)
            {
                std::string error = "Tried to locate Epiphan DVI2PCIeDuo in devices ";
                error.append(std::to_string(start));
                error.append(" (e.g. /dev/video");
                error.append(std::to_string(start));
                error.append(" on Linux)");
                error.append(" to ");
                error.append(std::to_string(end));
                error.append(" with no success");
                if (not error_device_checks.empty())
                {
                    error.append(" (");
                    error.append(error_device_checks);
                    error.append(" could be connected to, ");
                    error.append("but replied with meaningless frame dimensions)");
                }
                throw DeviceNotFound(error);
            }

            // check if found device returns frames
            VideoFrame_BGRA frame;
            if (not _sources[Device::DVI2PCIeDuo]->get_frame(frame))
            {
                std::string error = "Device ";
                error.append(std::to_string(deviceId));
                error.append(" connected, but appears to be offline.");
                throw DeviceOffline(error);
            }
        }

        // if no exception raised up to here, glory be to GiftGrab
        return _sources[Device::DVI2PCIeDuo];

    default:
        std::string msg;
        msg.append("Device ")
           .append(std::to_string(type))
           .append(" not recognised");
        throw DeviceNotFound(msg);
    }
}

void Factory::disconnect(enum Device type) {
    switch (type) {
    case DVI2PCIeDuo:
        if (_sources[Device::DVI2PCIeDuo]) {
            delete _sources[Device::DVI2PCIeDuo];
            _sources[Device::DVI2PCIeDuo] = NULL;
        }
        break;
    default:
        std::string msg;
        msg.append("Device ")
           .append(std::to_string(type))
           .append(" not recognised");
        throw DeviceNotFound(msg);
    }
}

}
