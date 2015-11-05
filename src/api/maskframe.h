#ifndef __MASKFRAME_H__
#define __MASKFRAME_H__

#include <stdlib.h>

class MaskFrame
{
public:
    MaskFrame();
    MaskFrame(const size_t rows, const size_t cols);
    ~MaskFrame();

    // Copy semantics
    MaskFrame(const MaskFrame & rhs);
    void operator=(const MaskFrame & rhs);

    const unsigned char * data() const
    {
        return _data;
    }

    const unsigned char * weights() const
    {
        return _weights;
    }

    void get_ellipse(float & x, float & y, float & width, float & height, float & angle) const
    {
        x = _e_loc_x; y = _e_loc_y; width = _e_width_x; height = _e_width_y; angle = _angle;
    }

    void set_ellipse(float x, float y, float width, float height, float angle)
    {
        _e_loc_x = x; _e_loc_y = y; _e_width_x = width; _e_width_y = height; _angle = angle;
    }

    void get_bound_rect(float & x, float & y, float & width, float & height)
    {
        x = _r_loc_x; y = _r_loc_y; width = _r_width_x; height = _r_width_y;
    }

    void set_bound_rect(float x, float y, float width, float height)
    {
        _r_loc_x = x; _r_loc_y = y; _r_width_x = width; _r_width_y = height;
    }

private:
    void clone(const MaskFrame &rhs);
    void clear();

    unsigned char *         _data;
    unsigned char *         _weights;

    size_t                  _rows;
    size_t                  _cols;

    // Ellipse
    float                   _e_loc_x;
    float                   _e_loc_y;
    float                   _e_width_x;
    float                   _e_width_y;
    float                   _angle;

    // Bounding rect
    float                   _r_loc_x;
    float                   _r_loc_y;
    float                   _r_width_x;
    float                   _r_width_y;
};

#endif
