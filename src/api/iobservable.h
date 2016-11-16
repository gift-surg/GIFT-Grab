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
