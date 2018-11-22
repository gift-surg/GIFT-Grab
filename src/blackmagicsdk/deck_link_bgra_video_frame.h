#pragma once

#include <cstdlib>
#include "DeckLinkAPI.h"

namespace gg
{

//!
//! \brief A very un-intelligent implementation of
//! Blackmagic SDK's video frame interface. This class is
//! not for external use, as such it does absolutely NO
//! sanity checking, or management of data passed to it.
//! It expects ALL of that to be done by the caller.
//!
class DeckLinkBGRAVideoFrame : public IDeckLinkVideoFrame
{
protected:
    // members for implementing promised API
    long            _width;
    long            _height;
    BMDFrameFlags   _flags;
    LPVOID          _pixel_buffer = nullptr;
    ULONG           _ref_count;

public:
    //!
    //! \brief Wrap passed parameters into a Blackmagic
    //! SDK video frame, with absolutely NO sanity
    //! checking
    //! \param width
    //! \param height
    //! \param pixel_buffer
    //! \param frame_flags
    //!
    DeckLinkBGRAVideoFrame(size_t width, size_t height,
                           LPVOID pixel_buffer, BMDFrameFlags frame_flags);

    //!
    //! \brief Simply destroy this object, DO NOTHING
    //! about data
    //!
    virtual ~DeckLinkBGRAVideoFrame();

public:
    // inherited methods
    virtual long STDMETHODCALLTYPE GetWidth(void);
    virtual long STDMETHODCALLTYPE GetHeight(void);
    virtual long STDMETHODCALLTYPE GetRowBytes(void);
    virtual HRESULT STDMETHODCALLTYPE GetBytes(void** buffer);
    virtual BMDFrameFlags STDMETHODCALLTYPE GetFlags(void);
    virtual BMDPixelFormat STDMETHODCALLTYPE GetPixelFormat(void);

    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, LPVOID *ppv);
    virtual ULONG STDMETHODCALLTYPE AddRef();
    virtual ULONG STDMETHODCALLTYPE Release();

    // Dummy implementations of remaining methods
    virtual HRESULT STDMETHODCALLTYPE GetAncillaryData(IDeckLinkVideoFrameAncillary** ancillary);
    virtual HRESULT STDMETHODCALLTYPE GetTimecode(BMDTimecodeFormat format, IDeckLinkTimecode** timecode);
};

}
