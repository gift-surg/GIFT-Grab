#include "videosourcefactory.h"
#include "include_catch.h"
#include <thread>
#include <chrono>

std::string address;
gg::ColourSpace colour, other_colour;
std::chrono::duration<float, std::milli> network_source_delay;  // sec

int main(int argc, char * argv[])
{
    bool args_ok = true;
    if (argc < 3)
        args_ok = false;
    else
    {
        address = std::string(argv[1]);
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
        float network_source_delay_ = 0;
        if (argc >= 4)
            network_source_delay_ = 1000 * atof(argv[3]);
        network_source_delay = std::chrono::duration<float, std::milli>(network_source_delay_);
    }
    if (not args_ok)
    {
        printf("Synopsis: %s <network_source_address> BGRA|I420 [ <network_source_delay> ]\n", argv[0]);
        return EXIT_FAILURE;
    }
    return Catch::Session().run();
}

TEST_CASE( "connect_network_source returns connection", "[VideoSourceFactory]" )
{
    gg::VideoSourceFactory & factory =  gg::VideoSourceFactory::get_instance();
    IVideoSource * source = nullptr;
    source = factory.connect_network_source(address, colour);
    std::this_thread::sleep_for(network_source_delay);
    REQUIRE_FALSE( source == nullptr );
    REQUIRE( source->get_colour() == colour );

    gg::VideoFrame frame(colour, false);
    REQUIRE( frame.cols() == 0 );
    REQUIRE( frame.rows() == 0 );
    REQUIRE( source->get_frame(frame) );
    REQUIRE( frame.cols() > 0 );
    REQUIRE( frame.rows() > 0 );

    delete source;
}

TEST_CASE( "Valid but uavailable network source address throws exception", "[VideoSourceFactory]" )
{
    gg::VideoSourceFactory & factory =  gg::VideoSourceFactory::get_instance();
    IVideoSource * source = nullptr;
    REQUIRE_THROWS_AS(
        source = factory.connect_network_source("rtsp://localhost:554/stream.sdp", colour),
        gg::NetworkSourceUnavailable
    );
    REQUIRE( source == nullptr );

    delete source;
}

TEST_CASE( "Invalid network source address throws exception", "[VideoSourceFactory]" )
{
    gg::VideoSourceFactory & factory =  gg::VideoSourceFactory::get_instance();
    IVideoSource * source = nullptr;
    REQUIRE_THROWS_AS(
        source = factory.connect_network_source("invalid://invalid-address", colour),
        gg::NetworkSourceUnavailable
    );
    REQUIRE( source == nullptr );

    delete source;
}
