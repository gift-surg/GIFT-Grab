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
{
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

    // If no glory, release everything and throw exception
    if (res != S_OK or _deck_link == nullptr)
    {
        release_deck_link();
        throw VideoSourceError(
            std::string("Could not get DeckLink device ").append(std::to_string(deck_link_index))
        );
    }
}


VideoSourceBlackmagicSDK::~VideoSourceBlackmagicSDK()
{
    release_deck_link();
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
