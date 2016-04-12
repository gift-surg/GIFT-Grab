#pragma once

#include "ivideotarget.h"
#include "except.h"

namespace gg
{

//!
//! \brief Uses OpenCV's video saving capabilities
//!
class VideoTargetOpenCV : public IVideoTarget
{
protected:
    //!
    //! \brief
    //!
    cv::VideoWriter _writer;

    //!
    //! \brief This buffer will hold BGR data
    //! when writing to file
    //!
    cv::Mat _buffer_bgr;

    //!
    //! \brief
    //!
    std::string _filepath;

    //!
    //! \brief
    //!
    float _framerate;

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
    void init(const std::string filepath, const float framerate);

    void append(const VideoFrame_BGRA & frame);

    void finalise();
};

}
