#include "sdk_utils.h"
#include <iostream>

//!
//! \brief Print ID of all requested device port
//! on the system
//! \param argc
//! \param argv \c argv[1] must be either "DVI"
//! or "SDI"
//! \return \c EXIT_SUCCESS on success, \c
//! EXIT_FAILURE on failure
//!
int main(int argc, char ** argv)
{
    if (argc <= 1)
        return EXIT_FAILURE;

    std::string id = "";
    if (port_id(argv[1], id))
    {
        std::cout << id;
        return EXIT_SUCCESS;
    }

    return EXIT_FAILURE;
}
