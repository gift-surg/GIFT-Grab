#include "factory.h"
#include <iostream>
#include <chrono>

#ifdef GENERATE_PERFORMANCE_OUTPUT
#include <boost/timer/timer.hpp>
#include <numeric>
#ifndef dvi2pcieduo_timer_format_s
#define dvi2pcieduo_timer_format \
        std::string(", %w, %u, %s, %t, %p" \
                    ", wall (s), user (s), system (s), user+system (s), CPU (\%)\n")
#endif
#ifndef dvi2pcieduo_app_name
#define dvi2pcieduo_app_name std::string("dvi2pcieduo-")
#endif
#endif

void grab(const enum gg::Device device,
          const size_t num_frames_to_grab,
          const size_t num_recordings,
          size_t & num_frames_grabbed,
          float & duration
#ifdef GENERATE_PERFORMANCE_OUTPUT
          , const bool exclude_io_times
#endif
          )
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
    auto started_at = std::chrono::high_resolution_clock::now();
    num_frames_grabbed = 0;
#ifdef GENERATE_PERFORMANCE_OUTPUT
    std::vector<float> init_durations(num_recordings),
                       finalise_durations(num_recordings);
#endif
    for (int recording = 0; recording < num_recordings; recording++)
    {
        std::string filename;
        filename.append("test-")
                .append(device_str)
                .append("-")
                .append(std::to_string(recording))
                .append(".mp4");
        { // START auto_cpu_timer scope
#ifdef GENERATE_PERFORMANCE_OUTPUT
        boost::timer::auto_cpu_timer t(dvi2pcieduo_app_name + "grab-init" + dvi2pcieduo_timer_format);
#endif

        target->init(filename, frame_rate);
#ifdef GENERATE_PERFORMANCE_OUTPUT
        init_durations[recording] = boost::chrono::duration_cast<boost::chrono::milliseconds>(
                    boost::chrono::nanoseconds(t.elapsed().wall)
                    ).count() / 1000.0;
#endif
        } // END auto_cpu_timer scope

        int num_frames_to_grab_in_recording = num_frames_to_grab / num_recordings;
        for (int i = 0; i < num_frames_to_grab_in_recording; i++)
        {
            { // START auto_cpu_timer scope
#ifdef GENERATE_PERFORMANCE_OUTPUT
            boost::timer::auto_cpu_timer t(dvi2pcieduo_app_name + "grab-get_frame" + dvi2pcieduo_timer_format);
#endif

            if (source->get_frame(frame)) ++num_frames_grabbed;
            if (i % 30 == 0)
                std::cout << device_str << ": "
                          << i << ". frame: "
                          << frame.cols() << " x " << frame.rows()
                          << std::endl;
            } // END auto_cpu_timer scope

            { // START auto_cpu_timer scope
#ifdef GENERATE_PERFORMANCE_OUTPUT
            boost::timer::auto_cpu_timer t(dvi2pcieduo_app_name + "grab-append" + dvi2pcieduo_timer_format);
#endif

            target->append(frame);
            } // END auto_cpu_timer scope
        }
        { // START auto_cpu_timer scope
#ifdef GENERATE_PERFORMANCE_OUTPUT
        boost::timer::auto_cpu_timer t(dvi2pcieduo_app_name + "grab-finalise" + dvi2pcieduo_timer_format);
#endif

        target->finalise();
#ifdef GENERATE_PERFORMANCE_OUTPUT
        finalise_durations[recording] = boost::chrono::duration_cast<boost::chrono::milliseconds>(
                    boost::chrono::nanoseconds(t.elapsed().wall)
                    ).count() / 1000.0;
#endif
        } // END auto_cpu_timer scope
    }
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::high_resolution_clock::now() - started_at
                ).count() / 1000.0; // because chrono::seconds truncates decimal part

#ifdef GENERATE_PERFORMANCE_OUTPUT
    if (exclude_io_times)
    {
        float excluded_times = std::accumulate(init_durations.begin(), init_durations.end(), 0.0f);
        excluded_times += std::accumulate(finalise_durations.begin(), finalise_durations.end(), 0.0f);
        duration -= excluded_times;
    }
#endif
    gg::Factory::disconnect(device);
    delete target;
}

struct grab_args
{
    enum gg::Device device;
    size_t num_frames_to_grab;
    size_t num_recordings;
    size_t num_frames_grabbed;
    float duration;
#ifdef GENERATE_PERFORMANCE_OUTPUT
    bool exclude_periop_times;
#endif
};

void * grab_thread(void * args)
{
    struct grab_args * g_args = (struct grab_args *) args;
    grab(g_args->device, g_args->num_frames_to_grab,
         g_args->num_recordings,
         g_args->num_frames_grabbed, g_args->duration
#ifdef GENERATE_PERFORMANCE_OUTPUT
         , g_args->exclude_periop_times
#endif
         );
}

int main(int argc, char ** args)
{
    bool multi_threaded = false, exclude_periops = false;
    for (int i = 0; i < argc; i++)
    {
        std::string arg(args[i]);
        if (arg == "--multi-threaded")
            multi_threaded = true;
#ifdef GENERATE_PERFORMANCE_OUTPUT
        else if (arg == "--exclude-io")
            exclude_periops = true;
#endif
    }

    try
    {
        pthread_t sdi, dvi;
        struct grab_args sdi_args, dvi_args;
        sdi_args.device = gg::Device::DVI2PCIeDuo_SDI;
        dvi_args.device = gg::Device::DVI2PCIeDuo_DVI;
        sdi_args.num_frames_to_grab = dvi_args.num_frames_to_grab = 180;
        sdi_args.num_recordings = dvi_args.num_recordings = 3;
#ifdef GENERATE_PERFORMANCE_OUTPUT
        sdi_args.exclude_periop_times = dvi_args.exclude_periop_times = exclude_periops;
#endif
        if (multi_threaded)
        {
            pthread_create(&sdi, nullptr, &grab_thread, &sdi_args);
            pthread_create(&dvi, nullptr, &grab_thread, &dvi_args);
            pthread_join(sdi, nullptr);
            pthread_join(dvi, nullptr);
        }
        else
        {
            grab(sdi_args.device, sdi_args.num_frames_to_grab,
                 sdi_args.num_recordings, sdi_args.num_frames_grabbed,
                 sdi_args.duration
#ifdef GENERATE_PERFORMANCE_OUTPUT
                 , sdi_args.exclude_periop_times
#endif
                 );
            grab(dvi_args.device, dvi_args.num_frames_to_grab,
                 dvi_args.num_recordings, dvi_args.num_frames_grabbed,
                 dvi_args.duration
#ifdef GENERATE_PERFORMANCE_OUTPUT
                 , dvi_args.exclude_periop_times
#endif
                 );
        }
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
        std::cout << (multi_threaded ? "(multi-threaded)" : "(sequential)")
#ifdef GENERATE_PERFORMANCE_OUTPUT
                  << (exclude_periops ? " and (excluded init/finalise times)" : "")
#endif
                  << std::endl << std::endl;
        return EXIT_SUCCESS;
    }
    catch (std::exception & e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
