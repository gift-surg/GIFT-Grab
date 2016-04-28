#include "factory.h"
#include <opencv_video_source.h>
#include <boost/python.hpp>

using namespace boost::python;

void foo()
{
    std::cout << "This should print with a dot at the end." << std::endl;
}

class IVideoSourceWrap : IVideoSource, wrapper<IVideoSource>
{
    bool get_frame_dimensions(int & width, int & height)
    {
        return this->get_override("get_frame_dimensions")(width, height);
    }

    bool get_frame(VideoFrame_BGRA & frame)
    {
        return this->get_override("get_frame")(frame);
    }

    double get_frame_rate()
    {
        return this->get_override("get_frame_rate")();
    }

    void set_sub_frame(int x, int y, int width, int height)
    {
        this->get_override("set_sub_frame")(x, y, width, height);
    }
};

BOOST_PYTHON_MODULE(pygiftgrab)
{
    def( "foo", foo );

    enum_<gg::Device>("Device")
        .value("DVI2PCIeDuo_SDI", gg::Device::DVI2PCIeDuo_SDI)
        .value("DVI2PCIeDuo_DVI", gg::Device::DVI2PCIeDuo_DVI)
    ;

    class_<IVideoSource, boost::noncopyable>("IVideoSource", no_init)
        .def("get_frame_dimensions", pure_virtual(&IVideoSource::get_frame_dimensions))
        .def("get_frame", pure_virtual(&IVideoSource::get_frame))
        .def("get_frame_rate", pure_virtual(&IVideoSource::get_frame_rate))
        .def("set_sub_frame", pure_virtual(&IVideoSource::set_sub_frame))
    ;

    class_<VideoSourceOpenCV, bases<IVideoSource>, boost::noncopyable>("VideoSourceOpenCV", init<int>())
        .def(init<char *>())
        .def("get_frame", &VideoSourceOpenCV::get_frame)
        .def("get_frame_dimensions", &VideoSourceOpenCV::get_frame_dimensions)
        .def("get_frame_rate", &VideoSourceOpenCV::get_frame_rate)
        .def("set_sub_frame", &VideoSourceOpenCV::set_sub_frame)
    ;
}
