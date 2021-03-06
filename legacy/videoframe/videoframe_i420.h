#pragma once
#include <stdlib.h>
#include "videoframe.h"

namespace gg
{

class VideoFrame_I420 : public VideoFrame
{
public:
    //!
    //! \brief Constructor that uses externally
    //! managed data by default
    //! \param manage_data
    //!
    VideoFrame_I420(bool manage_data = false);

    //!
    //! \brief Allocates memory for specified dimensions, and
    //! sets all pixels to black
    //! \param cols
    //! \param rows
    //!
    VideoFrame_I420(const size_t cols, const size_t rows);

    //!
    //! \brief Initialise a frame from passed \c data.
    //! No checks are performed to ensure integrity of
    //! \c length in conjunction with \c cols and \c
    //! rows
    //! \param data
    //! \param length \c data length
    //! \param cols frame width
    //! \param rows frame height
    //! \param manage_data if \c true, \c memcpy \c data,
    //! otherwise copy \c data pointer
    //!
    VideoFrame_I420(unsigned char * data, const size_t length,
                    const size_t cols, const size_t rows,
                    bool manage_data);

public:
    //!
    //! \brief
    //! \param rhs
    //!
    void operator=(const VideoFrame_I420 & rhs);

protected:
    //!
    //! \brief Convenience method to initialise object from
    //! passed \c data
    //! \param data
    //! \param length
    //! \param cols
    //! \param rows
    //! \sa _manage_data
    //!
    void init_from_pointer(unsigned char * data, size_t length,
                           size_t cols, size_t rows);
};

}
