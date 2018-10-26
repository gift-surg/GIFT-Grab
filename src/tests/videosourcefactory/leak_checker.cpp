#include <cstdlib>
#include <cstring>
#include "videosourcefactory.h"

using namespace gg;

void show_error(int argc, char *argv[])
{
    printf("Please check CLI args of:\n");
    for (int i = 0; i < argc - 1; i++)
        printf("%s ", argv[i]);
    printf("%s\n", argv[argc - 1]);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    if (argc < 3)
        show_error(argc, argv);

    Device device;
    if (std::strcmp(argv[1], "DVI2PCIeDuo_SDI") == 0)
        device = Device::DVI2PCIeDuo_SDI;
    else if (std::strcmp(argv[1], "DVI2PCIeDuo_DVI") == 0)
        device = Device::DVI2PCIeDuo_DVI;
    else if (std::strcmp(argv[1], "DeckLinkSDI4K") == 0)
        device = Device::DeckLinkSDI4K;
    else if (std::strcmp(argv[1], "DeckLink4KExtreme12G") == 0)
        device = Device::DeckLink4KExtreme12G;
    else
        show_error(argc, argv);

    ColourSpace colour;
    if (std::strcmp(argv[2], "BGRA") == 0)
        colour = ColourSpace::BGRA;
    else if (std::strcmp(argv[2], "I420") == 0)
        colour = ColourSpace::I420;
    else if (std::strcmp(argv[2], "UYVY") == 0)
        colour = ColourSpace::UYVY;
    else
        show_error(argc, argv);

    VideoSourceFactory &factory = VideoSourceFactory::get_instance();
    IVideoSource *source = factory.get_device(device, colour);

    /* Upon exiting, the factory should free all
     * allocated memory. So there should be no
     * leaks.
     */
    return EXIT_SUCCESS;
}
