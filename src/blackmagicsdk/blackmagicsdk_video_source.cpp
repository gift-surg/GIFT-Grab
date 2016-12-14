#include "blackmagicsdk_video_source.h"

namespace gg
{

VideoSourceBlackmagicSDK::VideoSourceBlackmagicSDK()
    : IVideoSource()
{
    // nop
}


VideoSourceBlackmagicSDK::VideoSourceBlackmagicSDK(ColourSpace colour)
    : IVideoSource(colour)
{
    // TODO
}


VideoSourceBlackmagicSDK::~VideoSourceBlackmagicSDK()
{
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
