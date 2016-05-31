#include "factory.h"
#include <iostream>
#include <chrono>

void grab(const enum gg::Device device,
          const size_t num_frames_to_grab,
          size_t & num_frames_grabbed,
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

    float frame_rate = 0;
#ifdef USE_COLOUR_SPACE_I420
    gg::VideoFrame_I420 frame;
    frame_rate = 59;
#else
    VideoFrame_BGRA frame;
    frame_rate = 29;
#endif

    IVideoSource * source = gg::Factory::connect(device);
    gg::IVideoTarget * target = gg::Factory::writer(gg::Storage::File_H265);
    target->init("test.mp4", frame_rate);
    auto started_at = std::chrono::high_resolution_clock::now();
    num_frames_grabbed = 0;
    for (int i = 0; i < num_frames_to_grab; i++)
    {
        if (source->get_frame(frame)) ++num_frames_grabbed;
        if (i % 30 == 0)
            std::cout << device_str << ": "
                      << i << ". frame: "
                      << frame.cols() << " x " << frame.rows()
                      << std::endl;
        target->append(frame);
    }
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::high_resolution_clock::now() - started_at
                ).count() / 1000.0; // because chrono::seconds truncates decimal part
    gg::Factory::disconnect(device);
    target->finalise();
}

struct grab_args
{
    enum gg::Device device;
    size_t num_frames_to_grab;
    size_t num_frames_grabbed;
    float duration;
};

void * grab_thread(void * args)
{
    struct grab_args * g_args = (struct grab_args *) args;
    grab(g_args->device, g_args->num_frames_to_grab,
         g_args->num_frames_grabbed, g_args->duration);
}

int main()
{
    try
    {
        pthread_t sdi, dvi;
        struct grab_args sdi_args, dvi_args;
        sdi_args.device = gg::Device::DVI2PCIeDuo_SDI;
        dvi_args.device = gg::Device::DVI2PCIeDuo_DVI;
        sdi_args.num_frames_to_grab = dvi_args.num_frames_to_grab = 180;
        pthread_create(&sdi, nullptr, &grab_thread, &sdi_args);
        pthread_create(&dvi, nullptr, &grab_thread, &dvi_args);
        pthread_join(sdi, nullptr);
        pthread_join(dvi, nullptr);
        float frame_rate_sdi = sdi_args.num_frames_to_grab/sdi_args.duration;
        float frame_rate_dvi = dvi_args.num_frames_to_grab/dvi_args.duration;
        std::cout << "SDI grabbed " << sdi_args.num_frames_grabbed
                  << " frames in "
                  << sdi_args.duration << " sec: "
                  << frame_rate_sdi << " fps"
                  << std::endl;
        std::cout << "DVI grabbed "  << dvi_args.num_frames_grabbed
                  << " frames in "
                  << dvi_args.duration << " sec: "
                  << frame_rate_dvi << " fps"
                  << std::endl;
        return EXIT_SUCCESS;
    }
    catch (std::exception & e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
