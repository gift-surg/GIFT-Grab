#include "blackmagicsdk_video_source.h"
#include "deck_link_display_mode_detector.h"
#include <chrono>

namespace gg
{

VideoSourceBlackmagicSDK::VideoSourceBlackmagicSDK()
    : IVideoSource()
    , _frame_rate(0.0)
    , _video_buffer(nullptr)
    , _video_buffer_length(0)
    , _cols(0)
    , _rows(0)
    , _buffer_video_frame(VideoFrame(UYVY))
    , _running(false)
{
    // nop
}


VideoSourceBlackmagicSDK::VideoSourceBlackmagicSDK(size_t deck_link_index,
                                                   ColourSpace colour)
    : IVideoSource(colour)
    , _frame_rate(0.0)
    , _video_buffer(nullptr)
    , _video_buffer_length(0)
    , _cols(0)
    , _rows(0)
    , _buffer_video_frame(VideoFrame(colour, false)) // TODO manage data?
    , _deck_link(nullptr)
    , _deck_link_input(nullptr)
    , _video_input_flags(bmdVideoInputFlagDefault | bmdVideoInputDualStream3D)
    , _12_bit_rgb_to_bgra_converter(nullptr)
    , _bgra_frame_buffers({nullptr, nullptr})
    , _running(false)
{
    // Pixel format, i.e. colour space
    BMDPixelFormat pixel_format;
    switch(_colour)
    {
    case UYVY:
        pixel_format = bmdFormat8BitYUV;
        break;
    case BGRA:
        // We currently only support BGRA with DeckLink 4K Extreme 12G,
        // and that card supports only this YUV format:
        pixel_format = bmdFormat10BitYUV;
        break;
    case I420:
    default:
        bail("BlackmagicSDK video source supports only UYVY and BGRA colour spaces");
    }

    // Get an iterator through the available DeckLink ports
    IDeckLinkIterator * deck_link_iterator = CreateDeckLinkIteratorInstance();
    if (deck_link_iterator == nullptr)
        bail("DeckLink drivers do not appear to be installed");

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
        bail(
            std::string("Could not get DeckLink device ").append(std::to_string(deck_link_index))
        );

    // Get the input interface of connected DeckLink port
    res = _deck_link->QueryInterface(IID_IDeckLinkInput, reinterpret_cast<void **>(&_deck_link_input));
    // No glory: release everything and throw exception
    if (res != S_OK)
        bail("Could not get the Blackmagic DeckLink input interface");

    // Set the input format (i.e. display mode)
    BMDDisplayMode display_mode;
    std::string error_msg = "";
    if (not detect_input_format(pixel_format, _video_input_flags, display_mode,
                                _frame_rate, _cols, _rows, error_msg))
    {
        _video_input_flags ^= bmdVideoInputDualStream3D;
        if (not detect_input_format(pixel_format, _video_input_flags, display_mode,
                                    _frame_rate, _cols, _rows, error_msg))
            bail(error_msg);
    }

    // Set this object (IDeckLinkInputCallback instance) as callback
    res = _deck_link_input->SetCallback(this);
    // No glory: release everything and throw exception
    if (res != S_OK)
        bail("Could not set the callback of Blackmagic DeckLink device");

    // Enable video input
    res = _deck_link_input->EnableVideoInput(display_mode,
                                             pixel_format,
                                             _video_input_flags);
    // No glory
    if (res != S_OK)
        bail("Could not enable video input of Blackmagic DeckLink device");

    // Colour converter for post-capture colour conversion
    if (_colour == BGRA)
    {
        _12_bit_rgb_to_bgra_converter = CreateVideoConversionInstance();
        if (_12_bit_rgb_to_bgra_converter == nullptr)
            bail("Could not create colour converter for Blackmagic source");
    }

    // Start streaming
    _running = true;
    res = _deck_link_input->StartStreams();
    // No glory: release everything and throw exception
    if (res != S_OK)
    {
        _running = false;
        bail("Could not start streaming from the Blackmagic DeckLink device");
    }
}


VideoSourceBlackmagicSDK::~VideoSourceBlackmagicSDK()
{
    { // Artificial scope for data lock
        // Make sure streamer thread not trying to access buffer
        std::lock_guard<std::mutex> data_lock_guard(_data_lock);
        _running = false;
    }

    // Stop streaming and disable enabled inputs
    _deck_link_input->SetCallback(nullptr);
    _deck_link_input->StopStreams();
    _deck_link_input->DisableVideoInput();

    // Release DeckLink members
    release_deck_link();
}


bool VideoSourceBlackmagicSDK::get_frame_dimensions(int & width, int & height)
{
    // Make sure only this thread is accessing the cols and rows members now
    std::lock_guard<std::mutex> data_lock_guard(_data_lock);
    if (_cols <= 0 or _rows <= 0)
        return false;

    width = _cols;
    height = _rows;
    return true;
}


bool VideoSourceBlackmagicSDK::get_frame(VideoFrame & frame)
{
    if (frame.colour() != _colour)
        return false;

    // Make sure only this thread is accessing the video frame data now
    std::lock_guard<std::mutex> data_lock_guard(_data_lock);

    if (_video_buffer_length > 0 and _cols > 0 and _rows > 0)
    {
        frame.init_from_specs(_video_buffer, _video_buffer_length, _cols, _rows);
        return true;
    }
    else
        return false;
}


double VideoSourceBlackmagicSDK::get_frame_rate()
{
    return _frame_rate;
}


void VideoSourceBlackmagicSDK::set_sub_frame(int x, int y, int width, int height)
{
    // issue #147
    throw VideoSourceError("Blackmagic does not support cropping yet");
}


void VideoSourceBlackmagicSDK::get_full_frame()
{
    // nop: set_sub_frame currently not implemented
}


HRESULT STDMETHODCALLTYPE VideoSourceBlackmagicSDK::QueryInterface(REFIID iid, LPVOID * ppv)
{
    return E_NOINTERFACE;
}


ULONG STDMETHODCALLTYPE VideoSourceBlackmagicSDK::AddRef(void)
{
    __sync_add_and_fetch(&_n_ref, 1);
    return _n_ref;
}


ULONG STDMETHODCALLTYPE VideoSourceBlackmagicSDK::Release(void)
{
    __sync_sub_and_fetch(&_n_ref, 1);
    return _n_ref;
}


HRESULT STDMETHODCALLTYPE VideoSourceBlackmagicSDK::VideoInputFormatChanged(
    BMDVideoInputFormatChangedEvents events,
    IDeckLinkDisplayMode * display_mode,
    BMDDetectedVideoInputFormatFlags format_flags
)
{
    // not supported yet: see issue #149
    return S_OK;
}


HRESULT STDMETHODCALLTYPE VideoSourceBlackmagicSDK::VideoInputFrameArrived(
    IDeckLinkVideoInputFrame * video_frame,
    IDeckLinkAudioInputPacket * audio_packet
)
{
    if (not _running)
        // nop if not running!
        return S_OK;

    // Not processing the audio packet, but only the video
    // frame for the time being
    if (video_frame == nullptr)
        // nop if no data
        return S_OK;

    // Nr. of bytes of received data
    size_t n_bytes = VideoFrame::required_data_length(
        _colour, video_frame->GetWidth(), video_frame->GetHeight()
    );
    if (is_stereo())
        n_bytes *= 2;

    { // Artificial scope for data lock
        // Make sure only this thread is accessing the buffer now
        std::lock_guard<std::mutex> data_lock_guard(_data_lock);

        // Extend buffer if more memory needed than already allocated
        if (n_bytes > _video_buffer_length)
            _video_buffer = reinterpret_cast<uint8_t *>(
                        realloc(_video_buffer, n_bytes * sizeof(uint8_t))
            );
        if (_video_buffer == nullptr) // something's terribly wrong!
            // nop if something's terribly wrong!
            return S_OK;

        HRESULT res;

        if (_colour == BGRA)
        {
            // convert to BGRA from capture format
            if (_bgra_frame_buffers[0] == nullptr)
                _bgra_frame_buffers[0] = new DeckLinkBGRAVideoFrame(
                    video_frame->GetWidth(), video_frame->GetHeight(),
                    _video_buffer, video_frame->GetFlags()
                );
            res = _12_bit_rgb_to_bgra_converter->ConvertFrame(
                video_frame, _bgra_frame_buffers[0]
            );
        }
        else
            // Get the new data into the buffer
            res = video_frame->GetBytes(
                reinterpret_cast<void **>(&_video_buffer)
            );

        // If the last operation failed, return
        if (FAILED(res))
            return res;

        if (is_stereo())
        {
            IDeckLinkVideoFrame *right_eye_frame = nullptr;
            IDeckLinkVideoFrame3DExtensions *three_d_extensions = nullptr;
            if ((video_frame->QueryInterface(
                    IID_IDeckLinkVideoFrame3DExtensions,
                    (void **) &three_d_extensions) != S_OK) ||
                (three_d_extensions->GetFrameForRightEye(
                    &right_eye_frame) != S_OK))
            {
                right_eye_frame = nullptr;
            }

            if (three_d_extensions != nullptr)
                three_d_extensions->Release();

            if (right_eye_frame != nullptr)
            {
                if (_colour == BGRA)
                {
                    // convert to BGRA from capture format
                    if (_bgra_frame_buffers[1] == nullptr)
                        _bgra_frame_buffers[1] = new DeckLinkBGRAVideoFrame(
                            video_frame->GetWidth(), video_frame->GetHeight(),
                            &_video_buffer[n_bytes / 2], video_frame->GetFlags()
                        );
                    res = _12_bit_rgb_to_bgra_converter->ConvertFrame(
                        right_eye_frame, _bgra_frame_buffers[1]
                    );
                }
                else
                    res = right_eye_frame->GetBytes(
                        reinterpret_cast<void **>(&_video_buffer[n_bytes / 2])
                    );
                right_eye_frame->Release();
                // If data could not be read into the buffer, return
                if (FAILED(res))
                    return res;
            }
        }

        // Set video frame specs according to new data
        _video_buffer_length = n_bytes;
        _cols = video_frame->GetWidth();
        _rows = video_frame->GetHeight();

        // Propagate new video frame to observers
        _buffer_video_frame.init_from_specs(
            _video_buffer, _video_buffer_length, _cols, _rows,
            is_stereo() ? 2 : 1
        );
    }

    this->notify(_buffer_video_frame);

    // Everything went fine, return success
    return S_OK;
}


void VideoSourceBlackmagicSDK::release_deck_link() noexcept
{
    if (_deck_link_input != nullptr)
    {
        _deck_link_input->Release();
        _deck_link_input = nullptr;
    }

    if (_deck_link != nullptr)
        _deck_link->Release();

    if (_12_bit_rgb_to_bgra_converter != nullptr)
    {
        _12_bit_rgb_to_bgra_converter->Release();
        _12_bit_rgb_to_bgra_converter = nullptr;
    }

    for (size_t i = 0; i < 2; i++)
        if (_bgra_frame_buffers[i] != nullptr)
        {
            _bgra_frame_buffers[i]->Release();
            _bgra_frame_buffers[i] = nullptr;
        }
}


bool VideoSourceBlackmagicSDK::detect_input_format(BMDPixelFormat pixel_format,
                                                   BMDVideoInputFlags & video_input_flags,
                                                   BMDDisplayMode & display_mode,
                                                   double & frame_rate,
                                                   size_t & cols, size_t & rows,
                                                   std::string & error_msg) noexcept
{
    std::vector<BMDDisplayMode> display_modes =
    {
        bmdModeHD1080p6000, bmdModeHD1080p5994,
        bmdModeHD1080p50,
        bmdModeHD1080p30, bmdModeHD1080p2997,
        bmdModeHD1080p25, bmdModeHD1080p24, bmdModeHD1080p2398,
        bmdModeHD1080i6000, bmdModeHD1080i5994,
        bmdModeHD1080i50,
        bmdModeHD720p60, bmdModeHD720p5994,
        bmdModeHD720p50,
        bmdMode4K2160p60, bmdMode4K2160p5994,
        bmdMode4K2160p50,
        bmdMode4K2160p30, bmdMode4K2160p2997,
        bmdMode4K2160p25, bmdMode4K2160p24, bmdMode4K2160p2398,
        bmdMode2k25, bmdMode2k24, bmdMode2k2398,
    };

    DeckLinkDisplayModeDetector detector(
                _deck_link_input,
                display_modes, pixel_format, video_input_flags
    );
    BMDDisplayMode display_mode_ = detector.get_display_mode();
    if (display_mode_ != bmdModeUnknown)
    {
        frame_rate = detector.get_frame_rate();
        detector.get_frame_dimensions(cols, rows);
        display_mode = display_mode_;
        video_input_flags = detector.get_video_input_flags();
        return true;
    }
    else
    {
        error_msg = detector.get_error_msg();
        return false;
    }
}

}
