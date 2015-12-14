#include "opencv_video_source.h"
#include <string>
#include <iostream>

VideoSourceOpenCV::VideoSourceOpenCV(const char * path)
    :IVideoSource()
{
    _cap.open(path);
    if (!_cap.isOpened()) {
        std::string error = std::string("Could not open the video source: ")
                            + path;
        throw std::runtime_error(error);
    }
}

VideoSourceOpenCV::VideoSourceOpenCV(int deviceId)
    : IVideoSource()
{
    _cap.open(deviceId);
    if (!_cap.isOpened()) {
        std::string error("Device with id ");
        error.append(std::to_string(deviceId));
        error.append(" could not be opened");
        throw gg::DeviceNotFound(error);
    }
}

double VideoSourceOpenCV::get_frame_rate()
{
    if (!_cap.isOpened()) return 0;
    return _cap.get(CV_CAP_PROP_FPS);
}

bool VideoSourceOpenCV::get_frame_dimensions(int &width, int & height)
{
    width = height = 0;
    if (!_cap.isOpened()) return false;

    width = (int)_cap.get(CV_CAP_PROP_FRAME_WIDTH);
    height = (int)_cap.get(CV_CAP_PROP_FRAME_HEIGHT);

    return (width != 0 && height != 0);
}

bool VideoSourceOpenCV::get_frame(VideoFrame_BGRA & frame)
{
    if (!_cap.isOpened()) return false;

    bool has_read = _cap.read(_buffer);

    if (has_read) {
        // Initialize the BGRA buffer, if not only done so.
        if (_buffer_bgra.empty() ||
            _buffer_bgra.rows != _buffer.rows ||
            _buffer_bgra.cols != _buffer.cols) {
            _buffer_bgra = cv::Mat::zeros(_buffer.rows, _buffer.cols, CV_8UC4);
        }
        // Convert to BGRA
        cv::cvtColor(_buffer, _buffer_bgra, CV_BGR2BGRA);
        if (_get_sub_frame) {
            _buffer_bgra(_sub_frame).copyTo(_buffer_sub_bgra);
            frame.init_from_opencv_mat(_buffer_sub_bgra);
        }

        else {
            frame.init_from_opencv_mat(_buffer_bgra);
        }
    }
    return has_read;
}
