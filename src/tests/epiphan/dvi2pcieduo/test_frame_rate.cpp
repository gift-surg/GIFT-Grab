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
typedef std::vector< time_point<system_clock> > timestamps;

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
        // TODO
        return false;
    }

    //!
    //! \brief Output collected timestamps, as well
    //! as computed statistics to CSV file with \c
    //! filename
    //! \param filename nop if empty
    //! \sa statistics()
    //! \throw in case (non-empty) \c filename not
    //! pointing to a valid file, writeable by user
    //!
    void report(std::string filename)
    {
        // TODO
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
