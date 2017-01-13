#include "include_catch.h"
#include "videotargetfactory.h"
#include "videoframe.h"
#include <cstdio>
#include <vector>


gg::Codec codec;
std::string extension;
float frame_rate = 30.0;
gg::ColourSpace colour;
std::vector<std::string> filenames;


//!
//! \brief Generate a random filename
//! in current directory, including the
//! proper extension
//! \return
//!
std::string generate_random_filename()
{
    // get formatted filepath-safe timestamp
    timeval time_now;
    gettimeofday(&time_now, NULL);
    int milli = time_now.tv_usec / 1000;
    char buffer[80];
    strftime(buffer, 80, "%Y-%m-%d-%H-%M-%S", localtime(&time_now.tv_sec));
    char timestamp[84] = "";
    sprintf(timestamp, "%s-%03d", buffer, milli);

    // append timestamp to filename
    std::string filename = "./test_video_target_factory_";
    filename.append(std::string(timestamp));
    filename.append(extension);

    filenames.push_back(filename);

    return filename;
}


//!
//! \brief Check whether a file exists
//! with the given \c filename
//! \param filename
//! \return
//!
bool file_exists(std::string filename)
{
    if (FILE * file = fopen(filename.c_str(), "r"))
    {
        fclose(file);
        return true;
    }
    else
        return false;
}


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
            extension = ".mp4";
        }
        else if (strcmp(argv[1], "Xvid") == 0)
        {
            codec = gg::Xvid;
            extension = ".avi";
        }
        else if (strcmp(argv[1], "VP9") == 0)
        {
            codec = gg::VP9;
            extension = ".webm";
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
    for (std::string filename : filenames)
        std::remove(filename.c_str());
    return ret;
}

TEST_CASE( "get_instance returns singleton", "[VideoTargetFactory]" )
{
    gg::VideoTargetFactory & factory = gg::VideoTargetFactory::get_instance();
    gg::VideoTargetFactory & factory_ = gg::VideoTargetFactory::get_instance();
    REQUIRE( &factory == &factory_ );
}

TEST_CASE( "create_file_writer with valid file path returns file writer",
           "[VideoTargetFactory]")
{
    gg::VideoTargetFactory & factory = gg::VideoTargetFactory::get_instance();
    gg::IVideoTarget * target = nullptr;
    std::string filename = generate_random_filename();
    target = factory.create_file_writer(codec, filename, frame_rate);
    REQUIRE_FALSE( target == nullptr );
    delete target;
}

TEST_CASE( "create_file_writer with invalid path throws exception",
           "[VideoTargetFactory]")
{
    gg::VideoTargetFactory & factory = gg::VideoTargetFactory::get_instance();
    gg::IVideoTarget * target = nullptr;
    REQUIRE_THROWS_AS(
        factory.create_file_writer(codec, "/this/file/should/never/exist.extension", frame_rate),
        gg::VideoTargetError
    );
    REQUIRE( target == nullptr );
}

TEST_CASE( "create_file_writer with invalid extension throws exception",
           "[VideoTargetFactory]" )
{
    gg::VideoTargetFactory & factory = gg::VideoTargetFactory::get_instance();
    gg::IVideoTarget * target = nullptr;
    std::string filename = generate_random_filename();
    filename.append(".invalidextension");
    REQUIRE_THROWS_AS(
        factory.create_file_writer(codec, filename, frame_rate),
        gg::VideoTargetError
    );
    REQUIRE( target == nullptr );
}

TEST_CASE( "create_file_writer with invalid frame rate throws exception",
           "[VideoTargetFactory]" )
{
    gg::VideoTargetFactory & factory = gg::VideoTargetFactory::get_instance();
    gg::IVideoTarget * target = nullptr;
    std::string filename = generate_random_filename();
    REQUIRE_THROWS_AS(
        factory.create_file_writer(codec, filename, -frame_rate),
        gg::VideoTargetError
    );
    REQUIRE( target == nullptr );
    REQUIRE_THROWS_AS(
        factory.create_file_writer(codec, filename, 0.0),
        gg::VideoTargetError
    );
    REQUIRE( target == nullptr );
}

TEST_CASE( "create_file_writer returns ready-to-use file writer object (RAII)",
           "[VideoSourceFactory]")
{
    gg::VideoTargetFactory & factory = gg::VideoTargetFactory::get_instance();
    gg::IVideoTarget * target = nullptr;
    std::string filename = generate_random_filename();
    REQUIRE_FALSE( file_exists(filename) );
    target = factory.create_file_writer(codec, filename, frame_rate);
    gg::VideoFrame frame(colour, 1920, 1080);
    target->append(frame);
    delete target;
    REQUIRE( file_exists(filename) );
}

