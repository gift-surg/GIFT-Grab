#pragma once
#include <DeckLinkAPI.h>
#include <vector>
#include <string>

namespace gg
{

//!
//! \brief This class is a surrogate for detecting what \c
//! BMDDisplayMode a passed \c IDeckLinkInput interface is
//! actually receiving
//!
class DeckLinkDisplayModeDetector : public IDeckLinkInputCallback
{
protected:
    //!
    //! \brief
    //!
    IDeckLinkInput * _deck_link_input;

    //!
    //! \brief Reference count
    //! \sa AddRef()
    //! \sa Release()
    //!
    int32_t _n_ref;

    //!
    //! \brief
    //!
    std::vector<BMDDisplayMode> _display_modes_to_check;

    //!
    //! \brief
    //!
    BMDPixelFormat _pixel_format;

    //!
    //! \brief
    //!
    BMDVideoInputFlags _video_input_flags;

    //!
    //! \brief
    //!
    BMDDisplayMode _display_mode;

    //!
    //! \brief
    //!
    double _frame_rate;

    //!
    //! \brief
    //!
    std::string _error_msg;

    //!
    //! \brief Flag indicating whether the streaming
    //! thread is running
    //!
    bool _running;

protected:
    //!
    //! \brief Default constructor that should not be used
    //! externally
    //!
    DeckLinkDisplayModeDetector();

public:
    //!
    //! \brief Sequentially try all the display modes passed,
    //! and stop at the first one that actually works.
    //!
    //! Also infer the frame rate of the video input using
    //! the mechanics of Blackmagic Design SDK.
    //!
    //! \param deck_link_input It is the caller's responsibility
    //! to ensure the lifetime of the object pointed to by this
    //! pointer is at least as long as the object constructed by
    //! this constructor. As a safety net, this constructor does
    //! nop if the passed pointer is null.
    //! \param display_modes_to_check These are checked first-
    //! come-first-served: the first one found to be working is
    //! returned
    //! \param pixel_format The colour space
    //! \param video_input_flags
    //! \sa get_display_mode()
    //! \sa get_frame_rate()
    //!
    DeckLinkDisplayModeDetector(IDeckLinkInput * deck_link_input,
                                std::vector<BMDDisplayMode> display_modes_to_check,
                                BMDPixelFormat pixel_format,
                                BMDVideoInputFlags video_input_flags);

    //!
    //! \brief
    //!
    virtual ~DeckLinkDisplayModeDetector();

public:
    //!
    //! \brief
    //!
    //! \return The display mode that works, if at all,
    //! or \c bmdModeUnknown if none of the passed display
    //! modes work
    //!
    BMDDisplayMode get_display_mode() noexcept;

    //!
    //! \brief
    //! \return
    //!
    double get_frame_rate() noexcept;

    //!
    //! \brief Get the last detailed error message set
    //! \return a detailed message describing the last
    //! encountered error
    //!
    std::string get_error_msg() noexcept;

    // IDeckLinkInputCallback overrides

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, LPVOID * ppv) override;

    ULONG STDMETHODCALLTYPE AddRef(void) override;

    ULONG STDMETHODCALLTYPE Release(void) override;

    HRESULT STDMETHODCALLTYPE VideoInputFormatChanged(
        BMDVideoInputFormatChangedEvents events,
        IDeckLinkDisplayMode * display_mode,
        BMDDetectedVideoInputFormatFlags format_flags
    ) override;

    HRESULT STDMETHODCALLTYPE VideoInputFrameArrived(
        IDeckLinkVideoInputFrame * video_frame,
        IDeckLinkAudioInputPacket * audio_packet
    ) override;
};

}
