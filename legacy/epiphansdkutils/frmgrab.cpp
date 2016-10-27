#include "frmgrab.h"
#include <iostream>

int main()
{
    const char * sn = "D2P260426.sdi"; // or D2P260426.vga
    FrmGrabber * fg = nullptr;
    FrmGrab_Init();

    fg = FrmGrabLocal_OpenSN(sn);
    if (not fg)
        return -1;
    V2U_UINT32 flags[2] = { V2U_GRABFRAME_FORMAT_I420,
                            V2U_GRABFRAME_FORMAT_BGR24 };
    V2URect roi;
    roi.x = 300;
    roi.y = 200;
    roi.width = 600;
    roi.height = 400;
    for (int i = 0; i < 2; i++)
    {
        V2U_GrabFrame2 * frame = FrmGrab_Frame(fg, flags[i], &roi);
        if (frame)
        {
            std::string palette_str = "unknown";
            switch(frame->palette)
            {
            case V2U_GRABFRAME_FORMAT_I420:
                palette_str = "I420";
                break;
            case V2U_GRABFRAME_FORMAT_BGR24:
                palette_str = "BGR24";
                break;
            }

            std::cout << std::endl;
            std::cout << "Frame # " << i << std::endl;
            std::cout << "pixbuf=" << frame->pixbuf << " "
                      << "pixbuflen=" << frame->pixbuflen << " "
                      << "imagelen=" << frame->imagelen << std::endl
                      << "palette=" << palette_str << "\n"
                      << "roi-x=" << frame->crop.x << " "
                      << "roi-y=" << frame->crop.y << " "
                      << "roi-width=" << frame->crop.width << " "
                      << "roi-height=" << frame->crop.height << "\n"
                      << "mode-width=" << frame->mode.width << " "
                      << "mode-height=" << frame->mode.height << " "
                      << "mode-vfreq=" << frame->mode.vfreq << " "
                      << "retcode=" << frame->retcode << std::endl;
            FrmGrab_Release(fg, frame);
        }
    }
    FrmGrab_Close(fg);
    FrmGrab_Deinit();
    return 0;
}
