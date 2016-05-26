#ifndef __VIDEOFRAME_H__
#define __VIDEOFRAME_H__

// OpenCV includes
#include <opencv2/core/core.hpp>
#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <memory>
#include "maskframe.h"

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
    //! \param manage_data
    //!
    VideoFrame(bool manage_data=false);

    //!
    //! \brief OpenCV based constructor
    //! \param mat
    //! \param manage_data
    //!
    VideoFrame(const cv::Mat & mat, bool manage_data=false);

    //!
    //! \brief Constructor with full data and parameter specs
    //! \param data if \c manage_data is set, all data in this
    //! will be copied; otherwise this \c data will be used (i.e.
    //! pointer will be copied)
    //! \param rows
    //! \param cols
    //! \param manage_data
    //!
    VideoFrame(unsigned char * data, size_t rows, size_t cols, bool manage_data=false);

    //!
    //! \brief Allocates memory for specified dimensions, and
    //! sets all pixels to black
    //! \param rows
    //! \param cols
    //!
    VideoFrame(const size_t rows, const size_t cols);

    //!
    //! \brief Initialise from passed \c data, based on the data
    //! management setting
    //! \param data
    //! \param rows
    //! \param cols
    //! \sa manages_own_data
    //!
    void init_from_pointer(unsigned char * data, size_t rows, size_t cols);

    //!
    //! \brief Initialise from passed OpenCV \c mat, based on the
    //! data management setting
    //! \param mat
    //! \sa manages_own_data
    //!
    void init_from_opencv_mat(const cv::Mat & mat);

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

    //!
    //! \brief Destructor that will free allocated data in case data
    //! management is set
    //! \sa manages_own_data
    //!
    virtual ~VideoFrame();

    //!
    //! \brief Copy data from \c rhs, also setting data management
    //! \param rhs
    //! \sa manages_own_data
    //!
    VideoFrame(const VideoFrame & rhs);

    //!
    //! \brief operator =
    //! \param rhs
    //! \sa VideoFrame(const VideoFrame & rhs)
    //!
    void operator=(const VideoFrame & rhs);

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
    //! \brief Get number of channels, i.e. 4 in BGRA
    //! \return
    //!
    size_t channels() const
    {
        return 4;
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

private:
    //!
    //! \brief Frame data
    //!
    unsigned char*  _data;

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

    //!
    //! \brief
    //! \param rhs
    //! \sa VideoFrame(const VideoFrame & rhs)
    //!
    void clone(const VideoFrame &rhs);

    //!
    //! \brief Free all managed data
    //! \sa ~VideoFrame
    //!
    void clear();
};

//!
//! \brief A class to represent a video frame with BGRA pixels
//!
//! We set the format to BGRA, simply because this is native to
//! most GPUs in the wild and should be faster to load than traditional
//! RGB (due to 4 byte alignment) and RGBA due to less reordering
//! on the GPU but this is all probably hardware dependent.
class VideoFrame_BGRA : public VideoFrame
{
public:
    //!
    //! \brief Constructor that by default will use externally
    //! managed frame data
    //! \param manage_data
    //!
    VideoFrame_BGRA(bool manage_data=false);

    //!
    //! \brief Allocates memory for specified dimensions, and
    //! sets all pixels to black
    //! \param rows
    //! \param cols
    //!
    VideoFrame_BGRA(const size_t rows, const size_t cols);
};

#endif
