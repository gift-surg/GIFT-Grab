#pragma once

#ifdef USE_OPENCV
#include <opencv2/core/core.hpp>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#endif // USE_OPENCV

#include <memory>
#include "maskframe.h"

namespace gg
{

//!
//! \brief Supported colour spaces
//!
//! For YUV formats, see https://wiki.videolan.org/YUV
//!
enum ColourSpace
{
    BGRA,
    I420
};

//!
//! \brief A class to represent a video frame
//!
//! For efficiency issues, data can be chosen to not be copied.
//! In that case, the user is responsible for ensuring the data
//! pointer is valid duing the lifetime of this object.
//!
class VideoFrame
{
public:
    //!
    //! \brief Constructor that by default will use externally
    //! managed frame data
    //!
    //! \param colour
    //! \param manage_data
    //!
    VideoFrame(enum ColourSpace colour, bool manage_data=false);

    //!
    //! \brief Allocates memory for specified dimensions, and
    //! sets all pixels to black
    //!
    //! \param colour
    //! \param cols
    //! \param rows
    //!
    VideoFrame(enum ColourSpace colour, size_t cols, size_t rows);

#ifdef USE_OPENCV
    //!
    //! \brief Compute an elliptical mask for this image
    //! \param x
    //! \param y
    //! \param width
    //! \param height
    //! \return
    //!
    std::unique_ptr<MaskFrame> compute_image_mask(int x, int y, int width,
                                                  int height);
#endif // USE_OPENCV

    //!
    //! \brief Destructor that will free allocated data in case data
    //! management is set
    //! \sa manages_own_data
    //!
    virtual ~VideoFrame();

    //!
    //! \brief Initialise using passed data
    //! \param data
    //! \param data_length
    //! \param cols
    //! \param rows
    //! \sa manages_own_data
    //!
    void init_from_pointer(unsigned char * data, size_t data_length,
                           size_t cols, size_t rows);

    //!
    //! \brief Get length of data buffer
    //! \return
    //!
    const size_t data_length() const
    {
        return _data_length;
    }

    //!
    //! \brief Get number of rows (y-axis, i.e. height)
    //! \return
    //!
    size_t rows() const
    {
        return _rows;
    }

    //!
    //! \brief Get number of columns (x-axis, i.e. width)
    //! \return
    //!
    size_t cols() const
    {
        return _cols;
    }

    //!
    //! \brief Get whether data is managed by this object
    //! or externally
    //! \return
    //!
    bool manages_own_data() const
    {
        return _manage_data;
    }

    //!
    //! \brief Get a constant pointer to frame data
    //! \return
    //!
    const unsigned char * data() const
    {
        return _data;
    }

    //!
    //! \brief Get what colour space \c this frame uses
    //! \return
    //!
    const enum ColourSpace colour() const
    {
        return _colour;
    }

protected:
    //!
    //! \brief Colour space, i.e. description
    //! of contained data
    //!
    enum ColourSpace _colour;

    //!
    //! \brief Frame data
    //!
    unsigned char*  _data;

    //!
    //! \brief Frame data length
    //!
    size_t _data_length;

    //!
    //! \brief
    //! \sa rows()
    //!
    size_t          _rows;

    //!
    //! \brief
    //! \sa cols()
    //!
    size_t          _cols;

    //!
    //! \brief
    //! \sa manages_own_data()
    //!
    bool            _manage_data;

protected:
    //!
    //! \brief Allocate any necessary data
    //!
    void init();

    //!
    //! \brief Free all managed data
    //! \sa ~VideoFrame
    //!
    void clear();
};

}
