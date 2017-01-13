#include "videosourcefactory.h"
#include "include_catch.h"

gg::Device device = gg::DeckLinkSDI4K;
gg::ColourSpace colour = gg::UYVY;

int main()
{
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
    REQUIRE_FALSE( source == factory.get_device(device, colour) );
}
