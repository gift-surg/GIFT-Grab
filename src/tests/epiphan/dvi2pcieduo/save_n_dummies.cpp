#include "io_utils.h"
#include "videoframe.h"

using namespace gg;

int main()
{
    size_t width = 1920, height = 1080;
    VideoFrame frame(BGRA, width, height);
    memset(frame.data(), 255, frame.data_length());
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            unsigned char *data = frame.data();
            int pixel_idx = j * (width * 4) + i * 4;

            if (i < width / 2 && j < height / 2)
            {
                data[pixel_idx + 0] = 0;
                data[pixel_idx + 1] = 0;
            }
            else if (i < width / 2 && j >= height / 2)
            {
                data[pixel_idx + 0] = 0;
                data[pixel_idx + 2] = 0;
            }
            else if (i >= width / 2 && j < height / 2)
            {
                data[pixel_idx + 1] = 0;
                data[pixel_idx + 2] = 0;
            }
            else if (i >= width / 2 && j >= height / 2)
            {
                data[pixel_idx + 0] = 0;
            }
        }
    }

    for (int i = 0; i < 3; i++)
    {
        string filename = "snapshot-";
        filename.append(to_string(i));
        filename.append(".bin");

        save_binary(frame, filename);
    }

    return EXIT_SUCCESS;
}
