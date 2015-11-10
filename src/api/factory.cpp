#include "factory.h"
#include "opencv_video_source.h"

namespace gg {

IVideoSource * Factory::_sources[1] = { NULL };

IVideoSource * Factory::connect(enum Device type) {
    switch (type) {
    case DVI2PCIeDuo:
        if (_sources[Device::DVI2PCIeDuo] == NULL)
            _sources[Device::DVI2PCIeDuo] = new VideoSourceOpenCV(0);
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
