#include "blackmagicsdk_video_source.h"

namespace gg
{

VideoSourceBlackmagicSDK::VideoSourceBlackmagicSDK()
    : IVideoSource()
    , _video_buffer(nullptr)
    , _video_buffer_length(0)
    // This constructor should never be called,
    // therefore setting I420 arbitrarily for the
    // buffer video frame here.
    , _buffer_video_frame(VideoFrame(I420))
    , _running(false)
{
    // nop
}


VideoSourceBlackmagicSDK::VideoSourceBlackmagicSDK(size_t deck_link_index,
                                                   ColourSpace colour)
    : IVideoSource(colour)
    , _video_buffer(nullptr)
    , _video_buffer_length(0)
    , _buffer_video_frame(VideoFrame(colour, false)) // TODO manage data?
    , _deck_link(nullptr)
    , _deck_link_input(nullptr)
    , _running(false)
{
    switch(_colour)
    {
    case BGRA:
        break;
    case I420:
    default:
        release_deck_link();
        throw VideoSourceError(
            "BlackmagicSDK video source supports only BGRA"
        );
    }

    // Get an iterator through the available DeckLink ports
    IDeckLinkIterator * deck_link_iterator = CreateDeckLinkIteratorInstance();
    if (deck_link_iterator == nullptr)
    {
        release_deck_link();
        throw VideoSourceError("DeckLink drivers do not appear to be installed");
    }

    HRESULT res;

    // Get the desired DeckLink index (port)
    int idx = deck_link_index;
    while ((res = deck_link_iterator->Next(&_deck_link)) == S_OK)
    {
        if (idx == 0)
            break;
        --idx;

        _deck_link->Release();
    }
    if (deck_link_iterator != nullptr)
        deck_link_iterator->Release();
    // No glory: release everything and throw exception
    if (res != S_OK or _deck_link == nullptr)
    {
        release_deck_link();
        throw VideoSourceError(
            std::string("Could not get DeckLink device ").append(std::to_string(deck_link_index))
        );
    }

    // Get the input interface of connected DeckLink port
    res = _deck_link->QueryInterface(IID_IDeckLinkInput, reinterpret_cast<void **>(&_deck_link_input));
    // No glory: release everything and throw exception
    if (res != S_OK)
    {
        release_deck_link();
        throw VideoSourceError("Could not connect to any Blackmagic DeckLink device");
    }

    // Set this object (IDeckLinkInputCallback instance) as callback
    res = _deck_link_input->SetCallback(this);
    // No glory: release everything and throw exception
    if (res != S_OK)
    {
        release_deck_link();
        throw VideoSourceError("Could not set the callback of Blackmagic DeckLink device");
    }

    // Enable video input
    res = _deck_link_input->EnableVideoInput(
        bmdModeHD1080p6000, bmdFormat8BitBGRA,
        bmdVideoInputFlagDefault | bmdVideoInputEnableFormatDetection
    );
    // No glory: release everything and throw exception
    if (res != S_OK)
    {
        release_deck_link();
        throw VideoSourceError("Could not enable video input of Blackmagic DeckLink device");
    }

    // Disable audio input
    res = _deck_link_input->DisableAudioInput();
    // No glory: release everything and throw exception
    if (res != S_OK)
    {
        release_deck_link();
        throw VideoSourceError("Could not disable audio input of Blackmagic DeckLink device");
    }
}


VideoSourceBlackmagicSDK::~VideoSourceBlackmagicSDK()
{
    // Release DeckLink members
    release_deck_link();

    // Release allocated memory
    free(_video_buffer);
    // TODO
}


bool VideoSourceBlackmagicSDK::get_frame_dimensions(int & width, int & height)
{
    // TODO
}


bool VideoSourceBlackmagicSDK::get_frame(VideoFrame & frame)
{
    // TODO
    return false;
}


double VideoSourceBlackmagicSDK::get_frame_rate()
{
    // TODO
    return 0;
}


void VideoSourceBlackmagicSDK::set_sub_frame(int x, int y, int width, int height)
{
    // TODO
}


void VideoSourceBlackmagicSDK::get_full_frame()
{
    // TODO
}


HRESULT STDMETHODCALLTYPE VideoSourceBlackmagicSDK::QueryInterface(REFIID iid, LPVOID * ppv)
{
    // TODO
}


ULONG STDMETHODCALLTYPE VideoSourceBlackmagicSDK::AddRef(void)
{
    // TODO
}


ULONG STDMETHODCALLTYPE VideoSourceBlackmagicSDK::Release(void)
{
    // TODO
}


HRESULT STDMETHODCALLTYPE VideoSourceBlackmagicSDK::VideoInputFormatChanged(
    BMDVideoInputFormatChangedEvents events,
    IDeckLinkDisplayMode * display_mode,
    BMDDetectedVideoInputFormatFlags format_flags
)
{
    // TODO
}


HRESULT STDMETHODCALLTYPE VideoSourceBlackmagicSDK::VideoInputFrameArrived(
    IDeckLinkVideoInputFrame * video_frame,
    IDeckLinkAudioInputPacket * audio_packet
)
{
    // TODO
}


void VideoSourceBlackmagicSDK::release_deck_link() noexcept
{
    this->Release();

    if (_deck_link_input != nullptr)
    {
        _deck_link_input->Release();
        _deck_link_input = nullptr;
    }

    if (_deck_link != nullptr)
        _deck_link->Release();
}

}
