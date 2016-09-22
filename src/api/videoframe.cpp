#include "videoframe.h"
#include <algorithm>
#include <cstring>

namespace gg
{

VideoFrame::VideoFrame(enum ColourSpace colour, bool manage_data)
  : _colour(colour),
    _manage_data(manage_data),
    _data(nullptr),
    _data_length(0),
    _rows(0),
    _cols(0)
{

}

VideoFrame::VideoFrame(ColourSpace colour, size_t cols, size_t rows)
    : _colour(colour)
    , _manage_data(true)
    , _data(nullptr)
    , _data_length(0)
    , _cols(cols)
    , _rows(rows)
{
    size_t data_length = get_data_length();
    allocate_memory(data_length);
    set_pixels_black();
}

#ifdef USE_OPENCV
std::unique_ptr<MaskFrame> VideoFrame::compute_image_mask(int x, int y,
                                                               int width, int height)
{
    cv::Rect rect(x, y, width, height);
    cv::Mat temp(_rows, _cols, CV_8UC4, _data);
    cv::Mat image, result;
    temp.copyTo(image);
    cv::cvtColor(image, image, CV_BGRA2BGR);
    cv::Mat bg, fg;

    for (int i = 0; i < 3; ++i) {
        if (i == 0) cv::grabCut(image, result, rect, bg, fg, 1, cv::GC_INIT_WITH_RECT);
        else cv::grabCut(image, result, rect, bg, fg, 1, cv::GC_INIT_WITH_MASK);
    }

    cv::Mat result_a, result_b;
    cv::compare(result,cv::GC_FGD,result_a,cv::CMP_EQ);
    // Get the pixels marked as likely background
    cv::compare(result,cv::GC_PR_FGD,result_b,cv::CMP_EQ);
    // Final results
    result=result_a + result_b;

    cv::vector<cv::vector<cv::Point> > contours;
    cv::findContours(result, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

    if (!(contours.size() >= 1)) {
        throw std::runtime_error("Could not generate the image mask");
    }

    int index = 0;
    for (int i = 1; i < contours.size(); ++i) {
        if (contours[i].size() > contours[index].size()) index = i;
    }

    if (contours[index].size() < 5) {
        throw std::runtime_error("Could not generate the image mask");
    }

    cv::RotatedRect e = cv::fitEllipse(contours[index]);
    // Small fudge factor
    e.size.width -= 30;
    e.size.height -= 30;

    cv::Rect bound_rect = e.boundingRect();
    int rx_1, ry_1, rx_2, ry_2;
    rx_1 = bound_rect.x; ry_1 = bound_rect.y;
    rx_2 = rx_1 + bound_rect.width; ry_2 = ry_1 + bound_rect.height;

    rx_1 = std::max(0, rx_1); rx_1 = std::min((int)_cols -1, rx_1);
    ry_1 = std::max(0, ry_1); ry_1 = std::min((int)_rows -1, ry_1);
    rx_2 = std::max(0, rx_2); rx_2 = std::min((int)_cols -1, rx_2);
    ry_2 = std::max(0, ry_2); ry_2 = std::min((int)_rows -1, ry_2);

    cv::Mat filled_mask = cv::Mat::zeros(_rows, _cols, CV_8UC1);
    cv::ellipse(filled_mask, e, cv::Scalar(255), CV_FILLED);

    int mw = rx_2 - rx_1;
    int mh = ry_2 - ry_1;

    mw -= (int)mw % 16;
    mh -= (int)mh % 16;

    cv::Rect sub_rect(rx_1, ry_1, mw, mh);
    cv::Mat cropped_mask;
    filled_mask(sub_rect).copyTo(cropped_mask);
    // Now create the mask with the correct dimensions.
    auto mask = std::unique_ptr<MaskFrame>(new MaskFrame(mw, mh));
    unsigned char * mask_data = const_cast<unsigned char *>(mask->data());
    memcpy(mask_data, cropped_mask.data, mw * mh);
    mask->set_ellipse(e.center.x, e.center.y, e.size.width, e.size.height, e.angle);
    mask->set_bound_rect(rx_1, ry_1, mw, mh);

    unsigned char * mask_weights = const_cast<unsigned char *>(mask->weights());
    cv::Mat wts = cv::Mat(cropped_mask.rows, cropped_mask.cols, CV_8UC1, mask_weights);

    float w;
    double cx = mw/2.0;
    double cy = mh/2.0;

    for (double r = 0; r < cropped_mask.rows; ++r) {
        uchar * p = cropped_mask.ptr((int)r);
        uchar * q = wts.ptr((int)r);
        for (double c = 0; c < cropped_mask.cols; ++c) {
            if (p[(int)c] > 0) {
                w = std::sqrt(std::pow((cx - c)/cx, 2.f) + std::pow((cy - r)/cy, 2.f));
                if (w > 1) w = 1.f;
                q[(int)c] = (unsigned char)((1.f - w)*255.9999);
            }
        }
    }
    return mask;
}
#endif // USE_OPENCV

VideoFrame::~VideoFrame()
{
    free_memory();
}

void VideoFrame::init_from_specs(unsigned char * data, size_t data_length,
                                 size_t cols, size_t rows)
{
    if (_manage_data)
    {
        allocate_memory(data_length);
        memcpy(_data, data, _data_length * sizeof(unsigned char));
    }
    else
    {
        _data_length = data_length;
        _data = data;
    }

    _cols = cols;
    _rows = rows;
}

void VideoFrame::allocate_memory(size_t data_length)
{
    if (_manage_data and data_length > _data_length)
    {
        _data_length = data_length;
        _data = reinterpret_cast<unsigned char*>(
                    realloc(_data, _data_length * sizeof(unsigned char)));
    }
}

void VideoFrame::free_memory()
{
    if (_manage_data)
    {
        free(_data);
        _data = nullptr;
        _data_length = 0;
        _cols = 0;
        _rows = 0;
    }
}

size_t VideoFrame::get_data_length() const
{
    switch (_colour)
    {
    case BGRA:
        return _cols * _rows * 4;

    case I420:
        return (
                    (_cols + 2 * 100) * (_rows + 2 * 100) + // Y plane (100 = padding, arbitrary)
                    ((_cols % 2 == 1 ? (_cols + 1) / 2 : _cols / 2) + 100) * // U, Y plane
                    ((_rows % 2 == 1 ? (_rows + 1) / 2 : _rows / 2) + 100)  // (100 = padding, arbitrary)
               );

    default:
        throw BasicException("Colour space indicator not set properly, cannot compute memory requirement");
    }
}

void VideoFrame::set_pixels_black()
{
    memset(_data, 0, _data_length * sizeof(unsigned char));
}

}
