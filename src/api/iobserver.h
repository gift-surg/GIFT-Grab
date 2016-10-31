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
class IObserver
{
public:
    //!
    //! \brief
    //! \param frame
    //!
    virtual void update(VideoFrame & frame) = 0;
};

}
