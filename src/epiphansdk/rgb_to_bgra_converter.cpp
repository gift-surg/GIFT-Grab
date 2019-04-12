#include "rgb_to_bgra_converter.h"
#include <assert.h>

namespace gg
{

RgbToBgraConverter::RgbToBgraConverter()
    : _width(0)
    , _height(0)
#ifdef USE_FFMPEG
    , _sws_context(nullptr)
#endif
{

}

RgbToBgraConverter::~RgbToBgraConverter()
{
#ifdef USE_FFMPEG
    sws_freeContext(_sws_context);
    _sws_context = nullptr;
#endif
}

void RgbToBgraConverter::convert(unsigned char *rgb,
                                 unsigned char *bgra)
{
#ifdef USE_FFMPEG
    _sws_srcSlice[0] = rgb;
    _sws_srcStride[0] = 3 * _width;
    _sws_dst[0] = bgra;
    _sws_dstStride[0] = 4 * _width;
    sws_scale(_sws_context,
              _sws_srcSlice, _sws_srcStride, 0, _height,
              _sws_dst, _sws_dstStride);
#else
    size_t length = 4 * _width * _height;
    for (size_t i = 0, j = 0; i < length; i += 4, j += 3)
    {
        bgra[i] = rgb[j+2];
        bgra[i+1] = rgb[j+1];
        bgra[i+2] = rgb[j];
        bgra[i+3] = 255;
    }
#endif
}

void RgbToBgraConverter::set_frame_dimensions(size_t width,
                                              size_t height)
{
    assert(width > 0);
    assert(height > 0);
    if (_width == width && _height == height)
        return;

    _width = width;
    _height = height;
#ifdef USE_FFMPEG
    _sws_context = sws_getCachedContext(
                _sws_context,
                _width, _height, _sws_srcFormat,
                _width, _height, _sws_dstFormat,
                0, nullptr, nullptr, nullptr       // advanced
                );
#endif
}

}
