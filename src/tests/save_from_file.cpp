#include "factory.h"
#include <chrono>
#include <thread>

void synopsis()
{
    std::cout << "save_1min_epiphan"
              << "    [ help | "
              << "    <file>    "
              << "    [ xvid | h265 ]"
              << " ]"
              << std::endl;
}

int main(int argc, char ** argv)
{
    enum gg::Target codec = gg::Target::File_XviD;
    std::string codec_string = "xvid";
    std::string filename = "";
    cv::VideoCapture cap;

    if (argc >= 2)
    {
        if (std::string(argv[1]) == "help")
        {
            synopsis();
            return 0;
        }
        else
        {
            filename = std::string(argv[1]);
            cap.open(filename);
            if (not cap.isOpened())
            {
                std::cerr << "File " << filename
                          << " could not be opened"
                          << std::endl;
                exit(-1);
            }

            if (argc >= 3)
            {
                codec_string = std::string(argv[2]);
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
            }
        }
    }

    float fps = cap.get(CV_CAP_PROP_FPS);
    int num_frames = cap.get(CV_CAP_PROP_FRAME_COUNT);
    int duration = (num_frames / fps) / 60;
    VideoFrame_BGRA frame;
    cv::Mat image;

    try
    {
        gg::IVideoTarget * file = gg::Factory::create(codec);
        std::string filename;
        filename.append(std::to_string(duration))
                .append("min_")
                .append(codec_string)
                .append("_from_file")
                .append(".avi");
        std::cout << "Saving to file " << filename << std::endl;
        file->init(filename, fps);

        auto start = std::chrono::steady_clock::now(); //use auto keyword to minimize typing strokes :)

        num_frames = cap.get(CV_CAP_PROP_FRAME_COUNT);
        for (int i = 0; i < num_frames; i++)
        {
            cap >> image;
            cv::cvtColor(image, image, CV_BGR2BGRA);
            if (i % 5 == 0)
            {
                float elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                            std::chrono::steady_clock::now() - start).count();
                int left = ((float) num_frames - i + 1) * elapsed / (i+1);
                std::cout << "Saving frame " << i+1 << " of " << num_frames
                          << " (" << left << " sec. left)"
                          << "\r";
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
