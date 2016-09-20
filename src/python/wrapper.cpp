#include "factory.h"
#include "except.h"
#ifdef USE_OPENCV
#include "opencv_video_source.h"
#include "opencv_video_target.h"
#endif // USE_OPENCV
#ifdef USE_I420
#ifdef USE_EPIPHANSDK
#include "epiphansdk_video_source.h"
#endif
#ifdef USE_LIBVLC
#include "vlc_video_source.h"
#endif
#endif
#ifdef USE_FFMPEG
#include "ffmpeg_video_target.h"
#endif
#include <boost/python.hpp>
#include <boost/python/exception_translator.hpp>

using namespace boost::python;

class IVideoSourceWrapper : IVideoSource, wrapper<IVideoSource>
{
    bool get_frame_dimensions(int & width, int & height)
    {
        return this->get_override("get_frame_dimensions")(width, height);
    }

    bool get_frame(VideoFrame_BGRA & frame)
    {
        return this->get_override("get_frame")(frame);
    }

#ifdef USE_I420
    bool get_frame(gg::VideoFrame_I420 & frame)
    {
        return this->get_override("get_frame")(frame);
    }
#endif

    double get_frame_rate()
    {
        return this->get_override("get_frame_rate")();
    }

    void set_sub_frame(int x, int y, int width, int height)
    {
        this->get_override("set_sub_frame")(x, y, width, height);
    }

    void get_full_frame()
    {
        this->get_override("get_full_frame")();
    }
};

class IVideoTargetWrapper : gg::IVideoTarget, wrapper<gg::IVideoTarget>
{
    void init(const std::string filepath, const float framerate)
    {
        this->get_override("init")(filepath, framerate);
    }

    void append(const VideoFrame_BGRA & frame)
    {
        this->get_override("append")(frame);
    }

    void finalise()
    {
        this->get_override("finalise")();
    }
};

void translate_VideoSourceError(gg::VideoSourceError const & e)
{
    std::string msg;
    msg.append("VideoSourceError: ").append(e.what());
    PyErr_SetString(PyExc_RuntimeError, msg.c_str());
}

void translate_DeviceNotFound(gg::DeviceNotFound const & e)
{
    std::string msg;
    msg.append("DeviceNotFound: ").append(e.what());
    PyErr_SetString(PyExc_IOError, msg.c_str());
}

void translate_DeviceOffline(gg::DeviceOffline const & e)
{
    std::string msg;
    msg.append("DeviceOffline: ").append(e.what());
    PyErr_SetString(PyExc_IOError, msg.c_str());
}

void translate_VideoTargetError(gg::VideoTargetError const & e)
{
    std::string msg;
    msg.append("VideoTargetError: ").append(e.what());
    PyErr_SetString(PyExc_RuntimeError, msg.c_str());
}

