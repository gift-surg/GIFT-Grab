#include "iobservable.h"

namespace gg
{

IObservable::~IObservable()
{

}

void IObservable::attach(IObserver & observer)
{
    std::lock_guard<std::mutex> lock_guard(_observers_lock);
    if (not attached(observer))
    {
        _observers.push_back(observer);
        if (not attached(observer))
            throw ObserverError("Could not attach observer");
    }
}

void IObservable::detach(IObserver & observer)
{
    std::lock_guard<std::mutex> lock_guard(_observers_lock);
    _observers.erase(std::find_if(
                         _observers.begin(),
                         _observers.end(),
                         /* Note that this lambda is a duplicate of the one
                          * used in the attach function below.
                          */
                         [&](const std::reference_wrapper<IObserver> &o)
                            {
                                return &(o.get()) == &observer;
                            }),
                     _observers.end());
    if (attached(observer))
        throw ObserverError("Could not detach observer");
}

void IObservable::notify(VideoFrame & frame) noexcept
{
    std::lock_guard<std::mutex> lock_guard(_observers_lock);
    for (IObserver & observer : _observers)
    {
        observer.update(frame);
    }
}

bool IObservable::attached(const IObserver & observer) const noexcept
{
    return std::find_if(
                _observers.begin(),
                _observers.end(),
                /* Note that this lambda is a duplicate of the one
                 * used in the detach function above.
                 */
                [&](const std::reference_wrapper<IObserver> &o)
                   {
                       return &(o.get()) == &observer;
                   }) != _observers.end();
}

}
