#include "videosourcefactory.h"
#include "iobserver.h"
#include "iobservable.h"
#include <thread>
#include <chrono>
#include <cassert>

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
public:
    void update(gg::VideoFrame & frame) override
    {
        // TODO
    }

    //!
    //! \brief Check whether the collected timestamps
    //! are in line with \c frame_rate
    //! \param frame_rate
    //! \return
    //!
    bool frame_rate_ok(float frame_rate)
    {
        // TODO
        return false;
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
    //! \param filename
    //! \sa statistics()
    //!
    void report(std::string filename)
    {
        // TODO
    }
};

int main(int argc, char * argv[])
{
    // TODO
    gg::Device device = gg::DVI2PCIeDuo_DVI;
    gg::ColourSpace colour = gg::I420;
    size_t duration = 10; // seconds
    float frame_rate = 10.0;
    std::string report_filename("./timer_report.csv");

//    gg::VideoSourceFactory & source_fac = gg::VideoSourceFactory::get_instance();
//    IVideoSource * epiphan = source_fac.get_device(device, colour);

    FrameRateTimer timer;
//    epiphan->attach(timer);

//    std::this_thread::sleep_for(std::chrono::seconds(duration));

//    epiphan->detach(timer);

//    timer.report();
    float max_fr, min_fr, avg_fr;
    size_t n_timestamps;
    if (not timer.statistics(max_fr, min_fr, avg_fr, n_timestamps))
        return EXIT_FAILURE;

    timer.report(report_filename);

    return avg_fr >= frame_rate and min_fr >= frame_rate ? EXIT_SUCCESS : EXIT_FAILURE;
}