BOOST_PYTHON_MODULE(pygiftgrab)
{
    register_exception_translator<gg::VideoSourceError>(&translate_VideoSourceError);
    register_exception_translator<gg::DeviceNotFound>(&translate_DeviceNotFound);
    register_exception_translator<gg::DeviceOffline>(&translate_DeviceOffline);
    register_exception_translator<gg::VideoTargetError>(&translate_VideoTargetError);

    enum_<gg::ColourSpace>("ColourSpace")
        .value("BGRA", gg::ColourSpace::BGRA)
        .value("I420", gg::ColourSpace::I420)
    ;

    enum_<gg::Device>("Device")
        .value("DVI2PCIeDuo_SDI", gg::Device::DVI2PCIeDuo_SDI)
        .value("DVI2PCIeDuo_DVI", gg::Device::DVI2PCIeDuo_DVI)
    ;

    enum_<gg::Storage>("Storage")
        .value("File_HEVC", gg::Storage::File_HEVC)
        .value("File_XviD", gg::Storage::File_XviD)
        .value("File_VP9", gg::Storage::File_VP9)
    ;

    class_<gg::VideoFrame>("VideoFrame", init<bool>())
        .def("rows", &gg::VideoFrame::rows)
        .def("cols", &gg::VideoFrame::cols)
    ;

    class_<VideoFrame_BGRA>("VideoFrame_BGRA", init<bool>())
        .def(init<const size_t, const size_t>())
        .def("rows", &VideoFrame_BGRA::rows)
        .def("cols", &VideoFrame_BGRA::cols)
    ;

    class_<IVideoSource, boost::noncopyable>("IVideoSource", no_init)
    ;

#ifdef USE_OPENCV
    bool (VideoSourceOpenCV::*opencv_get_frame_bgra)(VideoFrame_BGRA &) = &VideoSourceOpenCV::get_frame;
    class_<VideoSourceOpenCV, bases<IVideoSource>, boost::noncopyable>(
                "VideoSourceOpenCV", init<int>())
        .def(init<char *>())
        .def("get_frame", opencv_get_frame_bgra)
        .def("get_frame_dimensions", &VideoSourceOpenCV::get_frame_dimensions)
        .def("get_frame_rate", &VideoSourceOpenCV::get_frame_rate)
        .def("set_sub_frame", &VideoSourceOpenCV::set_sub_frame)
        .def("get_full_frame", &VideoSourceOpenCV::get_full_frame)
    ;
#endif // USE_OPENCV

#ifdef USE_I420
    class_<gg::VideoFrame_I420>("VideoFrame_I420", init<bool>())
        .def(init<const size_t, const size_t>())
        .def("rows", &gg::VideoFrame_I420::rows)
        .def("cols", &gg::VideoFrame_I420::cols)
    ;
#ifdef USE_EPIPHANSDK
    bool (gg::VideoSourceEpiphanSDK::*epiphansdk_get_frame_i420)(gg::VideoFrame_I420 &) = &gg::VideoSourceEpiphanSDK::get_frame;
    class_<gg::VideoSourceEpiphanSDK, bases<IVideoSource>, boost::noncopyable>(
                "VideoSourceEpiphanSDK", init<const std::string, const V2U_INT32>())
        .def("get_frame", epiphansdk_get_frame_i420)
        .def("get_frame_dimensions", &gg::VideoSourceEpiphanSDK::get_frame_dimensions)
        .def("get_frame_rate", &gg::VideoSourceEpiphanSDK::get_frame_rate)
        .def("set_sub_frame", &gg::VideoSourceEpiphanSDK::set_sub_frame)
        .def("get_full_frame", &gg::VideoSourceEpiphanSDK::get_full_frame)
    ;
#endif

#ifdef USE_LIBVLC
    bool (gg::VideoSourceVLC::*vlc_get_frame_i420)(gg::VideoFrame_I420 &) = &gg::VideoSourceVLC::get_frame;
    class_<gg::VideoSourceVLC, bases<IVideoSource>, boost::noncopyable>(
                "VideoSourceVLC", init<const std::string>())
        .def("get_frame", vlc_get_frame_i420)
        .def("get_frame_dimensions", &gg::VideoSourceVLC::get_frame_dimensions)
        .def("get_frame_rate", &gg::VideoSourceVLC::get_frame_rate)
        .def("set_sub_frame", &gg::VideoSourceVLC::set_sub_frame)
        .def("get_full_frame", &gg::VideoSourceVLC::get_full_frame)
    ;
#endif
#endif

    class_<gg::IVideoTarget, boost::noncopyable>("IVideoTarget", no_init)
    ;

#ifdef USE_FFMPEG
    void (gg::VideoTargetFFmpeg::*ffmpeg_append_bgra)(const VideoFrame_BGRA &) = &gg::VideoTargetFFmpeg::append;
#ifdef USE_I420
    void (gg::VideoTargetFFmpeg::*ffmpeg_append_i420)(const gg::VideoFrame_I420 &) = &gg::VideoTargetFFmpeg::append;
#endif

    class_<gg::VideoTargetFFmpeg, bases<gg::IVideoTarget>, boost::noncopyable>(
                "VideoTargetFFmpeg", init<std::string>())
        .def("init", &gg::VideoTargetFFmpeg::init)
        .def("append", ffmpeg_append_bgra)
#ifdef USE_I420
        .def("append", ffmpeg_append_i420)
#endif
        .def("finalise", &gg::VideoTargetFFmpeg::finalise)
    ;
#endif

#ifdef USE_OPENCV
    class_<gg::VideoTargetOpenCV, bases<gg::IVideoTarget>, boost::noncopyable>(
                "VideoTargetOpenCV", init<std::string>())
        .def("init", &gg::VideoTargetOpenCV::init)
        .def("append", &gg::VideoTargetOpenCV::append)
        .def("finalise", &gg::VideoTargetOpenCV::finalise)
    ;
#endif // USE_OPENCV

    class_<gg::Factory>("Factory", no_init)
        .def("connect", &gg::Factory::connect,
             /* because client should never delete returned
              * object on its own, but should rather call
              * disconnect when done
              */
             return_value_policy<reference_existing_object>())
        .staticmethod("connect")
        .def("disconnect", &gg::Factory::disconnect)
        .staticmethod("disconnect")
        .def("writer", &gg::Factory::writer,
             // because ownership is passed to client
             return_value_policy<manage_new_object>())
        .staticmethod("writer")
    ;
}
