#pragma once

#include "iobserver.h"
#include <vector>
#include <algorithm>
#include <typeinfo>
#include <functional>

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
        if (not attached(observer))
        {
            _observers.push_back(observer);
            if (not attached(observer))
                throw ObserverError("Attaching observer unsuccessful");
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
        _observers.erase(std::find_if(
                             _observers.begin(),
                             _observers.end(),
                             [&](const std::reference_wrapper<IObserver> &o)
                                {
                                    return &(o.get()) == &observer;
                                }),
                         _observers.end());
        if (attached(observer))
            throw ObserverError("Detaching observer unsuccessful");
    }

protected:
    //!
    //! \brief Notify all attached observers of new data
    //!
    virtual void notify(VideoFrame & frame) noexcept
    {
        for (IObserver & observer : _observers)
        {
            printf("%p observer is of type %s\n",
                   &observer, typeid(observer).name());
            observer.update(frame);
        }
    }

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
