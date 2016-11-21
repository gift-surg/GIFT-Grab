#include "videotargetfactory.h"

namespace gg
{

VideoTargetFactory VideoTargetFactory::_factory_singleton;

VideoTargetFactory::VideoTargetFactory()
{
    // nop
}

VideoTargetFactory::~VideoTargetFactory()
{
    // nop
}

VideoTargetFactory & VideoTargetFactory::get_instance()
{
    return _factory_singleton;
}

IVideoTarget * VideoTargetFactory::create_file_writer(enum Storage codec,
                                                      const std::string filename)
{
    // TODO
}

}
