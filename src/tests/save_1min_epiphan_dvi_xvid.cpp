#include "factory.h"
#include <chrono>
#include <thread>

int main()
{
    IVideoSource * epiphan = gg::Factory::connect(gg::Device::DVI2PCIeDuo_DVI);
    epiphan->set_sub_frame(600, 190, 678, 678); // optimal for Storz 27020 AA straight
    float fps = 20;
    int duration = 1; // min
    int num_frames = duration * 60 * fps;
    int sleep_duration = (int)(1000/fps); // ms
    VideoFrame_BGRA frame;

    try
    {
        gg::IVideoTarget * file = gg::Factory::create(gg::Target::File_XviD);
        file->init("2min_xvid_epiphan.avi", fps);
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
