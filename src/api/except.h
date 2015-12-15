#pragma once

#include <stdexcept>
#include <string>

namespace gg {

//!
//! \brief More specific exceptions should extend
//! this class, which provides all basic exception
//! functionality
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
//! \brief Thrown if connection attempts by
//! gg::Factory to requested device fail
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

}
