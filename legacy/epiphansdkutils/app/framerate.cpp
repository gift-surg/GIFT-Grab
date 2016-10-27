#include "sdk_utils.h"
#include <iostream>

//!
//! \brief Parameters to be given to \c
//! frame_rate function when invoked as a \c
//! pthread
//!
struct PthreadParams {
    std::string port;
    double frame_rate;
    std::string colour_space;
};

//!
//! \brief Convenience function for calling \c
//! frame_rate to be invoked as a \c pthread
//! \param args \c PthreadParams
//! \return
//!
void * call_frame_rate(void * args)
{
    struct PthreadParams * params = (struct PthreadParams *) args;
    bool * succeeded = new bool;
    *succeeded = frame_rate(params->port, params->frame_rate, params->colour_space);
    return succeeded;
}

//!
//! \brief Print maximum frame-rate that can be
//! achieved when grabbing from requested
//! port(s)
//! \param argc
//! \param argv \c argv[1] must be either "DVI",
//! "SDI", or "DVI+SDI"
//! \return
//!
int main(int argc, char ** argv)
{
    if (argc <= 1)
        return EXIT_FAILURE;

    double fr = 0;
    std::string port(argv[1]);
    if (port != SDI_PORT_STR and port != DVI_PORT_STR
        and port != DUAL_PORT_STR)
        return EXIT_FAILURE;

    std::string colour_space = COLOUR_SPACE_I420;
    if (argc >= 3)
        colour_space = std::string(argv[2]);

    if (port == DUAL_PORT_STR)
    {
        struct PthreadParams dvi_params;
        dvi_params.port = DVI_PORT_STR;
        dvi_params.colour_space = colour_space;
        struct PthreadParams sdi_params;
        sdi_params.port = SDI_PORT_STR;
        sdi_params.colour_space = colour_space;

        pthread_t dvi_thread;
        int rc;
        rc = pthread_create(&dvi_thread, nullptr, &call_frame_rate, &dvi_params);
        if (rc != 0) return EXIT_FAILURE;

        pthread_t sdi_thread;
        rc = pthread_create(&sdi_thread, nullptr, &call_frame_rate, &sdi_params);
        void * ret = nullptr;
        if (rc != 0)
        {
            pthread_join(dvi_thread, &ret);
            delete (char *) ret;
            return EXIT_FAILURE;
        }

        pthread_join(dvi_thread, &ret);
        bool succeeded = *((bool *) ret);
        delete (char *) ret;

        pthread_join(sdi_thread, &ret);
        succeeded &= *((bool *) ret);
        delete (char *) ret;

        // take the more conservative frame rate
        fr = std::min(dvi_params.frame_rate, sdi_params.frame_rate);

        if (not succeeded) return EXIT_FAILURE;
    }
    else
    {
        if (not frame_rate(port, fr, colour_space))
            return EXIT_FAILURE;
    }

    std::cout << fr;
    return EXIT_SUCCESS;
}
