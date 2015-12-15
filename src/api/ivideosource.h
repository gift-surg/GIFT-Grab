#ifndef __IVIDEOSOURCE_H__
#define __IVIDEOSOURCE_H__

#include "videoframe.h"

//!
//! \brief This abstract class defines the interface that every video source
//! must implement.
//!
//! This enables the underlying algorithms to be agnostic of
//! the data sources.
//!
class IVideoSource
{
public:
    //!
    //! \brief Destructor, doing nothing
    //!
    virtual ~IVideoSource()
    {}

    //!
    //! \brief Get frame dimensions
    //! \param width
    //! \param height
    //! \return whether querying frame dimensions has
    //! succeeded
    //!
    virtual bool get_frame_dimensions(int & width, int & height) = 0;

    //!
    //! \brief Grab next available \c frame
    //! \param frame
    //! \return
    //!
    virtual bool get_frame(VideoFrame_BGRA & frame) = 0;

    //!
    //! \brief Get frame rate of used source
    //! \return
    //!
    virtual double get_frame_rate() = 0;

    //!
    //! \brief Set a ROI within each grabbed frame
    //! \param x upper left corner
    //! \param y upper left corner
    //! \param width from \c x onwards
    //! \param height from \c y onwards
    //! \sa get_full_frame
    //!
    virtual void set_sub_frame(int x, int y, int width, int height) = 0;

    //!
    //! \brief Reset sub-framing, i.e. return full frames instead
    //! of only ROI
    //! \sa set_sub_frame
    //!
    virtual void get_full_frame() { _get_sub_frame = false; }

    //!
    //! \brief Set number of frames to discard after starting
    //! streaming (i.e. one-off, until streaming becomes stable).
    //! This is a device-specific setting.
    //! \param burn_num
    //!
    void set_number_of_frames_to_burn(unsigned int burn_num)
    {
        _num_burn_frames = burn_num;
    }

    //!
    //! \brief
    //! \return
    //! \sa set_number_of_frames_to_burn
    //!
    unsigned int get_number_of_frames_to_burn()
    {
        return _num_burn_frames;
    }

protected:
    //!
    //! \brief Default constructor that resets sub-framing
    //! and also \c _num_burn_frames
    //!
    IVideoSource()
        :_get_sub_frame(false), _num_burn_frames(0)
    {}

    //!
    //! \brief If \c false, then grab full frames, otherwise
    //! a specific ROI within grabbed frames
    //! \sa get_full_frame
    //! \sa set_sub_frame
    //!
    bool            _get_sub_frame;

    //!
    //! \brief Number of frames to discard after starting
    //! streaming
    //!
    unsigned int    _num_burn_frames;
};

#endif
