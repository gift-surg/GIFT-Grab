#pragma once
#include <stdlib.h>

namespace gg
{

class VideoFrame_I420
{
protected:
    //!
    //! \brief
    //!
    unsigned char * _data;

    //!
    //! \brief
    //!
    size_t _data_length;

    //!
    //! \brief
    //!
    size_t _cols;

    //!
    //! \brief
    //!
    size_t _rows;

    //!
    //! \brief
    //!
    bool _manage_data;

public:
    //!
    //! \brief Constructor that uses externally
    //! managed data by default
    //! \param manage_data
    //!
    VideoFrame_I420(bool manage_data = false);

    //!
    //! \brief Initialise a frame from passed \c data
    //! \param data
    //! \param length \c data length
    //! \param cols frame width
    //! \param rows frame height
    //! \param manage_data if \c true, \c memcpy \c data,
    //! otherwise copy \c data pointer
    //!
    VideoFrame_I420(unsigned char * data, const size_t length,
                    const size_t cols, const size_t rows,
                    bool manage_data);

public:
    //!
    //! \brief
    //! \param rhs
    //!
    void operator=(const VideoFrame_I420 & rhs);

    //!
    //! \brief Get number of rows (y-axis, i.e. height)
    //! \return
    //!
    size_t rows() const
    {
        // TODO
        return -1;
    }

    //!
    //! \brief Get number of columns (x-axis, i.e. width)
    //! \return
    //!
    size_t cols() const
    {
        // TODO
        return -1;
    }

protected:
    //!
    //! \brief Convenience method to initialise object from
    //! passed \c data
    //! \param data
    //! \param length
    //! \param cols
    //! \param rows
    //! \sa _manage_data
    //!
    void init_from_pointer(unsigned char * data, size_t length,
                           size_t cols, size_t rows);
};

}
