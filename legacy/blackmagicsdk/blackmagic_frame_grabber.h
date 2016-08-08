#ifndef __BLACKMAGIC_VIDEOSOURCE_H__
#define __BLACKMAGIC_VIDEOSOURCE_H__

#include "ivideosource.h"
#include "macros.h"
#include "DeckLinkCaptureDelegate.h"
// OpenCV includes
#include "opencv2/opencv.hpp"

//!
//! \brief This class is used for streaming video from BlackMagic
//! framegrabbers
//!
//! \author Based on the work by Luis Carlos Garcia-Peraza Herrera
//!
class FrameGrabberBlackMagic: public IVideoSource
{
public:
    //!
    //! \brief Default constructor that initialises nothing
    //!
    FrameGrabberBlackMagic();

    //!
    //! \brief Initialise communication with framegrabber after
    //! checking available hardware supports \c display_mode and
    //! \c pixel_format
    //! \param display_mode
    //! \param pixel_format
    //!
    FrameGrabberBlackMagic(BMDDisplayMode display_mode, BMDPixelFormat pixel_format);

    //!
    //! \brief Finalise communication with used framegrabber
    //!
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
    //!
    //! \brief Start actual streaming
    //! \throw std::runtime_error if framegrabber not initialised
    //! properly or if streaming cannot be started
    //!
    void start_capture();

    //!
    //! \brief Stop streaming if framegrabber present and in use
    //!
    void stop_capture();

    //!
    //! \brief Used for querying available hardware
    //!
    IDeckLink *                                 _deck_link;

    //!
    //! \brief Used for initialising found hardware and starting
    //! streaming
    //!
    IDeckLinkInput *                            _deck_link_input;

    //!
    //! \brief Gets frames
    //!
    DeckLinkCaptureDelegate *                   _delegate;

    //!
    //! \brief BGRA Buffer for frame ROI
    //!
    cv::Mat                                     _buffer_sub_bgra;

    //!
    //! \brief Descriptor for frame ROI
    //! \sa MaskFrame
    //!
    cv::Rect                                    _sub_frame;

    //!
    //! \brief Flag indicating whether initalisation and streaming
    //! are OK
    //!
    bool                                        _initialized;

    DISALLOW_COPY_AND_ASSIGNMENT(FrameGrabberBlackMagic);

};

#endif
