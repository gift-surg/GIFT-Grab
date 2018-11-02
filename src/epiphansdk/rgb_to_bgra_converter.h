#pragma once

#include <cstdlib>
#ifdef USE_FFMPEG
extern "C" {
#include <libswscale/swscale.h>
}
#endif

namespace gg
{

//!
//! \brief Epiphan SDK seems to support RGB but not BGRA.
//! This class abstracts the conversion of captured RGB
//! data to BGRA.
//! \sa VideoSourceEpiphanSDK
//!
class RgbToBgraConverter
{
protected:
    //!
    //! \brief Width of frames passed for conversion
    //!
    size_t _width;

    //!
    //! \brief Height of frames passed for conversion
    //!
    size_t _height;

#ifdef USE_FFMPEG
    unsigned char *_sws_srcSlice[1];
    int _sws_srcStride[1];
    unsigned char *_sws_dst[1];
    int _sws_dstStride[1];
    AVPixelFormat _sws_srcFormat = AV_PIX_FMT_RGB24;
    AVPixelFormat _sws_dstFormat = AV_PIX_FMT_BGRA;
    SwsContext *_sws_context;
#endif

public:
    //!
    //! \brief Create a converter, which should be
    //! initialised by informing it of the frame
    //! dimensions
    //! \sa set_frame_dimensions
    //!
    RgbToBgraConverter();

    //!
    //! \brief Free all allocated resources
    //!
    ~RgbToBgraConverter();

public:
    //!
    //! \brief Convert RGB data in passed buffer to BGRA
    //! data, saving it into the passed output buffer.
    //! \param rgb should be in line with the frame
    //! dimensions set before
    //! \param bgra should be at least as large as the
    //! input buffer
    //! \sa set_frame_dimensions
    //!
    void convert(unsigned char *rgb, unsigned char *bgra);

    //!
    //! \brief Allocate all resources needed for RGB to
    //! BGRA conversion based on passed frame dimensions
    //! \param width
    //! \param height
    //!
    void set_frame_dimensions(size_t width, size_t height);
};

}
