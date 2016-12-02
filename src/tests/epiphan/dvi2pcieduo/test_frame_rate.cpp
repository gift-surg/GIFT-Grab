#include "videosourcefactory.h"
#include "ffmpeg_video_target.h"
#include "iobservable.h"
#include <thread>
#include <chrono>
#include <iostream>
#include <cstring>
#include <fstream>
#include <vector>
#include <ctime>

std::vector<gg::Device> devices;
gg::ColourSpace colour;
size_t test_duration; // seconds
float frame_rate_to_check;
std::string video_filename(""), report_filename("");

using namespace std::chrono;
typedef time_point<system_clock> timestamp;
typedef std::vector<timestamp> timestamps;

//!
//! \brief This gg::VideoTargetFFmpeg extender
//! records a timestamp every time it is
//! updated.
//!
//! It can then be used to check the frame
//! rate of the gg::IObservable it was
//! attached to.
//!
class FrameRateTimer : public gg::VideoTargetFFmpeg
{
protected:
    timestamps _timestamps;

public:
    //!
    //! \brief Create a gg::VideoTargetFFmpeg for
    //! HEVC
    //! \param filename
    //! \param frame_rate
    //!
    FrameRateTimer(const std::string filename,
                   const float frame_rate)
        : gg::VideoTargetFFmpeg("HEVC",
                                filename,
                                frame_rate)
    {
        // nop
    }

    void update(gg::VideoFrame & frame) override
    {
        _timestamps.push_back(system_clock::now());
        gg::VideoTargetFFmpeg::update(frame);
    }

    //!
    //! \brief Get statistics based on currently
    //! available timestamp collection
    //! \param max_frame_rate fps
    //! \param min_frame_rate fps
    //! \param avg_frame_rate fps
    //! \param n_timestamps
    //! \return \c true if timestamps are available
    //! AND meaningful, such that the computations
    //! can be carried out, \c false otherwise
    //!
    bool statistics(float & max_frame_rate,
                    float & min_frame_rate,
                    float & avg_frame_rate,
                    size_t & n_timestamps)
    {
        if (_timestamps.empty() or _timestamps.size() < 10)
            return false;

        n_timestamps = _timestamps.size();

        // unit: sec
        float min_duration = std::numeric_limits<float>::max(),
              max_duration = std::numeric_limits<float>::min(),
              sum_durations = 0.0;
        for (size_t i = 0; i < _timestamps.size() - 1; i++)
        {
            float cur_duration = duration_s(_timestamps[i], _timestamps[i + 1]);
            sum_durations += cur_duration;
            if (cur_duration < min_duration)
                min_duration = cur_duration;
            else if (cur_duration > max_duration)
                max_duration = cur_duration;
        }

        if (min_duration <= 0.0 or max_duration <= 0.0
            or sum_durations <= 0.0)
            return false;

        max_frame_rate = 1.0 / min_duration;
        min_frame_rate = 1.0 / max_duration;
        avg_frame_rate = (n_timestamps - 1) / sum_durations;

        return true;
    }

    //!
    //! \brief Output timestamp for each \c update()
    //! call as well as the time duration between
    //! each consecutive call pair, as well as the
    //! computed statistics to CSV file with \c
    //! filename
    //! \param filename nop if empty
    //! \sa statistics()
    //! \throw std::ios_base::failure in case the
    //! (non-empty) \c filename does not point to a
    //! valid file, writeable by user
    //!
    void report(std::string filename)
    {
        // Open file
        std::ofstream outfile;
        outfile.open(filename);
        if (not outfile.is_open())
            throw std::ios_base::failure(
                    filename.append(" could not be opened"));

        // CSV format
        std::string delimiter(", ");

        // Get statistics
        float max_fr, min_fr, avg_fr;
        size_t n_timestamps;
        if (statistics(max_fr, min_fr, avg_fr, n_timestamps))
        {
            // Write header
            outfile << "Nr. of timestamps" << delimiter
                    << "Max. frame rate (fps)" << delimiter
                    << "Min. frame rate (fps)" << delimiter
                    << "Avg. frame rate (fps)" << std::endl;
            // Write data
            outfile << n_timestamps << delimiter
                    << max_fr << delimiter
                    << min_fr << delimiter
                    << avg_fr << std::endl;
        }
        else
            outfile << "Statistics could not be computed"
                    << std::endl;

        // Write timestamps and inter-frame durations
        if (n_timestamps > 0)
        {
            char buffer [80];
            typedef duration<int, std::ratio_multiply<hours::period, std::ratio<24> >::type> days;

            // Write header
            outfile << "Frame timestamp (date-time)" << delimiter
                    << "Duration since first frame (s)" << std::endl;
            // Write data
            for (timestamp ts : _timestamps)
            {
                // Nicely format date-time for human-readability
                time_t tt = system_clock::to_time_t(ts);
                tm local_tm = *localtime(&tt);
                strftime (buffer, 80, "%Y/%m/%d %H:%M:%S", &local_tm);

                // Append millisecond resolution to human-readable timestamp
                system_clock::duration tp = ts.time_since_epoch();
                days d = duration_cast<days>(tp);
                tp -= d;
                hours h = duration_cast<hours>(tp);
                tp -= h;
                minutes m = duration_cast<minutes>(tp);
                tp -= m;
                seconds s = duration_cast<seconds>(tp);
                tp -= s;
                milliseconds ms = duration_cast<milliseconds>(tp);
                tp -= ms;
                sprintf(buffer, "%s.%03lu", buffer, ms.count());

                // Current inter-frame duration
                float cur_duration = duration_s(_timestamps[0], ts);

                // Actual output to CSV file
                outfile << buffer << delimiter
                        << cur_duration << std::endl;
            }
        }
        else
            outfile << "No timestamps"
                    << std::endl;

        // Close file
        outfile.close();
    }

protected:
    //!
    //! \brief Get duration between two timestamps in seconds
    //! \param t0
    //! \param t1
    //! \return
    //!
    float duration_s(const timestamp & t0, const timestamp & t1)
    {
        duration<float> difference = t1 - t0;
        return difference.count();
    }
};

