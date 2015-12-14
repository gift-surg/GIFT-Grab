# Summary

An umbrella library that provides an easy-to-use API to stream video using frame grabber hardware.

# Supported hardware

## Current

* [Epiphan DVI2PCIe Duo](http://www.epiphan.com/products/dvi2pcie-duo/)

## Future

There are bits and pieces in the current codebase pertaining to some of the following, but we do not support them yet. Support might be added in the future for these and possibly other hardware in case need arises.

* [BlackMagic DeckLink SDI 4K](https://www.blackmagicdesign.com/uk/products/decklink/techspecs/W-DLK-11)
* [BlackMagic DeckLink Quad](https://www.blackmagicdesign.com/uk/products/decklink/techspecs/W-DLK-02)
* [Epiphan Pearl](http://www.epiphan.com/products/pearl/)

# Pre-requisites

1. Linux (tested on Ubuntu 14.04 LTS)
1. CMake 3.2+
1. C++11
1. OpenCV (tested with 2.4.12)

# Build

## Basic

```
git clone <this-repo> GiftGrab
mkdir GiftGrab-build
cd GiftGrab-build
cmake -D USE_FILES=ON ../GiftGrab/src
make -j
sudo make install # CMAKE_INSTALL_PREFIX defaults to /usr/local
```

## Advanced

* `-D BUILD_DOC=ON` for building documentation.
* `-D BUILD_EPIPHAN=ON` for building support for Epiphan framegrabbers (currently at least this option or `USE_FILES` needs to be specified for the library to build properly). __Note when using this option:__
   1. `/dev/video0` to `/dev/video4` will be probed (in ascending order) to locate an Epiphan framegrabber, and the first one to be found active will be assumed to be the device.
   1. Due to the use of the generic [OpenCV VideoCapture](http://docs.opencv.org/2.4/modules/highgui/doc/reading_and_writing_images_and_video.html#VideoCapture::VideoCapture%28int%20device%29) API for grabbing frames from Epiphan cards, it is currently not possible to distinguish Epiphan framegrabbers from others.

# Use
1. Put `FIND_PACKAGE(GiftGrab CONFIG REQUIRED)` into your project's CMake file.
1. Specify `GiftGrab_DIR` as `/usr/local/include/giftgrab` for CMake.
1. See `src/cmake/GiftGrabConfig.cmake` (installed in `/usr/local/include/giftgrab/GiftGrabConfig.cmake`) to see which CMake variables to use.