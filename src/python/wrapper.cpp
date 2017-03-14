#include "videosourcefactory.h"
#include "videotargetfactory.h"
#include "except.h"
#include "iobservable.h"
#include "gil.h"
#ifdef USE_OPENCV
#include "opencv_video_source.h"
#include "opencv_video_target.h"
#endif // USE_OPENCV
#ifdef USE_EPIPHANSDK
#include "epiphansdk_video_source.h"
#endif
#ifdef USE_LIBVLC
#include "vlc_video_source.h"
#endif
#ifdef USE_BLACKMAGICSDK
#include "blackmagicsdk_video_source.h"
#endif
#ifdef USE_FFMPEG
#include "ffmpeg_video_source.h"
#include "ffmpeg_video_target.h"
#endif
#include <boost/python.hpp>
#include <boost/python/exception_translator.hpp>
#ifdef USE_NUMPY
#include <boost/python/numpy.hpp>
#endif

using namespace boost::python;

class VideoFrameNumPyWrapper : public gg::VideoFrame, public wrapper<gg::VideoFrame>
{
protected:
    //!
    //! \brief Wrapped \c VideoFrame object
    //!
    gg::VideoFrame * _frame;

    //!
    //! \brief \c false indicates the wrapped
    //! \c VideoFrame object is managed externally,
    //! \c true indicates it was created and is
    //! managed by this class
    //! \sa _frame
    //!
    bool _manage_frame;

public:
    //!
    //! \brief Very thinly wrap passed \c frame
    //! \param frame it is the caller's responsibility
    //! to ensure the lifetime of the object pointed to
    //! by this pointer
    //!
    VideoFrameNumPyWrapper(gg::VideoFrame * frame)
        : _frame(frame)
        , _manage_frame(false)
    {
        _manage_data = false;
        sync_specs();
    }

    //!
    //! \brief Copy constructor needs to be defined
    //! here as well for compatibility with exposed
    //! interface
    //! \param rhs
    //!
    VideoFrameNumPyWrapper(const gg::VideoFrame & rhs)
        : _frame(new gg::VideoFrame(rhs))
        , _manage_frame(true)
    {
        _manage_data = false;
        sync_specs();
    }

    //!
    //! \brief This constructor needs to be defined
    //! here as well for compatibility with exposed
    //! interface
    //! \param colour
    //! \param cols
    //! \param rows
    //!
    VideoFrameNumPyWrapper(enum gg::ColourSpace colour,
                           size_t cols, size_t rows)
        : _frame(new gg::VideoFrame(colour, cols, rows))
        , _manage_frame(true)
    {
        _manage_data = false;
        sync_specs();
    }

    //!
    //! \brief This constructor needs to be defined
    //! here as well for compatibility with exposed
    //! interface
    //! \param colour
    //! \param manage_data
    //!
    VideoFrameNumPyWrapper(enum gg::ColourSpace colour,
                           bool manage_data)
        : _frame(new gg::VideoFrame(colour, manage_data))
        , _manage_frame(true)
    {
        _manage_data = false;
        sync_specs();
    }

    ~VideoFrameNumPyWrapper()
    {
        if (_manage_frame)
            delete _frame;
    }

#ifdef USE_NUMPY
    //!
    //! \brief see:
    //! http://www.boost.org/doc/libs/1_63_0/
    //! libs/python/doc/html/tutorial/tutorial/
    //! functions.html#tutorial.functions.default_arguments
    //! \return a flat NumPy array
    //! \sa data_as_ndarray()
    //!
    numpy::ndarray data_as_flat_ndarray() const
    {
        return data_as_ndarray(false);
    }

