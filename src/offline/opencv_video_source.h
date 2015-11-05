#ifndef __OPENCV_DATA_SOURCE_H__
#define __OPENCV_DATA_SOURCE_H__

#include "ivideosource.h"
#include "macros.h"

// OpenCV includes
#include "opencv2/opencv.hpp"

/**
 * OpenCV video source
 */

class VideoSourceOpenCV: public IVideoSource
{
public:
    VideoSourceOpenCV(const char *path);
    bool get_frame_dimensions(int & width, int & height);
    bool get_frame(VideoFrame_BGRA &frame);
    double get_frame_rate();
    void set_sub_frame(int x, int y, int width, int height)
    {
        _sub_frame = cv::Rect(x, y, width, height);
        _get_sub_frame = true;
        _buffer_sub_bgra = cv::Mat::zeros(height, width, CV_8UC4);
    }

private:
    cv::VideoCapture        _cap;
    cv::Mat                 _buffer;
    cv::Mat                 _buffer_bgra;
    cv::Mat                 _buffer_sub_bgra;
    cv::Rect                _sub_frame;

    DISALLOW_COPY_AND_ASSIGNMENT(VideoSourceOpenCV);
};

#endif
