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
    //! \param filename
    //! \param frame_rate
    //! \throw VideoTargetError with a detailed error message
    //! if \c fourcc not supported or if the target cannot be
    //! initialised for some reason
    //!
    VideoTargetOpenCV(const std::string fourcc,
                      const std::string filename,
                      const float frame_rate);

    //!
    //! \brief
    //!
    virtual ~VideoTargetOpenCV();

public:
    void init(const std::string filepath, const float framerate);

    void append(const VideoFrame & frame);

    void finalise();

protected:
    DISALLOW_COPY_AND_ASSIGNMENT(VideoTargetOpenCV);
};

}
