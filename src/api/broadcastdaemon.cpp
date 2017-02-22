#include "broadcastdaemon.h"
#ifdef BUILD_PYTHON
#include "gil.h"
#endif
#include <chrono>

namespace gg
{

BroadcastDaemon::BroadcastDaemon(IVideoSource * source)
        : _source(source)
        , _running(false)
{
    if (_source == nullptr)
        throw VideoSourceError("Null pointer passed"
                               " to broadcast daemon");
}

BroadcastDaemon::~BroadcastDaemon()
{
    stop();
}

void BroadcastDaemon::start(float frame_rate)
{
    if (frame_rate <= 0.0)
        throw VideoSourceError("Invalid frame rate");

    {
        std::lock_guard<std::mutex> lock_guard(_lock);
        if (_running)
            throw VideoSourceError("Broadcast daemon already running");
        else
            _running = true;
    }

#ifdef BUILD_PYTHON
    ScopedPythonGILRelease gil_release;
#endif

    _thread = std::thread(&BroadcastDaemon::run,
                          this,
                          1000.0 / frame_rate);
}

void BroadcastDaemon::stop()
{
    bool stopped = false;
    {
        std::lock_guard<std::mutex> lock_guard(_lock);
        if (_running)
        {
            _running = false;
            stopped = true;
        }
    }
    if (stopped)
        _thread.join();
}

void BroadcastDaemon::run(float sleep_duration_ms)
{
    VideoFrame frame(_source->get_colour(), false);
    /* in case no frame is obtained, an empty frame is
     * broadcast, to help against potential synchronisation
     * problems
     */
    int cols = 0, rows = 0;
    if (not _source->get_frame_dimensions(cols, rows))
    {
        // default values, in case something goes wrong
        cols = 256;
        rows = 64;
    }
    VideoFrame * fill_frame = new VideoFrame(_source->get_colour(), cols, rows);
    std::chrono::microseconds inter_frame_duration(
                static_cast<int>(1000 * sleep_duration_ms));
    while (_running)
    {
        auto start = std::chrono::high_resolution_clock::now();
        if (_source->get_frame(frame))
            _source->notify(frame);
        else
        {
            int old_cols = cols, old_rows = rows;
            if (not _source->get_frame_dimensions(cols, rows))
            {
                cols = old_cols;
                rows = old_rows;
            }
            if (cols != fill_frame->cols() or rows != fill_frame->rows())
            {
                delete fill_frame;
                fill_frame = new VideoFrame(_source->get_colour(), cols, rows);
            }
            _source->notify(*fill_frame);
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::microseconds processing_duration =
                std::chrono::duration_cast<std::chrono::microseconds>(
                    end - start
                    );
        auto sleep_duration = inter_frame_duration - processing_duration;
        if (sleep_duration.count() > 0)
            std::this_thread::sleep_for(sleep_duration);
    }
    delete fill_frame;
}

BroadcastDaemon::BroadcastDaemon()
{
    // nop
}

}
