#include "io_utils.h"
#include <fstream>

void save_binary(VideoFrame &frame, string filename)
{
    unsigned char *data = frame.data();
    ofstream outfile(filename, ofstream::binary);
    uint32_t width = frame.cols(), height = frame.rows();
    outfile.write(reinterpret_cast<char *>(&width), 4);
    outfile.write(reinterpret_cast<char *>(&height), 4);
    outfile.write(reinterpret_cast<char *>(data), frame.data_length());
}
