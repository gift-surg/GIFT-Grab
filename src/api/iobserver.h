#pragma once

#include "videoframe.h"

namespace gg
{

//!
//! \brief Every observer interested in listening
//! to IVideoSource data must implement this
//! interface, which defines the observer
//! (subscriber) part of the observer design
//! pattern (aka subscriber-publisher).
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
class IObserver
{
public:
    //!
    //! \brief
    //!
    virtual ~IObserver()
    {

    }

public:
    //!
    //! \brief
    //! \param frame
    //!
    virtual void update(VideoFrame & frame) = 0;
};

}
