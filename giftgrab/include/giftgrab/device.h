#pragma once

namespace gg
{

//!
//! \brief Lists supported framegrabber hardware
//!
enum Device
{
    DVI2PCIeDuo_DVI
    , DVI2PCIeDuo_SDI
    , DeckLinkSDI4K
    , DeckLink4KExtreme12G
    // for housekeeping in VideoSourceFactory
    , COUNT
};

}
