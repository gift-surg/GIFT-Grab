#include "videosourcefactory.h"
#include "include_catch.h"

gg::Device device = gg::DeckLinkSDI4K;
gg::ColourSpace colour;

int main(int argc, char * argv[])
{
    bool args_ok = true;
    if (argc < 2)
        args_ok = false;
    else
    {
        if (strcmp(argv[1], "BGRA") == 0)
            colour = gg::BGRA;
        else if (strcmp(argv[1], "UYVY") == 0)
            colour = gg::UYVY;
        else
            args_ok = false;
    }
    if (not args_ok)
    {
        printf("Synopsis: %s BGRA|UYVY\n", argv[0]);
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
