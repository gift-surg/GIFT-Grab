#pragma once

#include "ivideosource.h"
#include "macros.h"

namespace gg
{

//!
//! \brief This class is used for streaming video from BlackMagic
//! framegrabbers
//!
//! \author Based on the work by Luis Carlos Garcia-Peraza-Herrera
//!
class VideoSourceBlackmagicSDK : public IVideoSource
{
protected:
    //!
    //! \brief Default constructor that initialises nothing
    //!
    VideoSourceBlackmagicSDK();

public:
    //!
    //! \brief Initialise communication with a Blackmagic
    //! framegrabber for capturing frames in the passed \c colour
    //! space
    //! \param colour
    //! \throw VideoSourceError with a detailed error message if
    //! connection attempt fails
    //!
    VideoSourceBlackmagicSDK(ColourSpace colour);

    //!
    //! \brief Finalise communication with used framegrabber and
    //! release it
    //!
    virtual ~VideoSourceBlackmagicSDK();

    bool get_frame_dimensions(int & width, int & height) override;

    bool get_frame(VideoFrame & frame) override;

    double get_frame_rate() override;

    void set_sub_frame(int x, int y, int width, int height) override;

    void get_full_frame() override;

private:
    DISALLOW_COPY_AND_ASSIGNMENT(VideoSourceBlackmagicSDK);
};

}
