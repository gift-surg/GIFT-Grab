#pragma once

#include <stdexcept>
#include <string>

namespace gg {

//!
//! \brief Thrown if connection attempts by
//! gg::Factory to requested device fail
//!
class DeviceNotFound : public std::exception {
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
    DeviceNotFound(const std::string & detail);

    //!
    //! \brief Destructor
    //!
    virtual ~DeviceNotFound() noexcept;

    //!
    //! \brief Get exception detail
    //! \return
    //!
    virtual const char * what() const noexcept override;
};

}
