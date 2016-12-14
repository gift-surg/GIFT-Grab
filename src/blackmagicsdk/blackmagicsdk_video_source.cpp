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


VideoSourceBlackmagicSDK::VideoSourceBlackmagicSDK(ColourSpace colour)
    : IVideoSource(colour)
    , _video_buffer(nullptr)
    , _video_buffer_length(0)
    , _buffer_video_frame(VideoFrame(colour, false)) // TODO manage data?
{
    // TODO
}


VideoSourceBlackmagicSDK::~VideoSourceBlackmagicSDK()
{
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

}
