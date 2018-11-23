#include "deck_link_bgra_video_frame.h"
#include <cstring>

namespace gg
{

DeckLinkBGRAVideoFrame::DeckLinkBGRAVideoFrame(
    size_t width, size_t height,
    LPVOID pixel_buffer, BMDFrameFlags frame_flags
)
    : _width(width)
    , _height(height)
    , _pixel_buffer(pixel_buffer)
    , _flags(frame_flags)
{
    // nop
}

DeckLinkBGRAVideoFrame::~DeckLinkBGRAVideoFrame()
{
    _width = 0;
    _height = 0;
    _pixel_buffer = nullptr;
}

long STDMETHODCALLTYPE
DeckLinkBGRAVideoFrame::GetWidth(void)
{
    return _width;
}

long STDMETHODCALLTYPE
DeckLinkBGRAVideoFrame::GetHeight(void)
{
    return _height;
}

long STDMETHODCALLTYPE
DeckLinkBGRAVideoFrame::GetRowBytes(void)
{
    return 4 * _width;
}

HRESULT STDMETHODCALLTYPE
DeckLinkBGRAVideoFrame::GetBytes(void **buffer)
{
    if (_pixel_buffer == nullptr)
        return S_FALSE;
    *buffer = _pixel_buffer;
    return S_OK;
}

BMDFrameFlags STDMETHODCALLTYPE
DeckLinkBGRAVideoFrame::GetFlags(void)
{
    return _flags;
}

BMDPixelFormat STDMETHODCALLTYPE
DeckLinkBGRAVideoFrame::GetPixelFormat(void)
{
    return bmdFormat8BitBGRA;
}

HRESULT STDMETHODCALLTYPE
DeckLinkBGRAVideoFrame::QueryInterface(
    REFIID iid, LPVOID *ppv
)
{
    return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE DeckLinkBGRAVideoFrame::AddRef()
{
    __sync_add_and_fetch(&_ref_count, 1);
    return _ref_count;
}

ULONG STDMETHODCALLTYPE DeckLinkBGRAVideoFrame::Release()
{
    __sync_sub_and_fetch(&_ref_count, 1);
    return _ref_count;
}

HRESULT STDMETHODCALLTYPE
DeckLinkBGRAVideoFrame::GetAncillaryData(
    IDeckLinkVideoFrameAncillary **ancillary
)
{
    // nop
}

HRESULT STDMETHODCALLTYPE
DeckLinkBGRAVideoFrame::GetTimecode(
    BMDTimecodeFormat format, IDeckLinkTimecode** timecode
)
{
    // nop
}

}
