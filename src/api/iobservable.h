#pragma once

#include "iobserver.h"
#include <vector>
#include <algorithm>
#include <typeinfo>
#include <functional>
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
    std::vector< std::reference_wrapper<IObserver> > _observers;

    //!
    //! \brief
    //!
    std::mutex _observers_lock;

public:
    //!
    //! \brief
    //!
    virtual ~IObservable()
    {

    }

public:
    //!
    //! \brief Attach given observer to this observable.
    //! If observer already attached, no action is taken,
    //! no action is taken.
    //! \param observer
    //! \throw ObserverError with a detailed message if
    //! attach attempt unsuccessful
    //!
    virtual void attach(IObserver & observer)
    {
        std::lock_guard<std::mutex> lock_guard(_observers_lock);
        if (not attached(observer))
        {
            _observers.push_back(observer);
            if (not attached(observer))
                throw ObserverError("Could not attach observer");
        }
    }

    //!
    //! \brief Detach given observer from this observable
    //! \param observer
    //! \throw ObserverError with a detailed message if
    //! detach attempt unsuccessful
    //!
    virtual void detach(IObserver & observer)
    {
        std::lock_guard<std::mutex> lock_guard(_observers_lock);
        _observers.erase(std::find_if(
                             _observers.begin(),
                             _observers.end(),
                             [&](const std::reference_wrapper<IObserver> &o)
                                {
                                    return &(o.get()) == &observer;
                                }),
                         _observers.end());
        if (attached(observer))
            throw ObserverError("Could not detach observer");
    }

    //!
    //! \brief Notify all attached observers of new data
    //! \param frame
    //!
    virtual void notify(VideoFrame & frame) noexcept
    {
        std::lock_guard<std::mutex> lock_guard(_observers_lock);
        for (IObserver & observer : _observers)
        {
            observer.update(frame);
        }
    }

protected:
    //!
    //! \brief Check whether passed observer is already
    //! attached
    //! \param observer
    //! \return
    //!
    bool attached(const IObserver & observer) const noexcept
    {
        return std::find_if(
                    _observers.begin(),
                    _observers.end(),
                    [&](const std::reference_wrapper<IObserver> &o)
                       {
                           return &(o.get()) == &observer;
                       }) != _observers.end();
    }
};

}
