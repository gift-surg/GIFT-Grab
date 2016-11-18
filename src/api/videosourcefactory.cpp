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
}

void VideoSourceFactory::free_device(Device device)
{
    if (_devices[(int) device] != nullptr)
        delete _devices[(int) device];
    _devices[(int) device] = nullptr;
}

}
