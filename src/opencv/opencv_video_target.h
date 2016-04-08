#pragma once

#include "ivideotarget.h"
#include "except.h"

//!
//! \brief Uses OpenCV's video saving capabilities
//!
class VideoTargetOpenCV : public IVideoTarget
{
protected:
    //!
    //! \brief FOURCC code
    //! \sa VideoTargetOpenCV
    //!
    std::string _fourcc;

public:
    //!
    //! \brief Constructor featuring a \c fourcc code as in
    //! http://docs.opencv.org/2.4/doc/tutorials/highgui/video-write/video-write.html
    //! \param fourcc
    //! \throw VideoTargetError if \c fourcc not supported
    //!
    VideoTargetOpenCV(const std::string fourcc = "XVID");

public:
    void init(const std::string filepath, const float fps);

    void append(const VideoFrame_BGRA & frame);

    void finalise();
};