    //!
    //! \brief Create a NumPy array referencing gg::VideoFrame::data()
    //! \param structured
    //! \return a flat NumPy array if not \c structured; otherwise one
    //! that conforms to the shape SciPy routines expect: (height,
    //! width, channels), e.g. (9, 16, 4) for BGRA data of a 16 x 9
    //! image
    //! \throw gg::BasicException if wrapped gg::VideoFrame has colour
    //! other than BGRA (currently only BGRA data supported for
    //! structured ndarray exposure)
    //!
    numpy::ndarray data_as_ndarray(bool structured) const
    {
        tuple shape, strides;
        numpy::dtype data_type = numpy::dtype::get_builtin<uint8_t>();
        if (structured)
        {
            switch(colour())
            {
            case gg::BGRA:
                shape = make_tuple(_frame->rows(), _frame->cols(), 4);
                strides = make_tuple(_frame->cols() * 4 * sizeof(uint8_t),
                                     4 * sizeof(uint8_t),
                                     sizeof(uint8_t));
                break;
            // TODO: see issue #155
            case gg::I420:
            case gg::UYVY:
            default:
                throw gg::BasicException("Structured NumPy arrays"
                                         " supported only for BGRA");
                break;
            }
        }
        else
        {
            shape = make_tuple(_frame->data_length());
            strides = make_tuple(sizeof(uint8_t));
        }

        return numpy::from_data(
                    _frame->data(), data_type, shape, strides,
                    // owner (dangerous to pass None)
                    object()
               );
    }
#endif

protected:
    void sync_specs()
    {
        _colour = _frame->colour();
        _cols = _frame->cols();
        _rows = _frame->rows();
        _data = _frame->data();
        _data_length = _frame->data_length();
    }
};

class IObservableWrapper : public gg::IObservable, public wrapper<gg::IObservable>
{
public:
    void attach(gg::IObserver & observer)
    {
        if (override f = this->get_override("attach"))
            f(boost::ref(observer));
        else
            gg::IObservable::attach(boost::ref(observer));
    }

    void default_attach(gg::IObserver & observer)
    {
        gg::IObservable::attach(boost::ref(observer));
    }

    void detach(gg::IObserver & observer)
    {
        if (override f = this->get_override("detach"))
            f(boost::ref(observer));
        else
            gg::IObservable::detach(boost::ref(observer));
    }

    void default_detach(gg::IObserver & observer)
    {
        gg::IObservable::detach(boost::ref(observer));
    }
};

class IVideoSourceWrapper : public IVideoSource, public wrapper<IVideoSource>
{
public:
    bool get_frame_dimensions(int & width, int & height)
    {
        return this->get_override("get_frame_dimensions")(width, height);
    }

    bool get_frame(gg::VideoFrame & frame)
    {
        return this->get_override("get_frame")(frame);
    }

    double get_frame_rate()
    {
        return this->get_override("get_frame_rate")();
    }

    void set_sub_frame(int x, int y, int width, int height)
    {
        this->get_override("set_sub_frame")(x, y, width, height);
    }

    void get_full_frame()
    {
        this->get_override("get_full_frame")();
    }
};

/* The inheritance should be public, in order to preserve
 * the public accessibility of the update() method. The
 * Boost.Python examples feature struct's rather than
 * classes, and in struct's everything is public by
 * definition.
 */
class IObserverWrapper : public gg::IObserver, public wrapper<gg::IObserver>
{
public:
    IObserverWrapper()
    {
        /* Default constructor that needs to be called within
         * child Python class, e.g. super(Child, self).__init__()
         * This is normally generated by the compiler
         * automatically, but we define it here explicitly so
         * that it can serve as an example for this complex case.
         */
    }

    void update(gg::VideoFrame & frame)
    {
        gg::ScopedPythonGILLock gil_lock;
        VideoFrameNumPyWrapper wrapped_frame(&frame);
        this->get_override("update")(boost::ref(wrapped_frame));
    }
};

class IVideoTargetWrapper : public gg::IVideoTarget, public wrapper<gg::IVideoTarget>
{
public:
    void append(const gg::VideoFrame & frame)
    {
        this->get_override("append")(frame);
    }
};

//!
//! \brief This class is a workaround to the method resolution
//! order problems encountered when implementing Python classes
//! inheriting from both IObservable and IObserver, i.e. a node
//! that receives, processes video data and finally passes this
//! processed data down along the pipeline.
//!
class IObservableObserverWrapper : public gg::IObservable
                                 , public gg::IObserver
                                 , public wrapper<gg::IObserver>
{
public:
    IObservableObserverWrapper()
    {
        // nop
    }

    ~IObservableObserverWrapper()
    {
        // nop
    }

public:
    void attach(gg::IObserver & observer)
    {
        if (override f = this->get_override("attach"))
            f(boost::ref(observer));
        else
            gg::IObservable::attach(boost::ref(observer));
    }

    void default_attach(gg::IObserver & observer)
    {
        gg::IObservable::attach(boost::ref(observer));
    }

    void detach(gg::IObserver & observer)
    {
        if (override f = this->get_override("detach"))
            f(boost::ref(observer));
        else
            gg::IObservable::detach(boost::ref(observer));
    }

    void default_detach(gg::IObserver & observer)
    {
        gg::IObservable::detach(boost::ref(observer));
    }

    void update(gg::VideoFrame & frame)
    {
        if (override f = this->get_override("update"))
        {
            VideoFrameNumPyWrapper wrapped_frame(&frame);
            gg::ScopedPythonGILLock gil_lock;
            f(boost::ref(wrapped_frame));
        }
        notify(frame);
    }
};

