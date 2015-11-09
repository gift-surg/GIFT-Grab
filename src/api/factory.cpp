#include "factory.h"
#include "opencv_video_source.h"

namespace gg {

IVideoSource * Factory::connect(enum Device type) {
    IVideoSource * _current = NULL;
    switch (type) {
    case DVI2PCIeDuo:
        _current = new VideoSourceOpenCV(0);
        return _current;
        break;
    default:
        std::string msg;
        msg.append("Device ")
           .append(std::to_string(type))
           .append(" not recognised");
        throw DeviceNotFound(msg);
    }
}

void Factory::disconnect(IVideoSource * device) {
    if (device)
        delete device;
}

}
