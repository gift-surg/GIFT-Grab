#include <cstring>
#include <thread>

#include "videoframe.h"
#include "videotargetfactory.h"
#include "videosourcefactory.h"


using namespace gg;
using namespace std;

void paint(unsigned char *buffer, size_t length,
           size_t width, size_t height,
           size_t frame_no = 0);

int main()
{
    // initialise a frame with dummy data
    ColourSpace colour = BGRA;
    VideoFrame in_frame(colour);
    size_t width = 128, height = 64;
    size_t data_length = VideoFrame::required_data_length(colour, width, height);
    unsigned char *in_data = new unsigned char[data_length],
                  *out_data = new unsigned char[data_length];
    in_frame.init_from_specs(in_data, data_length, width, height);

    // encode a number of this dummy frame out to file
    float frame_rate = 30.0;
    size_t num_frames = 120;
    VideoTargetFactory &tgt_factory = VideoTargetFactory::get_instance();
    std::string out_filename = "out.mp4";
    IVideoTarget *writer = tgt_factory.create_file_writer(HEVC, out_filename, frame_rate);
    for (size_t i = 0; i < num_frames; i++)
    {
        paint(in_data, data_length, width, height, 2*i);
        writer->update(in_frame);
    }

    // done, clean up
    delete writer;
    delete []in_data;
    delete []out_data;

    // read output file
    VideoSourceFactory &src_factory = VideoSourceFactory::get_instance();
    IVideoSource *reader = src_factory.create_file_reader(out_filename, BGRA);
    std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(5000));
    delete reader;

    return EXIT_SUCCESS;
}

void paint(unsigned char *buffer, size_t length,
           size_t width, size_t height,
           size_t frame_no)
{
    memset(buffer, 255, length);
    for (size_t i = 0; i < width; i++)
        for (size_t j = 0; j < height; j++)
            for (size_t c = 0; c < 3; c++)
            {
                int here = j*4*width+i*4;
                buffer[here] = (here + frame_no) % 256;
            }
}
