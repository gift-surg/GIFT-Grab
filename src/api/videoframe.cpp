#include "videoframe.h"
#include <algorithm>
#include <cstring>

namespace gg
{

VideoFrame::VideoFrame(bool manage_data)
  : _manage_data(manage_data),
    _data(nullptr),
    _data_length(0),
    _rows(0),
    _cols(0)
{

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
    clear();
}

void VideoFrame::clear()
{
    if (_manage_data) {
        delete [] _data;
        _data = 0;
        _data_length = 0;
        _rows = 0;
        _cols = 0;
    }
}

}

VideoFrame_BGRA::VideoFrame_BGRA(bool manage_data)
    : VideoFrame(manage_data)
{

}

VideoFrame_BGRA::VideoFrame_BGRA(const size_t rows, const size_t cols)
    : gg::VideoFrame(true)
{
#ifdef USE_OPENCV
    init_from_opencv_mat(cv::Mat::zeros(rows, cols, CV_8UC4));
#else
    size_t data_length = rows * cols * 4;
    unsigned char data[data_length];
    memset(data, 0, data_length * sizeof(unsigned char));
    init_from_pointer(data, rows, cols);
#endif // USE_OPENCV
}

#ifdef USE_OPENCV
VideoFrame_BGRA::VideoFrame_BGRA(const cv::Mat & mat, bool manage_data)
{
    _manage_data = manage_data;
    init_from_opencv_mat(mat);
}
#endif // USE_OPENCV

VideoFrame_BGRA::VideoFrame_BGRA(unsigned char * data, size_t rows, size_t cols, bool manage_data)
  : VideoFrame(manage_data)
{
    init_from_pointer(data, rows, cols);
}

VideoFrame_BGRA::VideoFrame_BGRA(const VideoFrame_BGRA & rhs)
{
    clone(rhs);
}

void VideoFrame_BGRA::operator =(const VideoFrame_BGRA & rhs)
{
    clone(rhs);
}

#ifdef USE_OPENCV
void VideoFrame_BGRA::init_from_opencv_mat(const cv::Mat &mat)
{
    assert(mat.channels() == 4);
    _rows = mat.rows;
    _cols = mat.cols;
    _data_length = _rows * _cols * 4;

    // OpenCV stores images top to bottom while OpenGL is bottom to top.
    // The caller is supposed to setup the client visualization to have
    // origin at upper left corner
    //cv::flip(mat, mat, 0);

    if (_manage_data) {
        _data = new unsigned char[_data_length];
        memcpy(_data, mat.data, _data_length * sizeof(unsigned char));
    }
    // Just copy the pointer address
    else {
        _data = mat.data;
    }
}
#endif // USE_OPENCV

void VideoFrame_BGRA::init_from_pointer(unsigned char * data, size_t rows, size_t cols)
{
    _rows = rows;
    _cols = cols;

    _data_length = _rows * _cols * 4;
    if( _manage_data ){
        _data = new unsigned char[_data_length];
        memcpy(_data, data, _data_length * sizeof(unsigned char));
    }
    else{
        _data = data;
    }
}

void VideoFrame_BGRA::clone(const VideoFrame_BGRA & rhs)
{
    clear();
    _manage_data = true;
    _rows = rhs.rows();
    _cols = rhs.cols();
    _data_length = _rows * _cols * 4;
    _data = new unsigned char[_data_length];
    memcpy(_data, rhs._data, _data_length * sizeof(unsigned char));
}
