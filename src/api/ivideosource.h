#ifndef __IVIDEOSOURCE_H__
#define __IVIDEOSOURCE_H__

/**
 * This abstract class defines the interface that every video source must
 * implement. This enables the underlying algorithms to be agnostic of the
 * data source.
 **/

#include "videoframe.h"

class IVideoSource
{
public:
    virtual ~IVideoSource()
    {}
    virtual bool get_frame_dimensions(int & width, int & height) = 0;
    virtual bool get_frame(VideoFrame_BGRA & frame) = 0;
    virtual double get_frame_rate() = 0;
    virtual void set_sub_frame(int x, int y, int width, int height) = 0;
    virtual void get_full_frame() { _get_sub_frame = false; }

    void set_number_of_frames_to_burn(unsigned int burn_num)
    {
        _num_burn_frames = burn_num;
    }

    unsigned int get_number_of_frames_to_burn()
    {
        return _num_burn_frames;
    }

protected:
    IVideoSource()
        :_get_sub_frame(false), _num_burn_frames(0)
    {}

    bool            _get_sub_frame;
    unsigned int    _num_burn_frames;
};

#endif
