#pragma once

#include "iobserver.h"

namespace gg
{

//!
//! \brief Every IVideoSource that broadcasts
//! video frames needs to implement this interface,
//! which defines the observable (subject / publisher)
//! part of the observer design pattern (aka
//! subscriber-publisher).
//!
class IObservable
{
protected:
    //!
    //! \brief
    //!
    std::vector<IObserver *> _observers;

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
            _observers.push_back(&observer);
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
        _observers.erase(std::remove(
                             _observers.begin(),
                             _observers.end(),
                             &observer),
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
        for (IObserver * observer : _observers)
            observer->update(frame);
    }

    //!
    //! \brief Check whether passed observer is already
    //! attached
    //! \param observer
    //! \return
    //!
    bool attached(const IObserver & observer) const noexcept
    {
        return std::find(
                    _observers.begin(), _observers.end(),
                    &observer
                    ) != _observers.end();
    }
};

}
