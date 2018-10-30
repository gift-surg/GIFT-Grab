#include "argb_to_bgra_converter.h"
#include <assert.h>

namespace gg
{

ArgbToBgraConverter::ArgbToBgraConverter()
    : _width(0)
    , _height(0)
{

}

ArgbToBgraConverter::~ArgbToBgraConverter()
{

}

void ArgbToBgraConverter::convert(const unsigned char *argb,
                                  unsigned char *bgra)
{

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