void translate_BasicException(gg::BasicException const & e)
{
    std::string msg;
    msg.append("BasicException: ").append(e.what());
    PyErr_SetString(PyExc_RuntimeError, msg.c_str());
}

void translate_VideoSourceError(gg::VideoSourceError const & e)
{
    std::string msg;
    msg.append("VideoSourceError: ").append(e.what());
    PyErr_SetString(PyExc_RuntimeError, msg.c_str());
}

void translate_DeviceAlreadyConnected(gg::DeviceAlreadyConnected const & e)
{
    std::string msg;
    msg.append("DeviceAlreadyConnected: ").append(e.what());
    PyErr_SetString(PyExc_IOError, msg.c_str());
}

void translate_DeviceNotFound(gg::DeviceNotFound const & e)
{
    std::string msg;
    msg.append("DeviceNotFound: ").append(e.what());
    PyErr_SetString(PyExc_IOError, msg.c_str());
}

void translate_DeviceOffline(gg::DeviceOffline const & e)
{
    std::string msg;
    msg.append("DeviceOffline: ").append(e.what());
    PyErr_SetString(PyExc_IOError, msg.c_str());
}

void translate_NetworkSourceUnavailable(gg::NetworkSourceUnavailable const & e)
{
    std::string msg;
    msg.append("NetworkSourceUnavailable: ").append(e.what());
    PyErr_SetString(PyExc_IOError, msg.c_str());
}

void translate_VideoTargetError(gg::VideoTargetError const & e)
{
    std::string msg;
    msg.append("VideoTargetError: ").append(e.what());
    PyErr_SetString(PyExc_RuntimeError, msg.c_str());
}

void translate_ObserverError(gg::ObserverError const & e)
{
    std::string msg;
    msg.append("ObserverError: ").append(e.what());
    PyErr_SetString(PyExc_RuntimeError, msg.c_str());
}

