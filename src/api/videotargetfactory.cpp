#include "videotargetfactory.h"

namespace gg
{

VideoTargetFactory::VideoTargetFactory()
{
    // TODO
}

VideoTargetFactory::~VideoTargetFactory()
{
    // TODO
}

VideoTargetFactory & VideoTargetFactory::get_instance()
{
    // TODO
}

IVideoTarget * VideoTargetFactory::create_file_writer(enum Storage codec,
                                                      const std::string filename)
{
    // TODO
}

}
