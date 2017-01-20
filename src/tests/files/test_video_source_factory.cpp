#include "include_catch.h"
#include "codec.h"
#include "videoframe.h"


gg::Codec codec;
gg::ColourSpace colour;


int main(int argc, char * argv[])
{
    bool args_ok = true;
    if (argc < 3)
        args_ok = false;
    else
    {
        if (strcmp(argv[1], "HEVC") == 0)
        {
            codec = gg::HEVC;
        }
        else if (strcmp(argv[1], "Xvid") == 0)
        {
            codec = gg::Xvid;
        }
        else if (strcmp(argv[1], "VP9") == 0)
        {
            codec = gg::VP9;
        }
        else
            args_ok = false;
        if (strcmp(argv[2], "BGRA") == 0)
            colour = gg::BGRA;
        else if (strcmp(argv[2], "I420") == 0)
            colour = gg::I420;
        else if (strcmp(argv[2], "UYVY") == 0)
            colour = gg::UYVY;
        else
            args_ok = false;
    }
    if (not args_ok)
    {
        printf("Synopsis: %s HEVC|Xvid|VP9 BGRA|I420|UYVY\n", argv[0]);
        return EXIT_FAILURE;
    }
    int ret = Catch::Session().run();

    return ret;
}

// TODO
TEST_CASE( "failing test", "[VideoSourceFactory]" )
{
    FAIL( "tests not implemented yet" );
}
