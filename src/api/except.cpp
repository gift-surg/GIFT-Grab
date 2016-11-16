#include "except.h"

namespace gg {

BasicException::BasicException(const std::string & detail) :
    _detail(detail)
{

}

BasicException::~BasicException() noexcept
{
    _detail.clear();
}

const char * BasicException::what() const noexcept
{
    return _detail.data();
}

DeviceNotFound::DeviceNotFound(const std::string & detail) :
    BasicException(detail)
{

}

DeviceNotFound::~DeviceNotFound() noexcept
{

}

DeviceOffline::DeviceOffline(const std::string & detail) :
    BasicException(detail)
{

}

DeviceOffline::~DeviceOffline() noexcept
{

}

VideoTargetError::VideoTargetError(const std::string & detail) :
    BasicException(detail)
{

}

VideoTargetError::~VideoTargetError() noexcept
{

}

VideoSourceError::VideoSourceError(const std::string & detail) :
    BasicException(detail)
{

}

VideoSourceError::~VideoSourceError() noexcept
{

}

ObserverError::ObserverError(const std::string & detail) :
    BasicException(detail)
{

}

ObserverError::~ObserverError() noexcept
{

}

}
