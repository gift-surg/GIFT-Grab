#include "videosourcefactory.h"
#include "iobserver.h"
#include "iobservable.h"
#include <thread>
#include <chrono>
#include <iostream>
#include <cstring>
#include <fstream>
#include <vector>

gg::Device device;
gg::ColourSpace colour;
size_t test_duration; // seconds
float frame_rate_to_check;
std::string report_filename("");

using namespace std::chrono;
typedef time_point<system_clock> timestamp;
typedef std::vector< timestamp > timestamps;

//!
//! \brief This gg::IObserver implementor
//! records a timestamp every time it is
//! updated.
//!
//! It can then be used to check the frame
//! rate of the gg::IObservable it was
//! attached to.
//!
class FrameRateTimer : public gg::IObserver
{
protected:
    timestamps _timestamps;

public:
    void update(gg::VideoFrame & frame) override
    {
        _timestamps.push_back(system_clock::now());
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
            duration<float> difference = _timestamps[i + 1] - _timestamps[i];
            float cur_duration = difference.count();
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
    //! \brief Output collected timestamps, as well
    //! as computed statistics to CSV file with \c
    //! filename
    //! \param filename nop if empty
    //! \sa statistics()
    //! \throw std::ios_base::failure in case the
    //! (non-empty) \c filename does not point to a
    //! valid file, writeable by user
    //!
    void report(std::string filename)
    {
        float max_fr, min_fr, avg_fr;
        size_t n_timestamps;
        if (not statistics(max_fr, min_fr, avg_fr, n_timestamps))
            // nothing to output
            return;

        std::ofstream outfile;
        outfile.open(filename);
        if (not outfile.is_open())
            throw std::ios_base::failure(
                    filename.append(" could not be opened"));
        std::string delimiter(",\n");
        outfile << n_timestamps << delimiter;
        outfile << max_fr << delimiter;
        outfile << min_fr << delimiter;
        outfile << avg_fr << delimiter;
        for (timestamp ts: _timestamps)
        {
            std::time_t t = system_clock::to_time_t(ts);
            outfile << std::ctime(&t) << delimiter;
        }
        outfile.close();
    }
};

bool parse_args(int argc, char * argv[])
{
    if (argc < 5)
        return false;

    if (std::strcmp(argv[1], "DVI") == 0)
        device = gg::DVI2PCIeDuo_DVI;
    else if (std::strcmp(argv[1], "SDI") == 0)
        device = gg::DVI2PCIeDuo_SDI;
    else
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

    if (argc >= 6)
        report_filename = std::string(argv[5]);

    return true;
}

void synopsis(int argc, char * argv[])
{
    printf("%s  DVI | SDI  BGRA | I420  <test_duration>"
           "  <frame_rate_to_check>  [ <report_filename> ]"
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

//    gg::VideoSourceFactory & source_fac = gg::VideoSourceFactory::get_instance();
//    IVideoSource * epiphan = source_fac.get_device(device, colour);

    FrameRateTimer timer;
//    epiphan->attach(timer);

//    std::this_thread::sleep_for(std::chrono::seconds(duration));

//    epiphan->detach(timer);

    float max_fr, min_fr, avg_fr;
    size_t n_timestamps;
    if (not timer.statistics(max_fr, min_fr, avg_fr, n_timestamps))
        return EXIT_FAILURE;

    timer.report(report_filename);

    return ( avg_fr >= frame_rate_to_check and
             min_fr >= frame_rate_to_check ) ? EXIT_SUCCESS : EXIT_FAILURE;
}
