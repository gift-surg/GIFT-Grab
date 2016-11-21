#pragma once

#include "ivideotarget.h"
#include "except.h"

namespace gg {

//!
//! \brief Lists supported video saving options
//!
enum Storage
{
    File_XviD,
    File_HEVC,
    File_VP9
};

//!
//! \brief Used for connecting to supported
//! framegrabber hardware
//!
class Factory {
public:

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
