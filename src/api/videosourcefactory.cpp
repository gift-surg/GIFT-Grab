#include "videosourcefactory.h"

namespace gg
{

VideoSourceFactory VideoSourceFactory::_factory_singleton;

VideoSourceFactory::VideoSourceFactory()
{
    // TODO
}

VideoSourceFactory::~VideoSourceFactory()
{
    // TODO
}

VideoSourceFactory & VideoSourceFactory::get_instance()
{
    return _factory_singleton;
}

IVideoSource * VideoSourceFactory::get_device(Device device,
                                              ColourSpace colour)
{
    // TODO
    return nullptr;
}

}
