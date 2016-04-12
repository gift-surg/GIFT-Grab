#include "factory.h"
#include <chrono>
#include <thread>

int main(int argc, char ** argv)
{
    enum gg::Device port = gg::Device::DVI2PCIeDuo_DVI;
    enum gg::Target codec = gg::Target::File_XviD;
    std::string port_string = "dvi";
    std::string codec_string = "xvid";

    if (argc >= 2)
    {
        codec_string = std::string(argv[1]);
        if (codec_string == "xvid")
            codec = gg::Target::File_XviD;
        else if (codec_string == "h265")
            codec = gg::Target::File_H265;
        else
        {
            std::cerr << "Codec " << codec_string << " not recognised" << std::endl;
            exit(-1);
        }

        if (argc >= 3)
        {
            port_string = std::string(argv[2]);
            if (port_string == "sdi")
                port = gg::Device::DVI2PCIeDuo_SDI;
            else if (port_string == "dvi")
                port = gg::Device::DVI2PCIeDuo_DVI;
            else
            {
                std::cerr << "Port " << port_string << " not recognised" << std::endl;
                exit(-1);
            }
        }
    }

    IVideoSource * epiphan = gg::Factory::connect(port);
    epiphan->set_sub_frame(600, 190, 678, 678); // optimal for Storz 27020 AA straight
    float fps = 20;
    int duration = 1; // min
    int num_frames = duration * 60 * fps;
    int sleep_duration = (int)(1000/fps); // ms
    VideoFrame_BGRA frame;

    try
    {
        gg::IVideoTarget * file = gg::Factory::create(codec);
        std::string filename;
        filename.append("1min_")
                .append(codec_string)
                .append("_epiphan_")
                .append(port_string)
                .append(".avi");
        std::cout << "Saving to file " << filename << std::endl;
        file->init(filename, fps);
        for (int i = 0; i < num_frames; i++)
        {
            if (i % 5 == 0)
                std::cout << "Saving frame " << i+1 << " of " << num_frames
                          << " (" << (int)((num_frames - i) / fps) << " sec. remaining)"
                          << "\r";
            epiphan->get_frame(frame);
            file->append(frame);
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep_duration));
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
