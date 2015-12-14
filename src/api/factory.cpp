#include "factory.h"
#include "opencv_video_source.h"

namespace gg {

IVideoSource * Factory::_sources[1] = { NULL };

IVideoSource * Factory::connect(enum Device type) {
    switch (type) {
    case DVI2PCIeDuo:
        if (_sources[Device::DVI2PCIeDuo] == NULL)
        {
            int start = 0, end = 4;
            bool found = false;
            for (int deviceId = start; deviceId <= end; deviceId++)
            {
                try
                {
                    IVideoSource * src = new VideoSourceOpenCV(deviceId);
                    found = true;
                    _sources[Device::DVI2PCIeDuo] = src;
                }
                catch (DeviceNotFound & dnf)
                {
                    continue;
                }
            }
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
                throw DeviceNotFound(error);
            }
        }
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
