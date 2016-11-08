#pragma once

#include <boost/python.hpp>

namespace gg
{

//!
//! \brief This class provides a high-level means
//! of releasing the Python Global Interpreter
//! Lock (GIL).
//!
//! See https://docs.python.org/2/c-api/init.html
//! for a discussion. Also note that there are some
//! issues around the GIL (https://wiki.python.org/moin/GlobalInterpreterLock).
//! So use this with care. Below
//! is a sample bare-bones code snippet showing how
//! we make use of the GIL mechanism in GIFT-Grab:
//!
//! \code{.cpp}
//! PyEval_InitThreads() // acquires GIL
//! // Some C++ stuff here
//! {
//!     ScopedPythonGILRelease gil_release;
//!     /* e.g. start a C++ thread that occasionally
//!      * updates a Python object
//!      */
//! } // GIL release goes out of scope, so is no more
//!
//! // From within thread started above:
//! {
//!     ScopedPythonGILMutex gil_lock;
//!     /* e.g. call a Python function
//!      */
//!
//!     /* GIL deadlocks if the following sleep is
//!      * omitted
//!      */
//!     std::this_thread:sleep_for(std::chrono::milliseconds(3));
//! } // GIL lock goes out of scope: not locked now
//! \endcode
//!
class ScopedPythonGILRelease
{
protected:
    //!
    //! \brief Current Python thread state, for
    //! saving and restoring
    //!
    PyThreadState * _thread_state;

public:
    //!
    //! \brief Release GIL, if already locked,
    //! nop otherwise
    //!
    inline ScopedPythonGILRelease()
        : _thread_state(nullptr)
    {
        if (PyEval_ThreadsInitialized() != 0)
            /* non-zero means PyEval_InitThreads()
             * has been called, i.e. Python API
             * loaded
             */
            _thread_state = PyEval_SaveThread();
    }

    //!
    //! \brief Lock back previously released
    //! GIL
    //!
    inline ~ScopedPythonGILRelease()
    {
        if (_thread_state != nullptr)
        {
            PyEval_RestoreThread(_thread_state);
            _thread_state = nullptr;
        }
    }
};

//!
//! \brief This class provides a high-level means
//! of locking the Python Global Interpreter
//! Lock (GIL).
//!
//! See ScopedPythonGILRelease for a brief example.
//!
class ScopedPythonGILLock
{
protected:
    //!
    //! \brief GIL state to be saved and later
    //! restored
    //!
    PyGILState_STATE _gil_state;

public:
    //!
    //! \brief Lock GIL
    //!
    inline ScopedPythonGILLock()
    {
        _gil_state = PyGILState_Ensure();
    }

    //!
    //! \brief Release previously released
    //! GIL
    //!
    inline ~ScopedPythonGILLock()
    {
        PyGILState_Release(_gil_state);
    }
};

}
