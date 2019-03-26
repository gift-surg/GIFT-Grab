#include <cstring>

#include "videoframe.h"
#include "videotargetfactory.h"
#include "videosourcefactory.h"


using namespace gg;
using namespace std;

int main()
{
    // initialise a frame with dummy data
    ColourSpace colour = BGRA;
    VideoFrame in_frame(colour);
    size_t width = 128, height = 64;
    size_t data_length = VideoFrame::required_data_length(colour, width, height);
    unsigned char *in_data = new unsigned char[data_length],
                  *out_data = new unsigned char[data_length];
    memset(in_data, 0, data_length);
    in_frame.init_from_specs(in_data, data_length, width, height);

    // encode a number of this dummy frame out to file
    float frame_rate = 30.0;
    size_t num_frames = 10;
    IVideoTarget *writer = VideoTargetFactory::get_instance().create_file_writer(HEVC, "out.mp4", frame_rate);
    for (size_t i = 0; i < num_frames; i++)
        writer->update(in_frame);

    // done, clean up
    delete []in_data;
    delete []out_data;

    return EXIT_SUCCESS;
}
