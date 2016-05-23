#include "factory.h"
#include <iostream>
#include <chrono>

void grab(const enum gg::Device device,
          const size_t num_frames,
          float & duration)
{
    std::string device_str = "";
    switch (device)
    {
    case gg::Device::DVI2PCIeDuo_DVI:
        device_str = "DVI";
        break;
    case gg::Device::DVI2PCIeDuo_SDI:
        device_str = "SDI";
        break;
    default:
        throw gg::BasicException("Device not set");
    }

    gg::VideoFrame_I420 frame;
    IVideoSource * source = gg::Factory::connect(device);
    auto started_at = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < num_frames; )
    {
        source->get_frame(frame);
        std::cout << device_str << ": "
                  << ++i << ". frame: "
                  << frame.cols() << " x " << frame.rows()
                  << std::endl;
    }
    duration = std::chrono::duration_cast<std::chrono::seconds>(
                    std::chrono::high_resolution_clock::now() - started_at
                ).count();
    gg::Factory::disconnect(device);
}

int main()
{
#ifdef USE_COLOUR_SPACE_I420
    try
    {
        const size_t num_frames = 180;
        float duration_sdi = 0;
        grab(gg::Device::DVI2PCIeDuo_SDI, num_frames, duration_sdi);
        float frame_rate_sdi = num_frames/duration_sdi;
        std::cout << num_frames << " frames grabbed in "
                  << duration_sdi << " sec: "
                  << frame_rate_sdi << " fps"
                  << std::endl;
        return EXIT_SUCCESS;
    }
    catch (std::exception & e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
#else
    std::cerr << "Not using I420 colour space, exiting." << std::endl;
    return EXIT_FAILURE;
#endif
}
