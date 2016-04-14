#include "factory.h"
#include <chrono>
#include <thread>

void synopsis()
{
    std::cout << "save_1min_epiphan"
              << "    [ help | "
              << "    [ xvid | h265 ]"
              << "    [ <width> <height> ]"
              << " ]"
              << std::endl;
}

int main(int argc, char ** argv)
{
    enum gg::Target codec = gg::Target::File_XviD;
    std::string codec_string = "xvid";
    int width = 10, height = 8, square_size = 60;

    if (argc >= 2)
    {
        if (std::string(argv[1]) == "help")
        {
            synopsis();
            return 0;
        }
        else
        {
            codec_string = std::string(argv[1]);
            if (codec_string == "xvid")
                codec = gg::Target::File_XviD;
            else if (codec_string == "h265")
                codec = gg::Target::File_H265;
            else
            {
                std::cerr << "Codec " << codec_string << " not recognised" << std::endl;
                synopsis();
                exit(-1);
            }

            if (argc == 3)
            {
                synopsis();
                exit(-1);
            }
            else if (argc >= 4)
            {
                width = atoi(argv[2]); height = atoi(argv[3]);
            }
        }
    }

    float fps = 20;
    int duration = 1; // min
    int num_frames = duration * 60 * fps;
    VideoFrame_BGRA frame;
    cv::Mat image(height * square_size, width * square_size, CV_8UC4);

    try
    {
        gg::IVideoTarget * file = gg::Factory::create(codec);
        std::string filename;
        filename.append("1min_")
                .append(codec_string)
                .append("_colour_chessboard")
                .append(".mp4");
        std::cout << "Saving to file " << filename << std::endl;
        file->init(filename, fps);
        for (int i = 0; i < num_frames; i++)
        {
            if (i % 5 == 0)
                std::cout << "Saving frame " << i+1 << " of " << num_frames
                          << "\r";
            float elapsed = (((float) i) / num_frames);
            float remaining = 1 - elapsed;
            int red = 0,
                green = 0,
                blue = 0,
                alpha = 0;
            int min_intensity = 50; // don't want black
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
