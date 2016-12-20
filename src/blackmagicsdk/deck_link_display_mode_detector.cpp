#include "deck_link_display_mode_detector.h"

namespace gg
{

DeckLinkDisplayModeDetector::DeckLinkDisplayModeDetector()
    : _deck_link_input(nullptr)
    , _display_mode(bmdModeUnknown)
    , _error_msg("This constructor should never be used")
{
    // nop
}


DeckLinkDisplayModeDetector::DeckLinkDisplayModeDetector(IDeckLinkInput * deck_link_input,
                                                         std::vector<BMDDisplayMode> display_modes_to_check,
                                                         BMDPixelFormat pixel_format,
                                                         BMDVideoInputFlags video_input_flags)
    : _deck_link_input(deck_link_input)
    , _display_modes_to_check(display_modes_to_check)
    , _pixel_format(pixel_format)
    , _video_input_flags(video_input_flags)
    , _display_mode(bmdModeUnknown)
    , _frame_rate(0.0)
    , _error_msg("")
{
    // TODO
}


DeckLinkDisplayModeDetector::~DeckLinkDisplayModeDetector()
{
    // nop
}


BMDDisplayMode DeckLinkDisplayModeDetector::get_display_mode() noexcept
{
    // TODO
    return bmdModeUnknown;
}


double DeckLinkDisplayModeDetector::get_frame_rate() noexcept
{
    // TODO
    return 0.0;
}


std::string DeckLinkDisplayModeDetector::get_error_msg() noexcept
{
    // TODO
    return "";
}


HRESULT STDMETHODCALLTYPE DeckLinkDisplayModeDetector::QueryInterface(REFIID iid, LPVOID * ppv)
{
    return E_NOINTERFACE;
}


ULONG STDMETHODCALLTYPE DeckLinkDisplayModeDetector::AddRef(void)
{
    __sync_add_and_fetch(&_n_ref, 1);
    return _n_ref;
}


ULONG STDMETHODCALLTYPE DeckLinkDisplayModeDetector::Release(void)
{
    __sync_sub_and_fetch(&_n_ref, 1);
    return _n_ref;
}


HRESULT STDMETHODCALLTYPE DeckLinkDisplayModeDetector::VideoInputFormatChanged(
    BMDVideoInputFormatChangedEvents events,
    IDeckLinkDisplayMode * display_mode,
    BMDDetectedVideoInputFormatFlags format_flags
)
{
    // nop
    return S_OK;
}


HRESULT STDMETHODCALLTYPE DeckLinkDisplayModeDetector::VideoInputFrameArrived(
    IDeckLinkVideoInputFrame * video_frame,
    IDeckLinkAudioInputPacket * audio_packet
)
{
    if (video_frame != nullptr)
    {
        if (not (video_frame->GetFlags() & bmdFrameHasNoInputSource))
            // TODO
            ;
    }
    return S_OK;
}

}
