#pragma once

namespace gg
{

//!
//! \brief
//! \sa Device
//!
static const size_t NUM_DEVICES = 2;

//!
//! \brief Lists supported framegrabber hardware
//!
enum Device
{
    DVI2PCIeDuo_DVI
    , DVI2PCIeDuo_SDI
    , DeckLinkSDI4K
};

}
