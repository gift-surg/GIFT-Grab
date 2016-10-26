#include "sdk_utils.h"
#include "frmgrab.h"
#include <chrono>

using namespace std::chrono;

bool port_id(const std::string port, std::string & id)
{
    if (port != DVI_PORT_STR and port != SDI_PORT_STR)
    {
        id = "";
        return false;
    }

    FrmGrab_Init();

    int num_devices = FrmGrabLocal_Count();
    if (num_devices <= 0)
    {
        id = "";
        return false;
    }

    FrmGrabber ** fgs = (FrmGrabber **) malloc(sizeof(FrmGrabber*) * num_devices);

    FrmGrabLocal_OpenAll(fgs, num_devices);

    bool found = false;
    for (int i = 0; i < num_devices; i++)
    {
        std::string input_name(FrmGrab_GetInputName(fgs[i]));
        if (input_name.find(port) != std::string::npos)
        {
            id = std::string(FrmGrab_GetId(fgs[i]));
            found = true;
        }
        FrmGrab_Close(fgs[i]);
    }

    FrmGrab_Deinit();

    return found;
}

bool frame_rate(const std::string port,
                double & fr,
                const std::string colour_space)
{
    bool succeeded = false;
    fr = -1;

    std::string id;
    if (port_id(port, id))
    {
        FrmGrab_Init();

        FrmGrabber * fg = nullptr;
        fg = FrmGrabLocal_OpenSN(id.c_str());
        if (fg)
        {
            V2U_GrabFrame2* frame = nullptr;
            V2U_UINT32 frames = 0;

            high_resolution_clock::time_point started_at = high_resolution_clock::now();

            V2U_UINT32 flags = 0;
            if (colour_space == COLOUR_SPACE_I420)
                flags = V2U_GRABFRAME_FORMAT_I420;
            else if (colour_space == COLOUR_SPACE_BGR24)
                flags = V2U_GRABFRAME_FORMAT_BGR24;
            //    V2U_GRABFRAME_FORMAT(flags);

            if (flags)
            {
                if (FrmGrab_Start(fg))
                {
                    for (int i = 0; i < 300; i++)
                    {
                        // last param null => full frame
                        frame = FrmGrab_Frame(fg, flags, nullptr);

                        if (frame and frame->imagelen > 0)
                            frames++;

                        FrmGrab_Release(fg, frame);
                        frame = nullptr;
                    }

                    /* FrmGrab_Release ignores NULL argument */
                    FrmGrab_Release(fg, frame);
                    FrmGrab_Stop(fg);

                    if (frames >= 120) // for a stable measurement
                    {
                        duration<double> time_span =
                            duration_cast<duration<double>>(high_resolution_clock::now() - started_at);
                        fr = frames/time_span.count();
                        succeeded = true;
                    }
                }
            }
        }

        FrmGrab_Deinit();
    }

    return succeeded;
}
