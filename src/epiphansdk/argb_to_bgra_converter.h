#pragma once

#include <cstdlib>

namespace gg
{

//!
//! \brief Epiphan SDK seems to support ARGB but not BGRA.
//! This class abstracts the conversion of captured ARGB
//! data to BGRA.
//! \sa VideoSourceEpiphanSDK
//!
class ArgbToBgraConverter
{
protected:
    //!
    //! \brief Width of frames passed for conversion
    //!
    size_t _width;

    //!
    //! \brief Height of frames passed for conversion
    //!
    size_t _height;

public:
    //!
    //! \brief Create a converter, which should be
    //! initialised by informing it of the frame
    //! dimensions
    //! \sa set_frame_dimensions
    //!
    ArgbToBgraConverter();

    //!
    //! \brief Free all allocated resources
    //!
    ~ArgbToBgraConverter();

public:
    //!
    //! \brief Convert ARGB data in passed buffer to BGRA
    //! data, saving it into the passed output buffer.
    //! \param argb should be in line with the frame
    //! dimensions set before
    //! \param bgra should be at least as large as the
    //! input buffer
    //! \sa set_frame_dimensions
    //!
    void convert(const unsigned char *argb, unsigned char *bgra);

    //!
    //! \brief Allocate all resources needed for ARGB to
    //! BGRA conversion based on passed frame dimensions
    //! \param width
    //! \param height
    //!
    void set_frame_dimensions(size_t width, size_t height);
};

}
