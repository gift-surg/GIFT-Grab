#include "maskframe.h"
#include <string.h>

MaskFrame::MaskFrame()
    :_rows(0), _cols(0), _data(0), _weights(0),
     _e_loc_x(-1), _e_loc_y(-1), _e_width_x(0), _e_width_y(0)
{}

MaskFrame::MaskFrame(const size_t rows, const size_t cols)
    :_rows(rows), _cols(cols), _e_loc_x(-1), _e_loc_y(-1), _e_width_x(0), _e_width_y(0)
{
    const size_t num_pixels = _rows * _cols;
    _data = new unsigned char[num_pixels];
    memset(_data, 0, num_pixels);
    _weights = new unsigned char[num_pixels];
    memset(_weights, 0, num_pixels);
}

MaskFrame::MaskFrame(const MaskFrame & rhs)
{
    clone(rhs);
}

void MaskFrame::operator =(const MaskFrame & rhs)
{
    clone(rhs);
}

MaskFrame::~MaskFrame()
{
    clear();
}

void MaskFrame::clear()
{
    delete[] _data;
    _data = 0;
    delete[] _weights;
    _weights = 0;
    _rows = _cols = 0;
}

void MaskFrame::clone(const MaskFrame &rhs)
{
    clear();
    _rows = rhs._rows;
    _cols = rhs._cols;
    const size_t num_pixels = _rows * _cols;
    _data = new unsigned char[num_pixels];
    memcpy(_data, rhs._data, num_pixels);
    _weights = new unsigned char[num_pixels];
    memcpy(_weights, rhs._weights, num_pixels);
}
