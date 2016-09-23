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
