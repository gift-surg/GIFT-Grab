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
    , NUM_DEVICES /* not the best practice, but
                     needed to be able to track
                     the number of elements in
                     VideoSourceFactory::_devices
                   */
};

}
