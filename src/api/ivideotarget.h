#pragma once

#include "videoframe.h"
#include "except.h"
#include "iobserver.h"
#include "macros.h"

namespace gg
{

//!
//! \brief This abstract class provides the API
//! to be implemented by classes that provide
//! functionality to save streamed video e.g.
//! to files
//!
class IVideoTarget : public IObserver
{
protected:
    //!
    //! \brief Default constructor that should
    //! never be called publicly.
    //!
    IVideoTarget()
    {
        // nop
    }

public:
    //!
    //! \brief Destructor, doing nothing
    //!
    virtual ~IVideoTarget()
    {
        // nop
    }

public:
    //!
    //! \brief Append \c frame to output
    //! \param frame the caller is responsible for making
    //! sure the colour space of \c frame matches that of
    //! \c this object
    //! \throw VideoTargetError with a detailed
    //! error message if \c frame cannot be
    //! appended to output for some reason, including a
    //! mismatch between the colour space of \c frame and
    //! that of \c this object expects
    //!
    virtual void append(const VideoFrame & frame) = 0;

    void update(VideoFrame & frame) override
    {
        append(frame);
    }

protected:
    //!
    //! \brief Initialise a file writer
    //! \param filepath
    //! \param framerate how many frames per second
    //! \throw VideoTargetError if writer cannot
    //! be initialised, e.g. for IO reasons
    //!
    virtual void init(const std::string filepath, const float framerate) = 0;

    //!
    //! \brief Finalise writer, e.g. close file
    //! \throw VideoTargetError if writer cannot
    //! be finalised, e.g. for IO reasons
    //!
    virtual void finalise() = 0;

protected:
    //!
    //! \brief Directly throw a \c VideoTargetError
    //! if specified \c filepath is not of required
    //! \c filetype
    //! \param filepath
    //! \param filetype
    //! \throw VideoTargetError with a detailed
    //! message if specified \c filepath is not of
    //! required \c filetype
    //!
    virtual void check_filetype_support(
            const std::string filepath,
            const std::string filetype)
    {
        if (filepath.empty())
            throw VideoTargetError("Empty filepath specified");
        else if (filepath.length() <= filetype.length()+1) // +1 for the dot, i.e. .mp4
            throw VideoTargetError("Filepath not long enough to deduce output format");
        else if (0 != filepath.compare(
                            filepath.length() - filetype.length(),
                            filetype.length(),
                            filetype))
        {
            std::string msg;
            msg.append("Filetype of ")
               .append(filepath)
               .append(" not supported, use *.")
               .append(filetype)
               .append(" instead");
            throw VideoTargetError(msg);
        }
        else
            return; // nop, i.e. pass
    }

protected:
    DISALLOW_COPY_AND_ASSIGNMENT(IVideoTarget);
};

}
