#include "factory.h"
#include <boost/python.hpp>

void foo()
{
    std::cout << "This should print with a dot at the end." << std::endl;
}

BOOST_PYTHON_MODULE(pygiftgrab)
{
    using namespace boost::python;
    def( "foo", foo );

    enum_<gg::Device>("gg_Device")
        .value("DVI2PCIeDuo_SDI", gg::Device::DVI2PCIeDuo_SDI)
        .value("DVI2PCIeDuo_DVI", gg::Device::DVI2PCIeDuo_DVI)
    ;

    class_<IVideoSource, boost::noncopyable>("IVideoSource", no_init)
    ;
}
