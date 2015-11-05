
#ifndef __VIDEOFRAME_H__
#define __VIDEOFRAME_H__

#ifdef USE_OPENCV
    #include "opencv2/core/core.hpp"
    #include "opencv/cv.h"
    #include "opencv/highgui.h"
#endif

#include <memory>
#include "maskframe.h"


/**
 * A class to represent a video frame. For efficiency issues,
 * data can be chosen to not be copied. In that case, the user is
 * responsible for ensuring the data pointer is valid duing the
 * lifetime of this object.
 *
 * We set the format to BGRA, simply because this is native to most GPUs in the wild and
 * should be faster to load than traditional RGB (due to 4 byte alignment) and RGBA due to less
 * reordering on the GPU but this is all probably hardware dependent.
 */
class VideoFrame_BGRA
{
public:
    VideoFrame_BGRA(bool manage_data=false);
#ifdef USE_OPENCV
    /**
     ** OpenCV based constructor
     */
    VideoFrame_BGRA(const cv::Mat & mat, bool manage_data=false);
    VideoFrame_BGRA(unsigned char * data, size_t rows, size_t cols, bool manage_data=false);
    void init_from_pointer(unsigned char * data, size_t rows, size_t cols);
    void init_from_opencv_mat(const cv::Mat & mat);

    // Compute an elliptical mask for this image.
    std::unique_ptr<MaskFrame> compute_image_mask(int x, int y, int width,
                                                  int height);

#endif

    /**
     * Destructor
     */
    ~VideoFrame_BGRA();

    // Copy semantics
    VideoFrame_BGRA(const VideoFrame_BGRA & rhs);
    void operator=(const VideoFrame_BGRA & rhs);


    size_t rows() const
    {
        return _rows;
    }

    size_t cols() const
    {
        return _cols;
    }

    size_t channels() const
    {
        return 4;
    }

    bool manages_own_data() const
    {
        return _manage_data;
    }

    const unsigned char * data() const
    {
        return _data;
    }

private:
    unsigned char*  _data;
    size_t          _rows;
    size_t          _cols;
    bool            _manage_data;

    void clone(const VideoFrame_BGRA &rhs);
    void clear();
};

#endif
