#include "videosourcefactory.h"
#include "include_catch.h"

gg::Device device = gg::DeckLinkSDI4K;
gg::ColourSpace colour, other_colour;

int main(int argc, char * argv[])
{
    bool args_ok = true;
    if (argc < 2)
        args_ok = false;
    else
    {
        if (strcmp(argv[1], "BGRA") == 0)
        {
            colour = gg::BGRA;
            other_colour = gg::I420;
        }
        else if (strcmp(argv[1], "I420") == 0)
        {
            colour = gg::I420;
            other_colour = gg::BGRA;
        }
        else
            args_ok = false;
    }
    if (not args_ok)
    {
        printf("Synopsis: %s BGRA|I420\n", argv[0]);
        return EXIT_FAILURE;
    }
    return Catch::Session().run();
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
