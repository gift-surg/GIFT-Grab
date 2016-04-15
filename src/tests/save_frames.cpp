#include "factory.h"
#include <chrono>
#include <thread>

void synopsis()
{
    std::cout << "save_frames"
              << "  [  "
              << std::endl << "                "
              << " help " << " | "
              << std::endl << "                "
              << " epiphan [ xvid | h265 [ sdi | dvi ] ] " << " | "
              << std::endl << "                "
              << " file </file/path> [xvid | h265] " << " | "
              << std::endl << "                "
              << " chess xvid | h265   <width> <height> "
              << std::endl << "            "
              << " ]"
              << std::endl;
}

enum TestMode { Epiphan, File, Chessboard };

enum TestMode test_mode = TestMode::Chessboard;
enum gg::Target codec = gg::Target::File_XviD;
std::string codec_string = "xvid", filetype = "avi";
/* following numbers purposefully odd,
 * to test robustness when odd frame
 * width and/or height provided, whereas
 * H265 requires even
 */
int width = 11, height = 7, square_size = 61;
float fps = 20;
int duration = 1; // min
int num_frames = duration * 60 * fps;
int i = 0;
VideoFrame_BGRA frame;
cv::Mat image(height * square_size, width * square_size, CV_8UC4);

void parse_args(int argc, char ** argv)
{
    std::vector<std::string> args;
    for (int i = 0; i < argc; i++)
        args.push_back(std::string(argv[i]));

    if (args[1]=="help")
    {
        synopsis(); exit(0);
    }

    if (args[1]=="epiphan")
    {
        // TODO
        return;
    }

    if (args[1]=="file")
    {
        // TODO
        return;
    }

    if (args[1]=="chess")
    {
        test_mode = TestMode::Chessboard;

        if (argc < 5)
        {
            synopsis(); exit(-1);
        }

        codec_string = args[2];
        width = atoi(argv[3]); height = atoi(argv[4]);
    }

    // health checks
    if (codec_string == "xvid")
        codec = gg::Target::File_XviD;
    else if (codec_string == "h265")
    {
        codec = gg::Target::File_H265;
        filetype = "mp4";
    }
    else
    {
        std::cerr << "Codec " << codec_string << " not recognised" << std::endl;
        synopsis();
        exit(-1);
    }
}

std::string which_file()
{
    std::string filename;
    switch(test_mode)
    {case TestMode::Epiphan:
        // TODO
        break;
    case TestMode::File:
        // TODO
        break;
    case TestMode::Chessboard:
        filename.append("1min_")
                .append(codec_string)
                .append("_colour_chessboard.")
                .append(filetype);
        return filename;
    default:
        std::cerr << "Test mode not set" << std::endl;
        exit(-1);
    }
}

void get_frame(VideoFrame_BGRA & frame)
{
    // Chessboard params
    float elapsed, remaining;
    int red, green, blue, alpha, min_intensity;
    switch(test_mode)
    {
    case TestMode::Chessboard:
        elapsed = (((float) i) / num_frames);
        remaining = 1 - elapsed;
        red = 0, green = 0, blue = 0, alpha = 0;
        min_intensity = 50; // don't want black
        for (int row = 0; row < height; row++)
        {
            green = min_intensity + elapsed * (((float) row) / height) * (255 - min_intensity);
            for (int col = 0; col < width; col++)
            {
                red = min_intensity + elapsed * (((float) col) / width) * (255 - min_intensity);
                blue = min_intensity +
                       remaining *
                            sqrt( ((float)(row*row + col*col))
                                  /
                                  (height*height+width*width) ) * (255 - min_intensity);
                cv::Mat square(square_size, square_size, CV_8UC4, cv::Scalar(blue, green, red, alpha));
                square.copyTo(image(cv::Rect(col * square_size , row * square_size,
                                             square.cols, square.rows)));
            }
        }
        frame.init_from_opencv_mat(image);
        break;
    case TestMode::Epiphan:
        // TODO
        break;
    case TestMode::File:
        // TODO
        break;
    default:
        std::cerr << "Test mode not set" << std::endl;
        exit(-1);
    }
}

void time_left()
{
    if (i % 5 == 0)
        std::cout << "Saving frame " << i+1 << " of " << num_frames
                  << "\r";
}

int main(int argc, char ** argv)
{
    parse_args(argc, argv);

    try
    {
        gg::IVideoTarget * file = gg::Factory::create(codec);
        std::string filename = which_file();
        file->init(filename, fps);
        std::cout << "Saving to file " << filename << std::endl;
        for (i = 0; i < num_frames; i++)
        {
            time_left();
            get_frame(frame);
            file->append(frame);
        }
        file->finalise();
    }
    catch (gg::VideoTargetError & e)
    {
        std::cerr << e.what() << std::endl;
        return -1;
    }

    return 0;
}
