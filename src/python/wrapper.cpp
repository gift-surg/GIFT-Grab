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
}
