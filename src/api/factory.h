#pragma once

#include "ivideosource.h"
#include "ivideotarget.h"
#include "except.h"

namespace gg {

//!
//! \brief Lists supported framegrabber hardware
//!
enum Device {
    DVI2PCIeDuo_DVI,
    DVI2PCIeDuo_SDI
};

//!
//! \brief Lists supported video saving options
//!
enum Storage
{
    File_XviD,
    File_H265
};

//!
//! \brief Used for connecting to supported
//! framegrabber hardware
//!
class Factory {
protected:
    //!
    //! \brief So that can keep track of everything
    //! opened and in use
    //!
    static IVideoSource * _sources[2];

public:
    //!
    //! \brief Try to connect to specified device
    //! \param type
    //! \return pointer to the device's framegrabber.
    //! Client should not delete this pointer on its
    //! own, but should instead call \c disconnect
    //! with the same \c type when finished
    //! \throw DeviceNotFound if connection attempt
    //! fails
    //! \throw DeviceOffline if connection attempt
    //! OK, but device offline (i.e. getting no frames)
    //! \sa disconnect
    //!
    static IVideoSource * connect(enum Device type);

    //!
    //! \brief Disconnect from specified \c type device,
    //! if already connected
    //! \param type
    //! \throw DeviceNotFound if \c type not recognised
    //! \sa connect - IVideoSource object \c connect
    //! returns will be destroyed by this, and hence the
    //! pointer unusable
    //!
    static void disconnect(enum Device type);

    //!
    //! \brief Create specified video saving target
    //! \param type
    //! \return pointer to created video saving target
    //! \throw VideoTargetError with a detailed error
    //! message if creation of video target with
    //! specified \c type fails for some reason
    //!
    static IVideoTarget * writer(enum Storage type);
};

}
