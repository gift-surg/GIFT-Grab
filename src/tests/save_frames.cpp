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
              << " epiphan   xvid | h265   sdi | dvi   "
              << " [ <framerate>   [ <x> <y>  <width> <height> ]   ]"
              << " # optimal: 20 600 185 678 688"
              << std::endl << "                "
              << " file   </file/path>   xvid | h265 "
              << std::endl << "                "
              << " chess   xvid | h265   <width>   <height> "
              << std::endl << "            "
              << " ]"
              << std::endl;
}

enum TestMode { Epiphan, File, Chessboard };

enum TestMode test_mode;
enum gg::Target codec;
std::string codec_string, filetype;
int width, height, square_size;
float fps;
int duration; // min
int num_frames;
int i = 0;
VideoFrame_BGRA frame;
cv::Mat image;
cv::VideoCapture cap;
enum gg::Device port;
std::string port_string;
int sleep_duration;
IVideoSource * epiphan = NULL;
int roi_x, roi_y;

inline
void timer_start(std::chrono::high_resolution_clock::time_point & start)
{
    start = std::chrono::high_resolution_clock::now();
}

inline
float timer_end(const std::chrono::high_resolution_clock::time_point & start)
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(
                            std::chrono::high_resolution_clock::now() - start
                                                                ).count();
}

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
        fps = 60;
        duration = 1; // min

        // optimal for Storz 27020 AA straight
        if (argc >= 5)
        {
            fps = atof(argv[4]);
            if (argc >= 9)
            {
                roi_x = atoi(argv[5]);
                roi_y = atoi(argv[6]);
                width = atoi(argv[7]);
                height = atoi(argv[8]);
                epiphan->set_sub_frame(roi_x, roi_y, width, height);
            }
        }

        num_frames = duration * 60 * fps;
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
        /* square size purposefully odd,
         * to test robustness when odd frame
         * width and/or height provided, whereas
         * H265 requires even
         */
        square_size = 61;
        image = cv::Mat(height * square_size, width * square_size, CV_8UC4);

        fps = 60;
        duration = 1; // min
        num_frames = duration * 60 * fps;
    }

    else
    {
        synopsis(); exit(-1);
    }

    // health checks
    if (codec_string == "xvid")
    {
        codec = gg::Target::File_XviD;
        filetype = "avi";
    }
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
            .append("min_");
    switch(test_mode)
    {
    case TestMode::Epiphan:
        filename.append("epiphan"); break;
    case TestMode::File:
        filename.append("from_file"); break;
    case TestMode::Chessboard:
        filename.append("colour_chessboard"); break;
    default:
        std::cerr << "Test mode not set" << std::endl;
        exit(-1);
    }
    filename.append("_")
            .append(codec_string)
            .append(".")
            .append(filetype);
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

auto start = std::chrono::high_resolution_clock::now();
float elapsed;
void time_left()
{
    if (i % 5 == 0)
    {
        std::cerr << "Saving frame " << i+1 << " of " << num_frames;
        elapsed = timer_end(start);
        int left = ((float) num_frames - i + 1) * elapsed / (i+1);
        std::cerr << " (" << left << " sec. left)" << "\r";
    }
}

void wait_for_next(const float elapsed = 0)
{
    float real_sleep_duration;
    switch(test_mode)
    {
    case TestMode::File:
    case TestMode::Chessboard:
        // nop
        break;
    case TestMode::Epiphan:
        real_sleep_duration = sleep_duration - elapsed;
        if (real_sleep_duration < 0)
            std::cerr << std::endl
                      << "Elapsed duration longer "
                      << "than sleep duration by "
                      << real_sleep_duration
                      << " msec."
                      << std::endl;
        else
            std::this_thread::sleep_for(
                std::chrono::milliseconds(
                    (int)real_sleep_duration ));
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

        timer_start(start);
        std::chrono::high_resolution_clock::time_point current_start, analysis;
        for (i = 0; i < num_frames; i++)
        {
            time_left();
            timer_start(current_start);
            get_frame(frame);
            std::cout << "Frame:\t"
                      << timer_end(current_start) << "\tmsec." << "\t";
            timer_start(analysis);
            file->append(frame);
            std::cout << "Write:\t"
                      << timer_end(analysis) << "\tmsec." << std::endl;

            float current_elapsed = timer_end(current_start);
            wait_for_next(current_elapsed);
        }
        auto total = timer_end(start);
        std::cout << std::endl << "Total time was " << total << " msec." << std::endl;

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
