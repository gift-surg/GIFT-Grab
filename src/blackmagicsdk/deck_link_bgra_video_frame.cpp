#include "deck_link_bgra_video_frame.h"

namespace gg
{

DeckLinkBGRAVideoFrame::DeckLinkBGRAVideoFrame(
    size_t width, size_t height,
    LPVOID pixel_buffer, BMDFrameFlags frame_flags
)
{
    // TODO
}

DeckLinkBGRAVideoFrame::~DeckLinkBGRAVideoFrame()
{
    // TODO
}

long STDMETHODCALLTYPE
DeckLinkBGRAVideoFrame::GetWidth(void)
{
    // TODO
}

long STDMETHODCALLTYPE
DeckLinkBGRAVideoFrame::GetHeight(void)
{
    // TODO
}

long STDMETHODCALLTYPE
DeckLinkBGRAVideoFrame::GetRowBytes(void)
{
    // TODO
}

HRESULT STDMETHODCALLTYPE
DeckLinkBGRAVideoFrame::GetBytes(void **buffer)
{
    // TODO
}

BMDFrameFlags STDMETHODCALLTYPE
DeckLinkBGRAVideoFrame::GetFlags(void)
{
    // TODO
}

BMDPixelFormat STDMETHODCALLTYPE
DeckLinkBGRAVideoFrame::GetPixelFormat(void)
{
    // TODO
}

HRESULT STDMETHODCALLTYPE
DeckLinkBGRAVideoFrame::QueryInterface(
    REFIID iid, LPVOID *ppv
)
{
    // TODO
}

ULONG STDMETHODCALLTYPE DeckLinkBGRAVideoFrame::AddRef()
{
    // TODO
}

ULONG STDMETHODCALLTYPE DeckLinkBGRAVideoFrame::Release()
{
    // TODO
}

HRESULT STDMETHODCALLTYPE
DeckLinkBGRAVideoFrame::GetAncillaryData(
    IDeckLinkVideoFrameAncillary **ancillary
)
{
    // TODO
}

HRESULT STDMETHODCALLTYPE
DeckLinkBGRAVideoFrame::GetTimecode(
    BMDTimecodeFormat format, IDeckLinkTimecode** timecode
)
{
    // TODO
}

}
