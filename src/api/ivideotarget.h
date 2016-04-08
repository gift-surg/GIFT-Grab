#pragma once

#include "videoframe.h"

//!
//! \brief This abstract class provides the API
//! to be implemented by classes that provide
//! functionality to save streamed video e.g.
//! to files
//!
class IVideoTarget
{
public:
    //!
    //! \brief Initialise a file writer
    //! \param filepath
    //! \param fps frames per second
    //! \throw VideoTargetError if writer cannot
    //! be initialised, e.g. for IO reasons
    //!
    virtual void init(const std::string filepath, const float fps) = 0;

    //!
    //! \brief Append \c frame to output
    //! \param frame
    //! \throw VideoTargetError with a detailed
    //! error message if \c frame cannot be
    //! appended to output for some reason
    //!
    virtual void append(const VideoFrame_BGRA & frame) = 0;

    //!
    //! \brief Finalise writer, e.g. close file
    //! \throw VideoTargetError if writer cannot
    //! be finalised, e.g. for IO reasons
    //!
    virtual void finalise() = 0;
};
