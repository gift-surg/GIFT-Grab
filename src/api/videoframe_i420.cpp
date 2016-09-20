#include "videoframe_i420.h"
#include <cstring>

namespace gg
{

VideoFrame_I420::VideoFrame_I420(bool manage_data)
    : VideoFrame(manage_data)
{

}

VideoFrame_I420::VideoFrame_I420(const size_t cols, const size_t rows)
    : gg::VideoFrame(true)
{
#ifdef USE_OPENCV
    cv::Mat buffer = cv::Mat::zeros(rows, cols, CV_8UC4);
    cv::cvtColor(buffer, buffer, CV_BGRA2YUV_I420);
    init_from_pointer(buffer.data, buffer.total(), cols, rows);
#else
    size_t cols_half = cols % 2 == 1 ? (cols + 1) / 2 : cols / 2;
    size_t rows_half = rows % 2 == 1 ? (rows + 1) / 2 : rows / 2;
    size_t padding = 100;
    // -------------- Y plane -------- U, V plane   ----
    size_t length = (cols + 2 * padding) * (rows + 2 * padding) +
                    (cols_half + padding) * (rows_half + padding);
    _data = new unsigned char[length];
    _data_length = length;
    memset(_data, 0, _data_length * sizeof(unsigned char));
    _cols = cols;
    _rows = rows;
#endif // USE_OPENCV
}

VideoFrame_I420::VideoFrame_I420(unsigned char * data, const size_t length,
                                 const size_t cols, const size_t rows,
                                 bool manage_data)
    : VideoFrame(manage_data)
{
    init_from_pointer(data, length, cols, rows);
}

void VideoFrame_I420::operator =(const VideoFrame_I420 & rhs)
{
    _manage_data = true;
    init_from_pointer(rhs._data, rhs._data_length,
                      rhs._cols, rhs._rows);
}

void VideoFrame_I420::init_from_pointer(
    unsigned char * data, size_t length,
    size_t cols, size_t rows)
{
    if (_manage_data and not _data)
        _data = new unsigned char[length];
    else if (_manage_data and _data_length < length)
        _data = reinterpret_cast<unsigned char *>(realloc(_data, length));

    // TODO - check length vs rows and cols?
    _data_length = length;
    _cols = cols;
    _rows = rows;
    if (_manage_data)
        memcpy(_data, data, _data_length);
    else
        _data = data;
}

}
