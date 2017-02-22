#pragma once

#include "ivideosource.h"
#include "device.h"
#include "codec.h"

namespace gg
{

//!
//! \brief This class implements the factory
//! design pattern for creating connections
//! to supported video sources.
//!
//! It ensures no duplicate connection is
//! created to any one supported video source,
//! and in addition takes care of properly
//! garbage-collecting all video source
//! connections created during the lifetime
//! of the singleton factory.
//!
//! \sa IVideoSource
//! \sa Device
//!
class VideoSourceFactory
{
protected:
    //!
    //! \brief The factory singleton object
    //! that does the lifetime management of
    //! all created video sources
    //!
    static VideoSourceFactory _factory_singleton;

    //!
    //! \brief So that can keep track of everything
    //! opened and in use
    //! \sa Device the length of this pointer
    //! array is to be kept in sync with the
    //! number of devices supported
    //!
    IVideoSource * _devices[NUM_DEVICES];

    //!
    //! \brief At a single point in time, only one
    //! connection using only one colour space
    //! may exist
    //!
    ColourSpace _device_colours[NUM_DEVICES];

protected:
    //!
    //! \brief The constructor should never
    //! be publicly called
    //! \sa get_instance
    //!
    VideoSourceFactory();

public:
    //!
    //! \brief Free all connected devices
    //! \sa free_device
    //!
    virtual ~VideoSourceFactory();

public:
    //!
    //! \brief Get reference to singleton
    //! instance of this class
    //! \return
    //!
    static VideoSourceFactory & get_instance();

    //!
    //! \brief Try to connect to specified \c device for
    //! capturing video with specified \c colour space.
    //!
    //! Note that the availability of capturing video
    //! with specified \c colour space depends on the
    //! custom build options selected. Cf. documentation
    //! (README file) for details.
    //!
    //! \param device
    //! \param colour
    //! \return pointer to the device's framegrabber.
    //! Client should not delete this pointer on its
    //! own. The factory will take care of properly
    //! garbage-collecting this at the end of the
    //! factory singleton's lifetime.
    //! \throw DeviceAlreadyConnected if device already
    //! connected (i.e. using another colour space)
    //! \throw DeviceNotFound if connection attempt
    //! fails
    //! \throw DeviceOffline if connection attempt
    //! OK, but device offline (i.e. getting no frames)
    //! \sa free_device if the intention is to connect
    //! to \c device with another \c colour than the
    //! one \c device is currently connected to
    //! \throw VideoSourceError if GIFT-Grab not built
    //! with options required for the passed \c device
    //! and \c colour combination OR if there was a
    //! problem constructing the requested video source
    //!
    IVideoSource * get_device(enum Device device,
                              enum ColourSpace colour);

    //!
    //! \brief Close connection to specified \c device,
    //! if existing
    //! \param device
    //! \throw DeviceNotFound if \c type not recognised
    //! \sa get_device the IVideoSource object \c
    //! get_device returns will be destroyed by this,
    //! and hence the previously returned pointer will
    //! become unusable
    //!
    void free_device(enum Device device);

    //!
    //! \brief Open connection to network connection
    //! specified by given \c address
    //! \param address
    //! \param colour
    //! \return pointer to the created video source.
    //! The ownership of this object passes to the
    //! caller. So the caller is responsible for
    //! garbage-collecting it.
    //! \throw NetworkSourceUnavailable if no
    //! connection can be established
    //! \throw VideoSourceError if GIFT-Grab not built
    //! with options required for the passed \c device
    //! and \c colour combination OR if there was a
    //! problem constructing the requested video source
    //!
    IVideoSource * connect_network_source(std::string address,
                                          enum ColourSpace colour);

    //!
    //! \brief Create a reader to read \c colour_space
    //! frames format from \c filepath
    //! \param filepath Attempting to open an already
    //! open video file leads to undefined behaviour.
    //! However once the returned video source used for
    //! reading the file is destroyed, it can be re-opened.
    //! \param colour_space
    //! \return a polymorphic video file reader. The
    //! ownership of this returned object passes to the
    //! caller. The reader behaves like other video
    //! sources as far the callback paradigm (observer
    //! pattern) is concerned. Calling the \c get_frame
    //! method on this object leads to undefined
    //! behaviour, and hence should be avoided. The reader
    //! keeps sending out data with all pixels black after
    //! all video frames in the opened file are exhausted.
    //! \throw VideoSourceError with a detailed error
    //! message if opening of \c filepath fails for any
    //! reason
    //!
    IVideoSource * create_file_reader(const std::string filepath,
                                      enum ColourSpace colour_space);

protected:
    DISALLOW_COPY_AND_ASSIGNMENT(VideoSourceFactory);
};

}
