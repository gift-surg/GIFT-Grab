#include "deck_link_display_mode_detector.h"
#include <thread>
#include <chrono>

namespace gg
{

DeckLinkDisplayModeDetector::DeckLinkDisplayModeDetector()
    : _deck_link_input(nullptr)
    , _display_mode(bmdModeUnknown)
    , _error_msg("This constructor should never be used")
    , _running(false)
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
    // to be reset upon determining display mode
    , _error_msg("Could not determine display mode (unknown reason)")
    , _running(false)
{
    // These are output and result variables
    BMDDisplayModeSupport display_mode_support;
    IDeckLinkDisplayMode * deck_link_display_mode = nullptr;
    HRESULT res;

    _deck_link_input->SetCallback(this);
    for (BMDDisplayMode display_mode : _display_modes_to_check)
    {
        // Check whether the mode is supported
        _display_mode = display_mode;
        res = _deck_link_input->DoesSupportVideoMode(
            _display_mode, _pixel_format, _video_input_flags,
            &display_mode_support, &deck_link_display_mode
        );
        // No glory (could not even check mode support)
        if (res != S_OK)
        {
            _display_mode = bmdModeUnknown;
            _error_msg = "Could not check video mode support of Blackmagic DeckLink device";
            break;
        }

        // Mode not supported: go to next one
        if (display_mode_support != bmdDisplayModeSupported
            or deck_link_display_mode == nullptr)
        {
            _display_mode = bmdModeUnknown;
            continue;
        }

        // Enable video input for checking input coming
        res = _deck_link_input->EnableVideoInput(display_mode, _pixel_format, _video_input_flags);
        if (res != S_OK) // No glory: check next display mode
        {
            _display_mode = bmdModeUnknown;
            continue;
        }

        // Start streaming for checking input coming
        _running = true;
        res = _deck_link_input->StartStreams();
        if (res != S_OK) // No glory: check next display mode
        {
            _running = false;
            _deck_link_input->DisableVideoInput();
            _display_mode = bmdModeUnknown;
            continue;
        }

        // Allow input to come through
        while (_running)
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // Stop streaming and disable video input
        _deck_link_input->StopStreams();
        _deck_link_input->DisableVideoInput();

        // Glory!
        if (_display_mode != bmdModeUnknown)
        {
            _error_msg = "";
            break;
        }
    }
    _deck_link_input->SetCallback(nullptr);

    // If mode supported, then get frame rate
    if (_display_mode != bmdModeUnknown)
    {
        // Get frame rate of DeckLink device
        BMDTimeValue frame_rate_duration, frame_rate_scale;
        res = deck_link_display_mode->GetFrameRate(&frame_rate_duration, &frame_rate_scale);
        // No glory
        if (res != S_OK)
        {
            _display_mode = bmdModeUnknown;
            _error_msg = "Could not infer frame rate of Blackmagic DeckLink device";
        }
        else
            _frame_rate = (double) frame_rate_scale / (double) frame_rate_duration;
    }

    // Release the DeckLink display mode object
    if (deck_link_display_mode != nullptr)
    {
        deck_link_display_mode->Release();
        deck_link_display_mode = nullptr;
    }
}


DeckLinkDisplayModeDetector::~DeckLinkDisplayModeDetector()
{
    // nop
}


BMDDisplayMode DeckLinkDisplayModeDetector::get_display_mode() noexcept
{
    return _display_mode;
}


double DeckLinkDisplayModeDetector::get_frame_rate() noexcept
{
    return _frame_rate;
}


std::string DeckLinkDisplayModeDetector::get_error_msg() noexcept
{
    return _error_msg;
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
    if (_running)
    {
        if (video_frame != nullptr)
        {
            if (video_frame->GetFlags() & bmdFrameHasNoInputSource)
                _display_mode = bmdModeUnknown;
            _running = false;
        }
    }
    return S_OK;
}

}
