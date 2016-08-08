// This class is based on the work by Luis Carlos Garcia-Peraza Herrera
#include "blackmagic_frame_grabber.h"
#include <stdexcept>

FrameGrabberBlackMagic::FrameGrabberBlackMagic()
    :IVideoSource(), _deck_link(0), _deck_link_input(0), _delegate(0), _initialized(false)
{}

FrameGrabberBlackMagic::FrameGrabberBlackMagic(BMDDisplayMode display_mode, BMDPixelFormat pixel_format)
{
    BMDDisplayModeSupport dm_supported;
    IDeckLinkIterator *dc_iter = CreateDeckLinkIteratorInstance();
    if (!dc_iter) {
        throw std::runtime_error("Failed to create the DeckLink iterator interface. "
                                 "Please install the DeckLink drivers");
    }

    HRESULT result = S_OK;
    bool card_found = false;
    while ((result = dc_iter->Next(&_deck_link)) == S_OK) {
        if (_deck_link->QueryInterface(IID_IDeckLinkInput, (void**)&_deck_link_input) == S_OK) {
            // Check if this supports the required display mode
            if (_deck_link_input->DoesSupportVideoMode(display_mode, pixel_format, bmdVideoInputFlagDefault,
                                                       &dm_supported, NULL) == S_OK) {
                // This is the card we want
                if (dm_supported == bmdDisplayModeSupported) {
                    card_found = true;
                    break;
                }
            }
            else {
                _deck_link->Release();
            }
        }
        else {
            _deck_link->Release();
        }
    }

    // Release the iterator
    dc_iter->Release();

    if (result != S_OK || _deck_link == NULL) {
        throw std::runtime_error("Failed to find a BlackMagic card.");
    }

    if (!card_found) {
        throw std::runtime_error("Could not find a card which supports the desired display format.");
    }

    // not clear if this ever has to be deleted or 'Released'. The BlackMagic driver and API is not great.
    // However, delete or release seems to crash it. So, do not try and fix this 'bug'.
    _delegate = new DeckLinkCaptureDelegate();

    if (!_delegate) {
        throw std::runtime_error("Failed to create decklink delegate");
    }

    if (_deck_link_input->SetCallback(_delegate) != S_OK) {
        throw std::runtime_error("Failed to set callback in the DeckLink input");
    }

    if (_deck_link_input->EnableVideoInput(display_mode, pixel_format, bmdVideoInputFlagDefault) != S_OK) {
        throw std::runtime_error("Failed to enable video input.");
    }

    start_capture();

    _initialized = true;
}

FrameGrabberBlackMagic::~FrameGrabberBlackMagic()
{
    stop_capture();
    if (_delegate) {

    }
}

void FrameGrabberBlackMagic::start_capture()
{
    if (!_deck_link_input || _deck_link_input->StartStreams() != S_OK) {
        throw std::runtime_error("Could not start video capture.");
    }
}

void FrameGrabberBlackMagic::stop_capture()
{
    if (_deck_link_input) {
        // Ignore any issues here..
        _deck_link_input->StopStreams();
    }
}

bool FrameGrabberBlackMagic::get_frame_dimensions(int & width, int & height)
{
    if (_delegate) {
        width = _delegate->WIDTH; height = _delegate->HEIGHT;
        return (width != 0 && height != 0);
    }
    else {
        width = height = 0;
        return false;
    }
}

double FrameGrabberBlackMagic::get_frame_rate()
{
    return 25.f;
}

bool FrameGrabberBlackMagic::get_frame(VideoFrame_BGRA &frame)
{
    if (_initialized) {
        cv::Mat mat = _delegate->getFrame();
        if (_get_sub_frame) {
            mat(_sub_frame).copyTo(_buffer_sub_bgra);
            frame.init_from_opencv_mat(_buffer_sub_bgra);
        }
        else {
            frame.init_from_opencv_mat(mat);
        }
        return true;
    }
    return false;
}
