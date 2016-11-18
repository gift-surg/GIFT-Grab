#include "videosourcefactory.h"

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
    // TODO
    if (_devices[(int) device] == nullptr)
    {
        _devices[(int) device] = new DummyVideoSource;
        _device_colours[(int) device] = colour;
    }
    else if (_device_colours[(int) device] != colour)
        throw DeviceAlreadyConnected(
                "Device already connected using another colour space"
        );
    return _devices[(int) device];
}

void VideoSourceFactory::free_device(Device device)
{
    switch (device)
    {
    case DVI2PCIeDuo_DVI:
    case DVI2PCIeDuo_SDI:
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

}
