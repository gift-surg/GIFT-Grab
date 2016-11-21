#include "videosourcefactory.h"
#include "include_catch.h"

gg::Device device;
gg::ColourSpace colour, other_colour;

int main(int argc, char * argv[])
{
    bool args_ok = true;
    if (argc < 3)
        args_ok = false;
    else
    {
        if (strcmp(argv[1], "DVI") == 0)
            device = gg::DVI2PCIeDuo_DVI;
        else if (strcmp(argv[1], "SDI") == 0)
            device = gg::DVI2PCIeDuo_SDI;
        else
            args_ok = false;
        if (strcmp(argv[2], "BGRA") == 0)
        {
            colour = gg::BGRA;
            other_colour = gg::I420;
        }
        else if (strcmp(argv[2], "I420") == 0)
        {
            colour = gg::I420;
            other_colour = gg::BGRA;
        }
        else
            args_ok = false;
    }
    if (not args_ok)
    {
        printf("Synopsis: %s DVI|SDI BGRA|I420\n", argv[0]);
        return EXIT_FAILURE;
    }
    return Catch::Session().run();
}

TEST_CASE( "get_instance returns singleton", "[VideoSourceFactory]" )
{
    gg::VideoSourceFactory & factory = gg::VideoSourceFactory::get_instance();
    gg::VideoSourceFactory & factory_ = gg::VideoSourceFactory::get_instance();
    REQUIRE( &factory == &factory_ );
}

TEST_CASE( "get_device returns connection", "[VideoSourceFactory]" )
{
    gg::VideoSourceFactory & factory =  gg::VideoSourceFactory::get_instance();
    IVideoSource * source = nullptr;
    source = factory.get_device(device, colour);
    REQUIRE_FALSE( source == nullptr );
    REQUIRE( source->get_colour() == colour );

    gg::VideoFrame frame(colour, false);
    REQUIRE( frame.cols() == 0 );
    REQUIRE( frame.rows() == 0 );
    REQUIRE( source->get_frame(frame) );
    REQUIRE( frame.cols() > 0 );
    REQUIRE( frame.rows() > 0 );
}

TEST_CASE( "get_device returns singleton", "[VideoSourceFactory]" )
{
    gg::VideoSourceFactory & factory =  gg::VideoSourceFactory::get_instance();
    IVideoSource * source = factory.get_device(device, colour);
    IVideoSource * source_ = factory.get_device(device, colour);
    REQUIRE_FALSE( source == nullptr );
    REQUIRE( source == source_ );
}

/* The following tests are protected with these macro
 * definition checks because they involve support using
 * both colour spaces.
 */
#if defined(USE_OPENCV)                            // BGRA colour space
#if defined(USE_LIBVLC) or defined(USE_EPIPHANSDK) // I420 colour space
TEST_CASE( "get_device does not create duplicate", "[VideoSourceFactory]" )
{
    gg::VideoSourceFactory & factory = gg::VideoSourceFactory::get_instance();

    IVideoSource * source = factory.get_device(device, colour);
    IVideoSource * source_duplicate = nullptr;
    REQUIRE_THROWS_AS(
        source_duplicate = factory.get_device(device, other_colour),
        gg::DeviceAlreadyConnected
    );
    REQUIRE( source_duplicate == nullptr );
}

TEST_CASE( "free_device garbage-collects device", "[VideoSourceFactory]" )
{
    gg::VideoSourceFactory & factory = gg::VideoSourceFactory::get_instance();
    IVideoSource * source = factory.get_device(device, colour);
    REQUIRE_FALSE( source == nullptr );

    gg::VideoFrame frame(colour, false);
    REQUIRE( frame.cols() == 0 );
    REQUIRE( frame.rows() == 0 );
    REQUIRE( source->get_frame(frame) );
    REQUIRE( frame.cols() > 0 );
    REQUIRE( frame.rows() > 0 );

    factory.free_device(device);
    source = nullptr;
    source = factory.get_device(device, other_colour);
    REQUIRE_FALSE( source == nullptr );
}
#endif
#endif
