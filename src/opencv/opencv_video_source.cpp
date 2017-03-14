#include "opencv_video_source.h"
#include <string>
#include <iostream>
#include <cmath>

VideoSourceOpenCV::VideoSourceOpenCV(const char * path)
    : IVideoSource(gg::BGRA)
    , _frame_rate(0)
    , _daemon(nullptr)
{
    _cap.open(path);
    if (!_cap.isOpened())
    {
        std::string error = std::string("Could not open the video source: ")
                            + path;
        throw gg::VideoSourceError(error);
    }
    _daemon = new gg::BroadcastDaemon(this);
    _daemon->start(get_frame_rate());
}

VideoSourceOpenCV::VideoSourceOpenCV(int deviceId)
    : IVideoSource(gg::BGRA)
    , _frame_rate(0)
{
    _cap.open(deviceId);
    if (!_cap.isOpened()) {
        std::string error("Device with id ");
        error.append(std::to_string(deviceId));
        error.append(" could not be opened");
        throw gg::DeviceNotFound(error);
    }
    _daemon = new gg::BroadcastDaemon(this);
    _daemon->start(get_frame_rate());
}

VideoSourceOpenCV::~VideoSourceOpenCV()
{
    delete _daemon;
}

double VideoSourceOpenCV::get_frame_rate()
{
    if (!_cap.isOpened())
    {
        _frame_rate = 0;
        return _frame_rate;
    }

    if (_frame_rate > 0) // already obtained or estimated
        return _frame_rate;

    // will work for files, but sometimes <= 0 or nan with online devices
    _frame_rate = _cap.get(CV_CAP_PROP_FPS);
    if (std::isnan(_frame_rate))
        _frame_rate = 0;

    if (_frame_rate <= 0) // i.e. not a file, see:
    // http://www.learnopencv.com/how-to-find-frame-rate-or-frames-per-second-fps-in-opencv-python-cpp/
    {
        // Number of frames to capture
        int num_frames = 120;

        // Start and end times
        time_t start, end;

        gg::VideoFrame buffer(gg::ColourSpace::BGRA, true);

        // Start time
        time(&start);

        // Grab a few frames
        for(int i = 0; i < num_frames; i++)
        {
            get_frame(buffer);
        }

        // End Time
        time(&end);

        // Time elapsed
        float seconds = difftime(end, start);

        // Calculate frames per second
        _frame_rate  = num_frames / seconds;
    }

    // to make sure e.g. -1 is not returned,
    // which could lead to bugs in computations
    if (_frame_rate < 0) _frame_rate = 0;

    return _frame_rate;
}

bool VideoSourceOpenCV::get_frame_dimensions(int &width, int & height)
{
    width = height = 0;
    if (!_cap.isOpened()) return false;

    width = (int)_cap.get(CV_CAP_PROP_FRAME_WIDTH);
    height = (int)_cap.get(CV_CAP_PROP_FRAME_HEIGHT);

    return (width != 0 && height != 0);
}

bool VideoSourceOpenCV::get_frame(gg::VideoFrame & frame)
{
    std::lock_guard<std::mutex> buffer_lock_guard(_buffer_lock);

    if (frame.colour() != _colour)
        return false;

    if (!_cap.isOpened()) return false;

    bool has_read = _cap.read(_buffer);

    if (has_read) {
        // Initialize the BGRA buffer, only if not already done so.
        if (_buffer_bgra.empty() ||
            _buffer_bgra.rows != _buffer.rows ||
            _buffer_bgra.cols != _buffer.cols) {
            _buffer_bgra = cv::Mat::zeros(_buffer.rows, _buffer.cols, CV_8UC4);
        }
        // Convert to BGRA
        cv::cvtColor(_buffer, _buffer_bgra, CV_BGR2BGRA);
        unsigned char * data = nullptr;
        size_t data_length = 0;
        size_t cols = 0, rows = 0;
        if (_get_sub_frame) {
            _buffer_bgra(_sub_frame).copyTo(_buffer_sub_bgra);
            data = _buffer_sub_bgra.data;
            cols = _buffer_sub_bgra.cols;
            rows = _buffer_sub_bgra.rows;
            data_length = _buffer_sub_bgra.channels() * cols * rows;
        }

        else {
            data = _buffer_bgra.data;
            cols = _buffer_bgra.cols;
            rows = _buffer_bgra.rows;
            data_length = _buffer_bgra.channels() * cols * rows;
        }

        frame.init_from_specs(data, data_length, cols, rows);
    }
    return has_read;
}

