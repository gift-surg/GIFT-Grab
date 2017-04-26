#pragma once

#include "ivideosource.h"
#include "macros.h"
#include <thread>
#include <mutex>

namespace gg
{

//!
//! \brief An object of this class serves as a
//! surrogate for implementing the observer pattern
//! (http://www.oodesign.com/observer-pattern.html)
//! for IVideoSource implementors that do not
//! inherently support it due to implementation
//! details related to used external libraries.
//!
//! This class is intended to be used as a visitor
//! (http://www.oodesign.com/visitor-pattern.html) to
//! an IVideoSource object for augmenting that
//! object's functionality.
//!
//! \attention <b>In Python:</b> This class is \b not
//! available yet.
//!
class BroadcastDaemon
{
protected:
    //!
    //! \brief
    //! \sa the public constructor
    //!
    IVideoSource * _source;

    //!
    //! \brief Flag indicating status to actual
    //! daemon thread
    //!
    bool _running;

    //!
    //! \brief Actual daemon thread
    //! \sa _running
    //! \sa _lock
    //!
    std::thread _thread;

    //!
    //! \brief
    //!
    std::mutex _lock;

public:
    //!
    //! \brief Link given video source with this
    //! object (see class description)
    //! \param source The caller is responsible
    //! for ensuring the life span of the passed
    //! source outlasts this daemon
    //! \throw VideoSourceError if passed source
    //! pointer is null
    //!
    BroadcastDaemon(IVideoSource * source);

    //!
    //! \brief
    //!
    virtual ~BroadcastDaemon();

public:
    //!
    //! \brief Start broadcasting at specified
    //! frame rate
    //! \param frame_rate in frames per second
    //! \throw VideoSourceError if broadcast
    //! already running, or if invalid frame
    //! rate value passed
    //!
    void start(float frame_rate);

    //!
    //! \brief Stop current broadcast
    //!
    void stop();

protected:
    //!
    //! \brief This method is passed to broadcaster
    //! thread
    //! \param sleep_duration_ms
    //!
    void run(float sleep_duration_ms);

private:
    //!
    //! \brief Never use default constructor, as
    //! every object of this class must be linked
    //! to an IVideoSource
    //!
    BroadcastDaemon();

private:
    DISALLOW_COPY_AND_ASSIGNMENT(BroadcastDaemon);
};

}
