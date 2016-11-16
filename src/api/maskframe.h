#ifndef __MASKFRAME_H__
#define __MASKFRAME_H__

#include <stdlib.h>

//!
//! \brief This class is used in compute an elliptical mask
//! for grabbed video frames
//! \sa VideoFrame_BGRA::compute_image_mask
//!
class MaskFrame
{
public:
    //!
    //! \brief Default constructor, do nothing
    //!
    MaskFrame();

    //!
    //! \brief Allocate memory for a mask with specified
    //! number of \c rows and \c cols
    //! \param rows
    //! \param cols
    //!
    MaskFrame(const size_t rows, const size_t cols);

    //!
    //! \brief Free all allocated memory
    //!
    virtual ~MaskFrame();

    //!
    //! \brief Copy data from \c rhs, also setting
    //! \param rhs
    //!
    MaskFrame(const MaskFrame & rhs);

    //!
    //! \brief operator =
    //! \param rhs
    //! \sa MaskFrame(const MaskFrame & rhs)
    //!
    void operator=(const MaskFrame & rhs);

    //!
    //! \brief Get a constant pointer to already masked
    //! frame data
    //! \return
    //!
    const unsigned char * data() const
    {
        return _data;
    }

    //!
    //! \brief Get a constant pointer to mask weights
    //! \return
    //!
    const unsigned char * weights() const
    {
        return _weights;
    }

    //!
    //! \brief Get a copy of current ellipse parameters
    //! \param x
    //! \param y
    //! \param width
    //! \param height
    //! \param angle
    //! \sa set_ellipse
    //!
    void get_ellipse(float & x, float & y, float & width, float & height, float & angle) const
    {
        x = _e_loc_x; y = _e_loc_y; width = _e_width_x; height = _e_width_y; angle = _angle;
    }

    //!
    //! \brief Set ellipse parameters
    //! \param x
    //! \param y
    //! \param width
    //! \param height
    //! \param angle
    //!
    void set_ellipse(float x, float y, float width, float height, float angle)
    {
        _e_loc_x = x; _e_loc_y = y; _e_width_x = width; _e_width_y = height; _angle = angle;
    }

    //!
    //! \brief Get parameters of rectangle bounding current ellipse
    //! \param x
    //! \param y
    //! \param width
    //! \param height
    //! \sa set_bound_rect
    //!
    void get_bound_rect(float & x, float & y, float & width, float & height)
    {
        x = _r_loc_x; y = _r_loc_y; width = _r_width_x; height = _r_width_y;
    }

    //!
    //! \brief Set parameters of rectangle bounding current ellipse
    //! \param x
    //! \param y
    //! \param width
    //! \param height
    //!
    void set_bound_rect(float x, float y, float width, float height)
    {
        _r_loc_x = x; _r_loc_y = y; _r_width_x = width; _r_width_y = height;
    }

private:
    //!
    //! \brief
    //! \param rhs
    //! \sa MaskFrame(const MaskFrame & rhs)
    //!
    void clone(const MaskFrame &rhs);

    //!
    //! \brief Free all allocated data
    //! \sa ~MaskFrame
    //!
    void clear();

    //!
    //! \brief
    //! \sa data()
    //!
    unsigned char *         _data;

    //!
    //! \brief
    //! \sa weights()
    //!
    unsigned char *         _weights;

    //!
    //! \brief
    //! \todo Currently not used
    //!
    size_t                  _rows;

    //!
    //! \brief
    //! \todo Currently not used
    //!
    size_t                  _cols;

    //!
    //! \brief Centre of ellipse
    //!
    float                   _e_loc_x;

    //!
    //! \brief Centre of ellipse
    //!
    float                   _e_loc_y;

    //!
    //! \brief Width of ellipse
    //!
    float                   _e_width_x;

    //!
    //! \brief Height of ellipse
    //!
    float                   _e_width_y;

    //!
    //! \brief How much the ellipse is rotated
    //!
    float                   _angle;

    //!
    //! \brief Upper left corner of bounding box
    //!
    float                   _r_loc_x;

    //!
    //! \brief Upper left corner of bounding box
    //!
    float                   _r_loc_y;

    //!
    //! \brief Width of bounding box
    //!
    float                   _r_width_x;

    //!
    //! \brief Height of bounding box
    //!
    float                   _r_width_y;
};

#endif
