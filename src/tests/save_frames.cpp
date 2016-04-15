#include "factory.h"
#include <chrono>
#include <thread>

void synopsis()
{
    std::cout << "save_frames"
              << "  [  "
              << std::endl << "                "
              << " help "
              << std::endl << "                "
              << " epiphan   xvid | h265   sdi | dvi "
              << std::endl << "                "
              << " file   </file/path>   xvid | h265 "
              << std::endl << "                "
              << " chess   xvid | h265   <width>   <height> "
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
cv::VideoCapture cap;
enum gg::Device port = gg::Device::DVI2PCIeDuo_DVI;
std::string port_string = "dvi";
int sleep_duration = 0;
IVideoSource * epiphan = NULL;

void init(int argc, char ** argv)
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
        test_mode = TestMode::Epiphan;

        if (argc < 4)
        {
            synopsis(); exit(-1);
        }

        codec_string = args[2];
        port_string = args[3];

        if (port_string == "sdi")
            port = gg::Device::DVI2PCIeDuo_SDI;
        else if (port_string == "dvi")
            port = gg::Device::DVI2PCIeDuo_DVI;
        else
        {
            std::cerr << "Port " << port_string << " not recognised" << std::endl;
            synopsis();
            exit(-1);
        }

        epiphan = gg::Factory::connect(port);
        epiphan->set_sub_frame(600, 185, 678, 688); // optimal for Storz 27020 AA straight

        sleep_duration = (int)(1000/fps); // ms
    }

    else if (args[1]=="file")
    {
        test_mode = TestMode::File;

        if (argc < 4)
        {
            synopsis(); exit(-1);
        }

        cap.open(args[2]);
        if (not cap.isOpened())
        {
            std::cerr << "File " << args[2]
                      << " could not be opened"
                      << std::endl;
            exit(-1);
        }

        codec_string = args[3];
        num_frames = cap.get(CV_CAP_PROP_FRAME_COUNT);
        fps = cap.get(CV_CAP_PROP_FPS);
        duration = (num_frames / fps) / 60;
    }

    else if (args[1]=="chess")
    {
        test_mode = TestMode::Chessboard;

        if (argc < 5)
        {
            synopsis(); exit(-1);
        }

        codec_string = args[2];
        width = atoi(argv[3]); height = atoi(argv[4]);
    }

    else
    {
        synopsis(); exit(-1);
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
    filename.append(std::to_string(duration))
            .append("min_")
            .append(codec_string);
    switch(test_mode)
    {
    case TestMode::Epiphan:
        filename.append("_epiphan."); break;
    case TestMode::File:
        filename.append("_from_file."); break;
    case TestMode::Chessboard:
        filename.append("_colour_chessboard."); break;
    default:
        std::cerr << "Test mode not set" << std::endl;
        exit(-1);
    }
    filename.append(filetype);
    return filename;
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
        epiphan->get_frame(frame);
        break;
    case TestMode::File:
        cap >> image;
        cv::cvtColor(image, image, CV_BGR2BGRA);
        frame.init_from_opencv_mat(image);
        break;
    default:
        std::cerr << "Test mode not set" << std::endl;
        exit(-1);
    }
}

auto start = std::chrono::steady_clock::now();
float elapsed;
void time_left()
{
    if (i % 5 == 0)
        std::cout << "Saving frame " << i+1 << " of " << num_frames;

    int left;

    switch(test_mode)
    {
    case TestMode::Chessboard:
        // nop
        break;
    case TestMode::File:
    case TestMode::Epiphan:
        elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                    std::chrono::steady_clock::now() - start).count();
        left = ((float) num_frames - i + 1) * elapsed / (i+1);
        std::cout << " (" << left << " sec. left)";
        break;
    default:
        std::cerr << "Test mode not set" << std::endl;
        exit(-1);
    }

    std::cout << "\r";
}

void wait_for_next()
{
    switch(test_mode)
    {
    case TestMode::File:
    case TestMode::Chessboard:
        // nop
        break;
    case TestMode::Epiphan:
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_duration));
        break;
    default:
        std::cerr << "Test mode not set" << std::endl;
        exit(-1);
    }
}

void finalise()
{
    switch(test_mode)
    {
    case TestMode::File:
    case TestMode::Chessboard:
        // nop
        break;
    case TestMode::Epiphan:
        gg::Factory::disconnect(port);
        break;
    default:
        std::cerr << "Test mode not set" << std::endl;
        exit(-1);
    }
}

int main(int argc, char ** argv)
{
    try
    {
        init(argc, argv);

        gg::IVideoTarget * file = gg::Factory::create(codec);
        std::string filename = which_file();
        file->init(filename, fps);
        std::cout << "Saving to file " << filename << std::endl;

        start = std::chrono::steady_clock::now();
        for (i = 0; i < num_frames; i++)
        {
            time_left();
            get_frame(frame);
            file->append(frame);
            wait_for_next();
        }

        file->finalise();

        finalise();
    }
    catch (gg::VideoTargetError & e)
    {
        std::cerr << e.what() << std::endl;
        return -1;
    }
    catch (gg::DeviceOffline & e)
    {
        std::cerr << e.what() << std::endl;
        return -1;
    }

    return 0;
}
