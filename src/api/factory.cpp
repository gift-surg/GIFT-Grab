#include "factory.h"

namespace gg {

IVideoSource * Factory::connect(enum Device type) {
    switch (type) {
    case DVI2PCIeDuo:
        // TODO
        throw DeviceNotFound("not implemented");
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
