// This class is based on the work by Luis Carlos Garcia-Peraza Herrera

#ifndef __BLACKMAGIC_VIDEOSOURCE_H__
#define __BLACKMAGIC_VIDEOSOURCE_H__

#include "ivideosource.h"
#include "macros.h"

#include "DeckLinkCaptureDelegate.h"

// OpenCV includes
#include "opencv2/opencv.hpp"

class FrameGrabberBlackMagic: public IVideoSource
{
public:
    FrameGrabberBlackMagic();
    FrameGrabberBlackMagic(BMDDisplayMode display_mode, BMDPixelFormat pixel_format);
    ~FrameGrabberBlackMagic();
    bool get_frame_dimensions(int &width, int &height);
    bool get_frame(VideoFrame_BGRA &frame);
    double get_frame_rate();
    void set_sub_frame(int x, int y, int width, int height)
    {
        _sub_frame = cv::Rect(x, y, width, height);
        _get_sub_frame = true;
        _buffer_sub_bgra = cv::Mat::zeros(height, width, CV_8UC4);
    }

private:
    void start_capture();
    void stop_capture();

    IDeckLink *                                 _deck_link;
    IDeckLinkInput *                            _deck_link_input;
    DeckLinkCaptureDelegate *                   _delegate;

    cv::Mat                                     _buffer_sub_bgra;
    cv::Rect                                    _sub_frame;
    bool                                        _initialized;

    DISALLOW_COPY_AND_ASSIGNMENT(FrameGrabberBlackMagic);

};

#endif
