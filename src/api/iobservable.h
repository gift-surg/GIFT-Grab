#pragma once

#include "iobserver.h"
#include <vector>
#include <mutex>

namespace gg
{

//!
//! \brief Every IVideoSource that broadcasts
//! video frames needs to implement this interface,
//! which defines the observable (subject / publisher)
//! part of the observer design pattern (aka
//! subscriber-publisher).
//!
//! \attention <b>In Python:</b> an intermediate node
//! in a video processing pipeline should extend the
//! \c IObservableObserver class (<b>not available in
//! C++</b>) instead of IObservable and IObserver.
//! Below is the source code for a sample processing
//! node in Python.
//!
//! \code{.py}
//! from pygiftgrab import IObservableObserver
//!
//! class MyProcNode(IObservableObserver):
//!
//!     def __init__(self):
//!         super(MyProcNode, self).__init__()
//!
//!     def update(self, frame):
//!         # Implement gg::IObserver::update(frame)
//!         pass
//! \endcode
//!
//! Note that no new buffer is created when broadcasting
//! data, i.e. every IObserver is responsible for
//! ensuring data is promptly copied to their own
//! data buffer.
//!
class IObservable
{
protected:
    //!
    //! \brief
    //!
    std::vector< IObserver * > _observers;

    //!
    //! \brief
    //!
    std::mutex _observers_lock;

public:
    //!
    //! \brief
    //!
    virtual ~IObservable();

public:
    //!
    //! \brief Attach given observer to this observable.
    //! If observer already attached, no action is taken,
    //! no action is taken.
    //! \param observer
    //! \throw ObserverError with a detailed message if
    //! attach attempt unsuccessful
    //!
    virtual void attach(IObserver & observer);

    //!
    //! \brief Detach given observer from this observable
    //! \param observer
    //! \throw ObserverError with a detailed message if
    //! detach attempt unsuccessful
    //!
    virtual void detach(IObserver & observer);

    //!
    //! \brief Notify all attached observers of new data
    //!
    //! \attention <b>In Python:</b> This function is \b
    //! not available yet.
    //!
    //! \param frame
    //!
    virtual void notify(VideoFrame & frame) noexcept;

protected:
    //!
    //! \brief Check whether passed observer is already
    //! attached. Because this function is protected, no
    //! thread-safety operation is performed inside it.
    //! \param observer
    //! \return
    //! \sa _observers_lock
    //!
    bool attached(const IObserver & observer) const noexcept;
};

}
