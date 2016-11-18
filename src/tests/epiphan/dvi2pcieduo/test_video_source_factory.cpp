#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file

#include "videosourcefactory.h"
#include "catch.hpp"

// TODO Parametrize device and colour
gg::Device device = gg::DVI2PCIeDuo_DVI;
gg::ColourSpace colour = gg::I420, other_colour = gg::BGRA;

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
