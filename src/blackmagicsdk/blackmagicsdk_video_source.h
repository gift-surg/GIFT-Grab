#pragma once

#include "ivideosource.h"
#include "macros.h"
#include <DeckLinkAPI.h>

namespace gg
{

//!
//! \brief This class is used for streaming video from BlackMagic
//! framegrabbers
//!
//! \author Based on the work by Luis Carlos Garcia-Peraza-Herrera
//!
class VideoSourceBlackmagicSDK
    : public IVideoSource
    , public IDeckLinkInputCallback
{
protected:
    //!
    //! \brief Buffer for a video frame
    //!
    uint8_t * _video_buffer;

    //!
    //! \brief Total amount of memory allocated for a video frame
    //!
    size_t _video_buffer_length;

    //!
    //! \brief This video frame object will be used for notifying
    //! the observers
    //!
    VideoFrame _buffer_video_frame;

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

    // IDeckLinkInputCallback overrides:

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, LPVOID * ppv);

    ULONG STDMETHODCALLTYPE AddRef(void);

    ULONG STDMETHODCALLTYPE Release(void);

    HRESULT STDMETHODCALLTYPE VideoInputFormatChanged(
        BMDVideoInputFormatChangedEvents events,
        IDeckLinkDisplayMode * display_mode,
        BMDDetectedVideoInputFormatFlags format_flags
    );

    HRESULT STDMETHODCALLTYPE VideoInputFrameArrived(
        IDeckLinkVideoInputFrame * video_frame,
        IDeckLinkAudioInputPacket * audio_packet
    );

private:
    DISALLOW_COPY_AND_ASSIGNMENT(VideoSourceBlackmagicSDK);
};

}
