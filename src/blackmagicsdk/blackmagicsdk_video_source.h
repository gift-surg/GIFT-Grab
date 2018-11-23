#pragma once

#include "ivideosource.h"
#include "macros.h"
#include "deck_link_bgra_video_frame.h"
#include <DeckLinkAPI.h>
#include <mutex>

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
    //! \brief
    //!
    double _frame_rate;

    //!
    //! \brief This mutex will be locked when writing to or
    //! reading from the data buffer
    //! \sa _video_buffer
    //!
    std::mutex _data_lock;

    //!
    //! \brief Buffer for a video frame
    //!
    uint8_t * _video_buffer;

    //!
    //! \brief Total amount of memory allocated for a video frame
    //!
    size_t _video_buffer_length;

    //!
    //! \brief Video frame width
    //!
    size_t _cols;

    //!
    //! \brief Video frame height
    //!
    size_t _rows;

    //!
    //! \brief This video frame object will be used for notifying
    //! the observers
    //!
    VideoFrame _buffer_video_frame;

    //!
    //! \brief Manages the connection to the DeckLink hardware
    //!
    IDeckLink * _deck_link;

    //!
    //! \brief Manages the input from the DeckLink hardware
    //!
    IDeckLinkInput * _deck_link_input;

    //!
    //! \brief Detected video input flags
    //!
    BMDVideoInputFlags _video_input_flags;

    //!
    //! \brief Converter needed in case of BGRA captures
    //!
    IDeckLinkVideoConversion *_12_bit_rgb_to_bgra_converter;

    //!
    //! \brief Internal frame buffers for post-capture
    //! conversion
    //!
    DeckLinkBGRAVideoFrame *_bgra_frame_buffers[2];

    //!
    //! \brief Flag indicating streaming status
    //!
    bool _running;

    //!
    //! \brief Reference count
    //! \sa AddRef()
    //! \sa Release()
    //!
    int32_t _n_ref;

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
    //! \param deck_link_index The identifier index of the
    //! DeckLink card (port) to connect to
    //! \param colour
    //! \throw VideoSourceError with a detailed error message if
    //! connection attempt fails
    //!
    VideoSourceBlackmagicSDK(size_t deck_link_index,
                             ColourSpace colour);

    //!
    //! \brief Finalise communication with used framegrabber and
    //! release it
    //!
    virtual ~VideoSourceBlackmagicSDK();

    bool get_frame_dimensions(int & width, int & height) override;

    bool get_frame(VideoFrame & frame) override;

    double get_frame_rate() override;

    //!
    //! \brief Not implemented: throws VideoSourceError
    //! \param x
    //! \param y
    //! \param width
    //! \param height
    //! \throw VideoSourceError
    //!
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

protected:
    //!
    //! \brief Release all data structures and members
    //! related to DeckLink devices
    //!
    void release_deck_link() noexcept;

    //!
    //! \brief (Re-)allocate internal buffers ONLY IF the new
    //! dimensions are different than the previous ones
    //! \param cols new frame width
    //! \param rows new frame height
    //! \param frame_flags
    //!
    inline void smart_allocate_buffers(
        size_t cols, size_t rows, BMDFrameFlags frame_flags
    ) noexcept;

    //!
    //! \brief Try to detect the input video format, i.e.
    //! the display mode as well as the frame rate
    //! \param pixel_format Use this pixel format
    //! \param video_input_flags Use these video flags
    //! \param display_mode
    //! \param frame_rate
    //! \param cols detected frame width
    //! \param rows detected frame height
    //! \param frame_flags
    //! \param error_msg
    //! \return \c true on success, \c false otherwise,
    //! accompanied by a detailed error message, which
    //! could be used for instance for throwing an exception
    //!
    bool detect_input_format(BMDPixelFormat pixel_format,
                             BMDVideoInputFlags & video_input_flags,
                             BMDDisplayMode & display_mode,
                             double & frame_rate,
                             size_t & cols, size_t & rows,
                             BMDFrameFlags & frame_flags,
                             std::string & error_msg) noexcept;

    //!
    //! \brief Throw a VideoSourceError with the specified
    //! \c error_msg, after releasing DeckLink allocations
    //! \param error_msg
    //! \throw VideoSourceError
    //! \sa release_deck_link()
    //!
    inline void bail(std::string error_msg)
    {
        release_deck_link();
        throw VideoSourceError(error_msg);
    }

    //!
    //! \brief
    //! \return whether this source is in stereo mode
    //!
    inline bool is_stereo()
    {
        return _video_input_flags & bmdVideoInputDualStream3D;
    }

    //!
    //! \brief
    //! \return whether a post-capture colour conversion
    //! is needed
    //!
    inline bool need_conversion()
    {
        return _colour == BGRA and
               _12_bit_rgb_to_bgra_converter != nullptr;
    }

private:
    DISALLOW_COPY_AND_ASSIGNMENT(VideoSourceBlackmagicSDK);
};

}
