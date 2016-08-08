#pragma once

#include "videoframe.h"
#ifdef USE_I420
#include "videoframe_i420.h"
#endif
#include "except.h"

namespace gg
{

//!
//! \brief This abstract class provides the API
//! to be implemented by classes that provide
//! functionality to save streamed video e.g.
//! to files
//!
class IVideoTarget
{
public:
    //!
    //! \brief Initialise a file writer
    //! \param filepath
    //! \param framerate how many frames per second
    //! \throw VideoTargetError if writer cannot
    //! be initialised, e.g. for IO reasons
    //!
    virtual void init(const std::string filepath, const float framerate) = 0;

    //!
    //! \brief Append \c frame to output
    //! \param frame
    //! \throw VideoTargetError with a detailed
    //! error message if \c frame cannot be
    //! appended to output for some reason
    //!
    virtual void append(const VideoFrame_BGRA & frame) = 0;

#ifdef USE_I420
    //!
    //! \brief Append I420 \c frame to output
    //! \param frame
    //! \sa append
    //! \throw VideoTargetError by default. Sub-classes
    //! accepting I420 frames must override this function.
    //!
    virtual void append(const VideoFrame_I420 & frame)
    {
        throw VideoTargetError("By default, append(VideoFrame_I420 & frame) not implemented");
    }
#endif

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
};

}