BOOST_PYTHON_MODULE(pygiftgrab)
{
    PyEval_InitThreads();
#ifdef USE_NUMPY
    boost::python::numpy::initialize();
#endif

    register_exception_translator<gg::BasicException>(&translate_BasicException);
    register_exception_translator<gg::VideoSourceError>(&translate_VideoSourceError);
    register_exception_translator<gg::DeviceAlreadyConnected>(&translate_DeviceAlreadyConnected);
    register_exception_translator<gg::DeviceNotFound>(&translate_DeviceNotFound);
    register_exception_translator<gg::DeviceOffline>(&translate_DeviceOffline);
    register_exception_translator<gg::NetworkSourceUnavailable>(&translate_NetworkSourceUnavailable);
    register_exception_translator<gg::VideoTargetError>(&translate_VideoTargetError);
    register_exception_translator<gg::ObserverError>(&translate_ObserverError);

    enum_<gg::ColourSpace>("ColourSpace")
        .value("BGRA", gg::ColourSpace::BGRA)
        .value("I420", gg::ColourSpace::I420)
        .value("UYVY", gg::ColourSpace::UYVY)
    ;

    enum_<gg::Device>("Device")
        .value("DVI2PCIeDuo_SDI", gg::Device::DVI2PCIeDuo_SDI)
        .value("DVI2PCIeDuo_DVI", gg::Device::DVI2PCIeDuo_DVI)
        .value("DeckLinkSDI4K", gg::Device::DeckLinkSDI4K)
    ;

    enum_<gg::Codec>("Codec")
        .value("HEVC", gg::Codec::HEVC)
        .value("Xvid", gg::Codec::Xvid)
        .value("VP9", gg::Codec::VP9)
    ;

    class_<VideoFrameNumPyWrapper>("VideoFrame", init<enum gg::ColourSpace, bool>())
        .def(init<enum gg::ColourSpace, const size_t, const size_t>())
        .def("colour", &VideoFrameNumPyWrapper::colour)
        .def("rows", &VideoFrameNumPyWrapper::rows)
        .def("cols", &VideoFrameNumPyWrapper::cols)
        .def("data_length", &VideoFrameNumPyWrapper::data_length)
        .def("required_data_length", &VideoFrameNumPyWrapper::required_data_length)
        .staticmethod("required_data_length")
        .def("required_pixel_length", &VideoFrameNumPyWrapper::required_pixel_length)
        .staticmethod("required_pixel_length")
#ifdef USE_NUMPY
        .def("data", &VideoFrameNumPyWrapper::data_as_flat_ndarray)
        .def("data", &VideoFrameNumPyWrapper::data_as_ndarray)
#endif
    ;

    class_<IObservableWrapper, boost::noncopyable>("IObservable", no_init)
        .def("attach", &IObservableWrapper::attach)
        .def("detach", &IObservableWrapper::detach)
    ;

    class_<IVideoSource, bases<gg::IObservable>, boost::noncopyable>(
                "IVideoSource", no_init)
    ;

#ifdef USE_OPENCV
    class_<VideoSourceOpenCV, bases<IVideoSource>, boost::noncopyable>(
                "VideoSourceOpenCV", init<int>())
        .def(init<char *>())
        .def("get_frame", &VideoSourceOpenCV::get_frame)
        .def("get_frame_dimensions", &VideoSourceOpenCV::get_frame_dimensions)
        .def("get_frame_rate", &VideoSourceOpenCV::get_frame_rate)
        .def("set_sub_frame", &VideoSourceOpenCV::set_sub_frame)
        .def("get_full_frame", &VideoSourceOpenCV::get_full_frame)
        .def("attach", &gg::IObservable::attach, &IObservableWrapper::default_attach)
        .def("detach", &gg::IObservable::detach, &IObservableWrapper::default_detach)
    ;
#endif // USE_OPENCV

#ifdef USE_EPIPHANSDK
    class_<gg::VideoSourceEpiphanSDK, bases<IVideoSource>, boost::noncopyable>(
                "VideoSourceEpiphanSDK", init<const std::string, const V2U_INT32>())
        .def("get_frame", &gg::VideoSourceEpiphanSDK::get_frame)
        .def("get_frame_dimensions", &gg::VideoSourceEpiphanSDK::get_frame_dimensions)
        .def("get_frame_rate", &gg::VideoSourceEpiphanSDK::get_frame_rate)
        .def("set_sub_frame", &gg::VideoSourceEpiphanSDK::set_sub_frame)
        .def("get_full_frame", &gg::VideoSourceEpiphanSDK::get_full_frame)
        .def("attach", &gg::IObservable::attach, &IObservableWrapper::default_attach)
        .def("detach", &gg::IObservable::detach, &IObservableWrapper::default_detach)
    ;
#endif

#ifdef USE_LIBVLC
    class_<gg::VideoSourceVLC, bases<IVideoSource>, boost::noncopyable>(
                "VideoSourceVLC", init<const std::string>())
        .def("get_frame", &gg::VideoSourceVLC::get_frame)
        .def("get_frame_dimensions", &gg::VideoSourceVLC::get_frame_dimensions)
        .def("get_frame_rate", &gg::VideoSourceVLC::get_frame_rate)
        .def("set_sub_frame", &gg::VideoSourceVLC::set_sub_frame)
        .def("get_full_frame", &gg::VideoSourceVLC::get_full_frame)
        .def("attach", &gg::IObservable::attach, &IObservableWrapper::default_attach)
        .def("detach", &gg::IObservable::detach, &IObservableWrapper::default_detach)
    ;
#endif

#ifdef USE_BLACKMAGICSDK
    class_<gg::VideoSourceBlackmagicSDK, bases<IVideoSource>, boost::noncopyable>(
                "VideoSourceBlackmagicSDK", init<size_t, gg::ColourSpace>())
        .def("get_frame", &gg::VideoSourceBlackmagicSDK::get_frame)
        .def("get_frame_dimensions", &gg::VideoSourceBlackmagicSDK::get_frame_dimensions)
        .def("get_frame_rate", &gg::VideoSourceBlackmagicSDK::get_frame_rate)
        .def("set_sub_frame", &gg::VideoSourceBlackmagicSDK::set_sub_frame)
        .def("get_full_frame", &gg::VideoSourceBlackmagicSDK::get_full_frame)
        .def("attach", &gg::IObservable::attach, &IObservableWrapper::default_attach)
        .def("detach", &gg::IObservable::detach, &IObservableWrapper::default_detach)
    ;
#endif

    class_<IObserverWrapper, boost::noncopyable>("IObserver")
        .def("update", pure_virtual(&gg::IObserver::update))
    ;

    class_<gg::IVideoTarget, bases<gg::IObserver>, boost::noncopyable>(
                "IVideoTarget", no_init)
        .def("update", &gg::IVideoTarget::update)
    ;

#ifdef USE_FFMPEG
    class_<gg::VideoSourceFFmpeg, bases<IVideoSource>, boost::noncopyable>(
                "VideoSourceFFmpeg", init<std::string, enum gg::ColourSpace, bool>())
        .def("get_frame", &gg::VideoSourceFFmpeg::get_frame)
        .def("get_frame_dimensions", &gg::VideoSourceFFmpeg::get_frame_dimensions)
        .def("get_frame_rate", &gg::VideoSourceFFmpeg::get_frame_rate)
        .def("set_sub_frame", &gg::VideoSourceFFmpeg::set_sub_frame)
        .def("get_full_frame", &gg::VideoSourceFFmpeg::get_full_frame)
        .def("attach", &gg::IObservable::attach, &IObservableWrapper::default_attach)
        .def("detach", &gg::IObservable::detach, &IObservableWrapper::default_detach)
    ;
    class_<gg::VideoTargetFFmpeg, bases<gg::IVideoTarget>, boost::noncopyable>(
                "VideoTargetFFmpeg", init<std::string, std::string, float>())
        .def("append", &gg::VideoTargetFFmpeg::append)
    ;
#endif

#ifdef USE_OPENCV
    class_<gg::VideoTargetOpenCV, bases<gg::IVideoTarget>, boost::noncopyable>(
                "VideoTargetOpenCV", init<std::string, std::string, float>())
        .def("append", &gg::VideoTargetOpenCV::append)
    ;
#endif // USE_OPENCV

    class_<IObservableObserverWrapper, boost::noncopyable>("IObservableObserver")
        .def("attach", &gg::IObservable::attach, &IObservableObserverWrapper::default_attach)
        .def("detach", &gg::IObservable::detach, &IObservableObserverWrapper::default_detach)
        .def("update", pure_virtual(&gg::IObserver::update))
    ;

    class_<gg::VideoSourceFactory, boost::noncopyable>("VideoSourceFactory", no_init)
        .def("get_device", &gg::VideoSourceFactory::get_device
             /* Keep returned pointer (0) alive as long as factory (1) alive.
              * Boost.Python documentation says reference_existing_object is
              * dangerous, but apparently only when no additional lifetime
              * management with a call policy is provided (for details, see:
              * http://www.boost.org/doc/libs/1_39_0/libs/python/doc/v2/reference_existing_object.html
              * ).
              */
             , return_value_policy<reference_existing_object, with_custodian_and_ward_postcall<1, 0> >()
        )
        .def("free_device", &gg::VideoSourceFactory::free_device)
        .def("connect_network_source", &gg::VideoSourceFactory::connect_network_source
             // see note below about manage_new_object
             , return_value_policy<manage_new_object>())
        .def("get_instance", &gg::VideoSourceFactory::get_instance
             , return_value_policy<reference_existing_object>()
        )
        .staticmethod("get_instance")
        .def("create_file_reader", &gg::VideoSourceFactory::create_file_reader,
             // manage_new_object => give ownership caller
             // see note below for create_file_writer
             return_value_policy<manage_new_object>()
        )
    ;

    class_<gg::VideoTargetFactory, boost::noncopyable>("VideoTargetFactory", no_init)
        .def("create_file_writer", &gg::VideoTargetFactory::create_file_writer,
             /* We use manage_new_object here in order
              * to give ownership is to client. Also
              * this ensures that the newly created
              * object will be destroyed as part of the
              * Python object destruction procedure. See
              * https://wiki.python.org/moin/boost.python/CallPolicy#manage_new_object
              */
             return_value_policy<manage_new_object>()
        )
        .def("get_instance", &gg::VideoTargetFactory::get_instance
             , return_value_policy<reference_existing_object>()
        )
        .staticmethod("get_instance")
    ;
}
