#include "argb_to_bgra_converter.h"
#include <assert.h>

namespace gg
{

ArgbToBgraConverter::ArgbToBgraConverter()
    : _width(0)
    , _height(0)
#ifdef USE_FFMPEG
    , _sws_context(nullptr)
#endif
{

}

ArgbToBgraConverter::~ArgbToBgraConverter()
{
#ifdef USE_FFMPEG
    sws_freeContext(_sws_context);
    _sws_context = nullptr;
#endif
}

void ArgbToBgraConverter::convert(const unsigned char *argb,
                                  unsigned char *bgra)
{
    size_t length = 4 * _width * _height;
    for (size_t i = 0; i < length; i += 4)
    {
        bgra[i] = argb[i+3];
        bgra[i+1] = argb[i+2];
        bgra[i+2] = argb[i+1];
        bgra[i+3] = argb[i];
    }
}

void ArgbToBgraConverter::set_frame_dimensions(size_t width,
                                               size_t height)
{
    assert(width > 0);
    assert(height > 0);
    _width = width;
    _height = height;
}

}
