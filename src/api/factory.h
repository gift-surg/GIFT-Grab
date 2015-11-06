#pragma once

#include "ivideosource.h"
#include "except.h"

namespace gg {

//!
//! \brief Lists supported framegrabber hardware
//!
enum Device {
    DVI2PCIeDuo
};

//!
//! \brief Used for connecting to supported
//! framegrabber hardware
//!
class Factory {
public:
    //!
    //! \brief Try to connect to specified device
    //! \param type
    //! \return pointer to the device's framegrabber
    //! \throw DeviceNotFound if connection attempt
    //! fails
    //!
    static IVideoSource * connect(enum Device type);

    //!
    //! \brief Disconnect from specified \c device, if
    //! already connected
    //! \param device
    //!
    static void disconnect(IVideoSource * device);
};

}
