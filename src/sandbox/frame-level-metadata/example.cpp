#include <cstring>

#include "videoframe.h"
#include "videotargetfactory.h"
#include "videosourcefactory.h"


using namespace gg;
using namespace std;

void paint(unsigned char *buffer, size_t length, size_t width, size_t height);

int main()
{
    // initialise a frame with dummy data
    ColourSpace colour = BGRA;
    VideoFrame in_frame(colour);
    size_t width = 128, height = 64;
    size_t data_length = VideoFrame::required_data_length(colour, width, height);
    unsigned char *in_data = new unsigned char[data_length],
                  *out_data = new unsigned char[data_length];
    paint(in_data, data_length, width, height);
    in_frame.init_from_specs(in_data, data_length, width, height);

    // encode a number of this dummy frame out to file
    float frame_rate = 30.0;
    size_t num_frames = 120;
    IVideoTarget *writer = VideoTargetFactory::get_instance().create_file_writer(HEVC, "out.mp4", frame_rate);
    for (size_t i = 0; i < num_frames; i++)
        writer->update(in_frame);

    // done, clean up
    delete writer;
    delete []in_data;
    delete []out_data;

    return EXIT_SUCCESS;
}

void paint(unsigned char *buffer, size_t length, size_t width, size_t height)
{
    memset(buffer, 255, length);
    for (size_t i = 0; i < width; i++)
        for (size_t j = 0; j < height; j++)
            for (size_t c = 0; c < 3; c++)
            {
                int here = j*4*width+i*4;
                buffer[here] = here % 256;
            }
}
