#pragma once

#ifdef USE_OPENCV
#include <opencv2/core/core.hpp>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/core/version.hpp>
#if CV_MAJOR_VERSION == 3
#include <opencv2/imgproc/imgproc.hpp>
namespace cv
{
    using std::vector;
}
#endif
#endif // USE_OPENCV

#include <memory>
#include "maskframe.h"
#include "except.h"

namespace gg
{

//!
//! \brief Supported colour spaces
//!
//! For YUV formats, see https://wiki.videolan.org/YUV
//!
enum ColourSpace
{
    BGRA
    , I420
    , UYVY
};

//!
//! \brief A class to represent a video frame
//!
//! For efficiency issues, data can be chosen to not be copied.
//! In that case, the caller is responsible for ensuring the data
//! pointer is valid during the lifetime of the frame object.
//!
class VideoFrame
{
public:
    //!
    //! \brief Constructor that by default will use externally
    //! managed frame data
    //!
    //! The frame can later be initialised by \c init_from_specs()
    //!
    //! \param colour
    //! \param manage_data
    //! \sa init_from_specs
    //!
    VideoFrame(enum ColourSpace colour, bool manage_data = false);

    //!
    //! \brief Allocates memory for specified dimensions, and
    //! sets all pixels to black
    //!
    //! \c cols and \c rows are checked in conjunction with \c
    //! colour requirements. I420 requires even dimensions, so
    //! in case an odd dimension is provided, it is made even
    //! by adding a pixel.
    //!
    //! \param colour
    //! \param cols
    //! \param rows
    //!
    VideoFrame(enum ColourSpace colour, size_t cols, size_t rows);

    //!
    //! \brief Create a video frame by copying the data and
    //! all properties of \c rhs
    //! \param rhs
    //!
    VideoFrame(const VideoFrame & rhs);

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
    //! \brief Clone \c rhs, copying its data
    //! \param rhs
    //!
    void operator=(const VideoFrame & rhs);

    //!
    //! \brief Initialise using passed data AND frame specs
    //!
    //! The caller is responsible for ensuring consistency of specs.
    //! As such, the only check performed is whether the columns and
    //! rows are in line with the frame's colour space requirements.
    //! Other than that, this function proceeds BLINDLY. So use with
    //! care.
    //!
    //! \param data
    //! \param data_length
    //! \param cols
    //! \param rows
    //! \sa manages_own_data
    //! \sa VideoFrame(enum ColourSpace, size_t, size_t)
    //!
    void init_from_specs(unsigned char * data, size_t data_length,
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
    unsigned char * _data;

    //!
    //! \brief Frame data length
    //!
    size_t _data_length;

    //!
    //! \brief Always use \c set_dimensions() to set
    //! this
    //! \sa rows()
    //! \sa set_dimensions()
    //!
    size_t _rows;

    //!
    //! \brief Always use \c set_dimensions() to set
    //! this
    //! \sa cols()
    //! \sa set_dimensions()
    //!
    size_t _cols;

    //!
    //! \brief
    //! \sa manages_own_data()
    //!
    bool _manage_data;

protected:
    //!
    //! \brief Clone \c rhs, after setting \c
    //! _manage_data
    //! \param rhs
    //! \sa _manage_data
    //!
    void clone(const VideoFrame & rhs);

    //!
    //! \brief Set dimensions in conjunction with
    //! colour space requirements
    //!
    //! \param cols
    //! \param rows
    //! \sa VideoFrame(enum ColourSpace, size_t, size_t)
    //!
    void set_dimensions(size_t cols, size_t rows);

    //!
    //! \brief Allocate / extend memory and set data
    //! length indicator, ONLY IF managing own data
    //! \param data_length passed to avoid allocating
    //! in case existing memory satisfies the length
    //! requirement
    //! \sa _data
    //! \sa _data_length
    //! \sa _manage_data
    //!
    void allocate_memory(size_t data_length);

    //!
    //! \brief Free any managed memory, and set all
    //! spec variables to 0 or \c nullptr
    //! \sa _manage_data
    //! \sa _data
    //! \sa _data_length
    //! \sa _cols
    //! \sa _rows
    //!
    void free_memory();

    //!
    //! \brief Compute how much memory is needed, based
    //! on column and row specs
    //! \sa _manage_data
    //! \sa _cols
    //! \sa _rows
    //! \throw BasicException if \c _colour not set
    //! properly
    //!
    size_t get_data_length() const;

    //!
    //! \brief Set all pixels of frame to black
    //!
    void set_pixels_black();
};

}
