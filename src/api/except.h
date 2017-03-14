#pragma once

#include <stdexcept>
#include <string>

namespace gg {

//!
//! \brief More specific exceptions should extend
//! this class, which provides all basic exception
//! functionality
//!
//! \attention <b>In Python:</b> This exception
//! maps to \c RuntimeError
//!
class BasicException : public std::exception {
protected:
    //!
    //! \brief Exception details
    //!
    std::string _detail;

public:
    //!
    //! \brief Constructor with \c detail
    //! \param detail
    //!
    BasicException(const std::string & detail);

    //!
    //! \brief Destructor
    //!
    virtual ~BasicException() noexcept;

    //!
    //! \brief Get exception detail
    //! \return
    //!
    virtual const char * what() const noexcept override;
};

//!
//! \brief Thrown in case a connection is
//! attempted to a device that is already
//! connected
//!
//! \attention <b>In Python:</b> This exception
//! maps to \c IOError
//!
class DeviceAlreadyConnected : public BasicException
{
public:
    //!
    //! \brief
    //! \param detail
    //! \sa BasicException::BasicException
    //!
    DeviceAlreadyConnected(const std::string & detail);

    //!
    //! \brief
    //! \sa BasicException::~BasicException
    //!
    virtual ~DeviceAlreadyConnected() noexcept;
};

//!
//! \brief Thrown if connection attempts by
//! gg::VideoSourceFactory to requested device fail
//!
//! \attention <b>In Python:</b> This exception
//! maps to \c IOError
//!
class DeviceNotFound : public BasicException {
public:
    //!
    //! \brief
    //! \param detail
    //! \sa BasicException::BasicException
    //!
    DeviceNotFound(const std::string & detail);

    //!
    //! \brief
    //! \sa BasicException::~BasicException
    //!
    virtual ~DeviceNotFound() noexcept;
};

//!
//! \brief Thrown if a connected device is offline,
//! i.e. not returning any frames
//!
//! \attention <b>In Python:</b> This exception
//! maps to \c IOError
//!
class DeviceOffline : public BasicException {
public:
    //!
    //! \brief
    //! \param detail
    //! \sa BasicException::BasicException
    //!
    DeviceOffline(const std::string & detail);

    //!
    //! \brief
    //! \sa BasicException::~BasicException
    //!
    virtual ~DeviceOffline() noexcept;
};

//!
//! \brief Thrown in case of problems outputting
//! video, e.g. to files
//!
//! \attention <b>In Python:</b> This exception
//! maps to \c RuntimeError
//!
class VideoTargetError : public BasicException
{
public:
    //!
    //! \brief
    //! \param detail
    //! \sa BasicException::BasicException
    //!
    VideoTargetError(const std::string & detail);

    //!
    //! \brief ~VideoTargetError
    //! \sa BasicException::BasicException
    //!
    virtual ~VideoTargetError() noexcept;
};

//!
//! \brief Thrown in case of errors related to
//! video sources
//!
//! \attention <b>In Python:</b> This exception
//! maps to \c RuntimeError
//!
class VideoSourceError : public BasicException
{
public:
    //!
    //! \brief
    //! \param detail
    //!
    VideoSourceError(const std::string & detail);

    //!
    //! \brief ~VideoTargetError
    //! \sa BasicException::BasicException
    //!
    virtual ~VideoSourceError() noexcept;
};

//!
//! \brief Thrown in case of errors related to
//! connecting video observers to video sources.
//!
//! \attention <b>In Python:</b> This exception
//! maps to \c RuntimeError
//!
class ObserverError : public BasicException
{
public:
    //!
    //! \brief
    //! \param detail
    //!
    ObserverError(const std::string & detail);

    //!
    //! \brief
    //! \sa BasicException::BasicException
    //!
    virtual ~ObserverError() noexcept;
};

//!
//! \brief Thrown in case of problems related
//! to network video sources.
//!
//! \attention <b>In Python:</b> This exception
//! maps to \c IOError
//!
class NetworkSourceUnavailable : public BasicException
{
public:
    //!
    //! \brief
    //! \param detail
    //!
    NetworkSourceUnavailable(const std::string & detail);

    //!
    //! \brief
    //! \sa BasicException::BasicException
    //!
    virtual ~NetworkSourceUnavailable() noexcept;
};

}