bool parse_args(int argc, char * argv[])
{
    if (argc < 6)
        return false;

    if (std::strcmp(argv[1], "DVI") == 0 or
        std::strcmp(argv[1], "2") == 0)
        devices.push_back(gg::DVI2PCIeDuo_DVI);
    if (std::strcmp(argv[1], "SDI") == 0 or
        std::strcmp(argv[1], "2") == 0)
        devices.push_back(gg::DVI2PCIeDuo_SDI);
    if (devices.empty())
        return false;

    if (std::strcmp(argv[2], "BGRA") == 0)
        colour = gg::BGRA;
    else if (std::strcmp(argv[2], "I420") == 0)
        colour = gg::I420;
    else
        return false;

    int test_duration_ = std::atoi(argv[3]);
    if (test_duration_ <= 0)
        return false;
    else
        test_duration = test_duration_;

    double frame_rate_to_check_ = std::atof(argv[4]);
    if (frame_rate_to_check_ <= 0.0)
        return false;
    else
        frame_rate_to_check = frame_rate_to_check_;

    video_filename = std::string(argv[5]);

    if (argc >= 7)
        report_filename = std::string(argv[6]);

    return true;
}

void synopsis(int argc, char * argv[])
{
    printf("%s  DVI | SDI | 2  BGRA | I420  <test_duration>"
           "  <frame_rate_to_check>  <video_filename>"
           "  [ <report_filename> ]"
           "\n",
           argv[0]);
}

int main(int argc, char * argv[])
{
    if (not parse_args(argc, argv))
    {
        synopsis(argc, argv);
        return EXIT_FAILURE;
    }

    gg::VideoSourceFactory & source_fac = gg::VideoSourceFactory::get_instance();
    std::vector<IVideoSource *> sources;
    for (gg::Device device : devices)
    {
        IVideoSource * source = source_fac.get_device(device, colour);
        sources.push_back(source);
    }

    std::vector<FrameRateTimer *> timers;
    size_t i = 0;
    for (IVideoSource * source : sources)
    {
        FrameRateTimer * timer = new FrameRateTimer(
                video_filename + std::to_string(devices[i++]).append(".mp4"),
                frame_rate_to_check);
        source->attach(*timer);
        timers.push_back(timer);
    }

    std::this_thread::sleep_for(std::chrono::seconds(test_duration));

    for (size_t i = 0; i < sources.size(); i++)
        sources[i]->detach(*timers[i]);

    float max_frs[timers.size()], min_frs[timers.size()], avg_frs[timers.size()];
    size_t n_timestamps[timers.size()];
    for (size_t i = 0; i < timers.size(); i++)
    {
        timers[i]->report(report_filename + std::to_string(devices[i]).append(".csv"));
        if (not timers[i]->statistics(max_frs[i], min_frs[i], avg_frs[i], n_timestamps[i]))
            return EXIT_FAILURE;
    }

    for (FrameRateTimer * timer : timers)
        delete timer;

    for (size_t i = 0; i < timers.size(); i++)
        if (avg_frs[i] < frame_rate_to_check or min_frs[i] < frame_rate_to_check)
            return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
