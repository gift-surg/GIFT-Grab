#include "include_catch.h"
#include "videoframe.h"
#include "videosourcefactory.h"
#include <thread>
#include <chrono>


gg::ColourSpace colour;
std::string filepath = "";
double frame_rate = -1.0;
size_t frame_count = 0;
size_t frame_width = 0;
size_t frame_height = 0;
std::chrono::milliseconds video_duration; // inferred
std::chrono::milliseconds quarter_video_duration; // inferred


class FileChecker : public gg::IObserver
{
protected:
    IVideoSource * _file_reader;
    std::vector<gg::VideoFrame> _frames;

public:
    FileChecker(IVideoSource * file_reader)
        : gg::IObserver()
        , _file_reader(file_reader)
    {
        // nop
    }

    ~FileChecker()
    {
        // nop
    }

public:
    void update(gg::VideoFrame & frame)
    {
        _frames.push_back(frame);
    }

    void attach()
    {
        if (_file_reader != nullptr)
            _file_reader->attach(*this);
    }

    void detach()
    {
        if (_file_reader != nullptr)
            _file_reader->detach(*this);
    }

    bool assert_data()
    {
        return not _frames.empty();
    }

    bool assert_colour(enum gg::ColourSpace colour)
    {
        for (gg::VideoFrame frame : _frames)
        {
            if (frame.colour() != colour)
                return false;
        }
        return true;
    }

    bool assert_frame_rate(double frame_rate)
    {
        if (_file_reader->get_frame_rate() != frame_rate)
            return false;
        else
            return true;
    }

    bool assert_frame_dimensions(size_t frame_width,
                                 size_t frame_height)
    {
        for (gg::VideoFrame frame : _frames)
        {
            if (frame.cols() != frame_width or
                frame.rows() != frame_height)
                return false;
        }
        return true;
    }

    bool assert_frame_data_lengths(enum gg::ColourSpace colour,
                                   size_t frame_width, size_t frame_height)
    {
        size_t exp_data_length;
        switch (colour)
        {
        case gg::BGRA:
            exp_data_length = frame_width * frame_height * 4;
            break;
        case gg::I420:
            exp_data_length = frame_width * frame_height * 1.5;
            break;
        case gg::UYVY:
            exp_data_length = frame_width * frame_height * 2;
            break;
        }

        for (gg::VideoFrame frame : _frames)
        {
            size_t data_length = frame.data_length();
            if (data_length != exp_data_length)
                return false;
        }
        return true;
    }
};

int main(int argc, char * argv[])
{
    bool args_ok = true;
    if (argc < 7)
        args_ok = false;
    else
    {
        if (strcmp(argv[1], "BGRA") == 0)
            colour = gg::BGRA;
        else if (strcmp(argv[1], "I420") == 0)
            colour = gg::I420;
        else if (strcmp(argv[1], "UYVY") == 0)
            colour = gg::UYVY;
        else
            args_ok = false;
        filepath = std::string(argv[2]);
        frame_rate = std::atof(argv[3]);
        if (frame_rate <= 0)
            args_ok = false;
        frame_count = std::atoi(argv[4]);
        if (frame_count <= 0)
            args_ok = false;
        frame_width = std::atoi(argv[5]);
        if (frame_width <= 0)
            args_ok = false;
        frame_height = std::atoi(argv[6]);
        if (frame_height <= 0)
            args_ok = false;
    }
    if (not args_ok)
    {
        printf("Synopsis: %s BGRA|I420|UYVY"
               " <filepath> <frame_rate> <frame_count>"
               " <frame_width> <frame_height>\n", argv[0]);
        return EXIT_FAILURE;
    }
    video_duration = std::chrono::milliseconds(
                static_cast<size_t>(1000 * frame_count / frame_rate)
                );
    quarter_video_duration = std::chrono::milliseconds(
                static_cast<size_t>(250 * frame_count / frame_rate)
                );

    int ret = Catch::Session().run();

    return ret;
}

TEST_CASE( "create_file_reader with valid file path returns"
           " ready-to-use file reader object (RAII)",
           "[VideoSourceFactory]" )
{
    gg::VideoSourceFactory & factory = gg::VideoSourceFactory::get_instance();

    IVideoSource * source = nullptr;

    source = factory.create_file_reader(filepath, colour);
    REQUIRE( source != nullptr );

    FileChecker file_checker(source);
    file_checker.attach();
    std::this_thread::sleep_for(video_duration);
    file_checker.detach();
    REQUIRE( file_checker.assert_colour(colour) );
    REQUIRE( file_checker.assert_frame_rate(frame_rate) );
    REQUIRE( file_checker.assert_frame_dimensions(frame_width, frame_height) );
    REQUIRE( file_checker.assert_data() );
    REQUIRE( file_checker.assert_frame_data_lengths(colour,
                                                    frame_width, frame_height) );

    delete source;
}

TEST_CASE( "create_file_reader returns reader that releases"
           " file on destruction", "[VideoSourceFactory]" )
{
    gg::VideoSourceFactory & factory = gg::VideoSourceFactory::get_instance();

    IVideoSource * source = nullptr;

    { // artificial scope for file_checker_1
        source = factory.create_file_reader(filepath, colour);
        REQUIRE( source != nullptr );
        FileChecker file_checker_1(source);
        file_checker_1.attach();
        std::this_thread::sleep_for(quarter_video_duration);
        file_checker_1.detach();
        REQUIRE( file_checker_1.assert_data() );
        delete source;
        source = nullptr;
    }

    source = factory.create_file_reader(filepath, colour);
    FileChecker file_checker_2(source);
    file_checker_2.attach();
    std::this_thread::sleep_for(video_duration);
    file_checker_2.detach();
    REQUIRE( file_checker_2.assert_colour(colour) );
    REQUIRE( file_checker_2.assert_frame_rate(frame_rate) );
    REQUIRE( file_checker_2.assert_frame_dimensions(frame_width, frame_height) );
    REQUIRE( file_checker_2.assert_data() );
    REQUIRE( file_checker_2.assert_frame_data_lengths(colour,
                                                      frame_width, frame_height) );

    delete source;
}

TEST_CASE( "create_file_reader with invalid path throws exception",
           "[VideoSourceFactory]" )
{
    gg::VideoSourceFactory & factory = gg::VideoSourceFactory::get_instance();

    IVideoSource * source = nullptr;
    REQUIRE_THROWS_AS(
        source = factory.create_file_reader(
            "/this/path/should/never/exist.video", colour
        ),
        gg::VideoSourceError
    );
    REQUIRE( source == nullptr );
}
