#include "factory.h"
#include <iostream>
#include <chrono>

int main()
{
#ifdef USE_COLOUR_SPACE_I420
    try
    {
        gg::VideoFrame_I420 frame;
        enum gg::Device device = gg::Device::DVI2PCIeDuo_SDI;
        IVideoSource * source = gg::Factory::connect(device);
        auto started_at = std::chrono::high_resolution_clock::now();
        int num_frames = 180;
        for (int i = 0; i < num_frames; )
        {
            source->get_frame(frame);
            std::cout << ++i << ". frame: "
                      << frame.cols() << " x " << frame.rows()
                      << std::endl;
        }
        float elapsed_seconds = std::chrono::duration_cast<std::chrono::seconds>(
                        std::chrono::high_resolution_clock::now() - started_at
                    ).count();
        float frame_rate = num_frames/elapsed_seconds;
        std::cout << num_frames << " frames grabbed in " << elapsed_seconds << " sec: "
                  << frame_rate << " fps"
                  << std::endl;
        gg::Factory::disconnect(device);
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
