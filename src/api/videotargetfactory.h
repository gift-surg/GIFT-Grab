#pragma once

#include "macros.h"
#include "codec.h"
#include "ivideotarget.h"

namespace gg
{

//!
//! \brief This class implements the factory
//! design pattern for creating video targets,
//! for instance for encoding video streams to
//! files.
//!
//! The creation functions follow the "Resource
//! Acquisition Is Initialisation" (RAII)
//! principle.
//!
//! \sa IVideoTarget
//!
class VideoTargetFactory
{
protected:
    //!
    //! \brief The factory singleton object
    //!
    static VideoTargetFactory _factory_singleton;

protected:
    //!
    //! \brief The constructor should never
    //! be publicly called
    //! \sa get_instance
    //!
    VideoTargetFactory();

public:
    //!
    //! \brief
    //!
    virtual ~VideoTargetFactory();

public:
    //!
    //! \brief Get reference to singleton
    //! instance of this class
    //! \return
    //!
    static VideoTargetFactory & get_instance();

    //!
    //! \brief Create specified video saving target
    //! \param codec
    //! \param filename RAII on this filename if target
    //! is created successfully
    //! \param frame_rate the resulting file writer
    //! will use this frame rate
    //! \return pointer to created video saving target.
    //! It is the caller's responsibility to properly
    //! garbage-collect this pointer, i.e. its owner
    //! is the caller.
    //! \throw VideoTargetError with a detailed error
    //! message if creation of video target with
    //! specified \c type fails for some reason, most
    //! notably wrong file extension or invalid \c
    //! frame_rate
    //!
    IVideoTarget * create_file_writer(enum Codec codec,
                                      const std::string filename,
                                      const float frame_rate);

protected:
    DISALLOW_COPY_AND_ASSIGNMENT(VideoTargetFactory);
};

}
