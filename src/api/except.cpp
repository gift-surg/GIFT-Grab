#include "except.h"

namespace gg {

DeviceNotFound::DeviceNotFound(const std::string & detail) :
    _detail(detail)
{

}

DeviceNotFound::~DeviceNotFound() noexcept
{
    _detail.clear();
}

const char * DeviceNotFound::what() const noexcept
{
    return _detail.data();
}

}
