#pragma once
#include <string>

#define DVI_PORT_STR "DVI"
#define SDI_PORT_STR "SDI"
#define DUAL_PORT_STR "DVI+SDI"
#define COLOUR_SPACE_I420 "I420"
#define COLOUR_SPACE_BGR24 "BGR24"

//!
//! \brief Try to find out ID under which \c port
//! is registered
//! \param port either "DVI" or "SDI"
//! \param id \c port's ID, or empty string if
//! device not found
//! \return \c true if \c port found, \c false
//! otherwise
//!
bool port_id(const std::string port, std::string & id);

//!
//! \brief Try to estimate frame rate of port with
//! \c id
//! \param port "DVI" or "SDI"
//! \param fr invalid (i.e. negative) value if
//! estimation fails
//! \param colour_space "I420" or "BGR24"
//! \return \c true on success, \c false on failure
//! (e.g. port cannot be opened, IO error)
//!
bool frame_rate(const std::string port,
                double & fr,
                const std::string colour_space = "I420");
