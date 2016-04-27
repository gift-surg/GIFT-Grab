# Summary

An umbrella library that provides an easy-to-use C++ (and optionally Python) API to stream and capture video using frame grabber hardware.

# Supported hardware

## Current

* [Epiphan DVI2PCIe Duo](http://www.epiphan.com/products/dvi2pcie-duo/)

## Future

There are bits and pieces in the current codebase pertaining to some of the following, but we do not support them yet. Support might be added in the future for these and possibly other hardware in case need arises.

* [BlackMagic DeckLink SDI 4K](https://www.blackmagicdesign.com/uk/products/decklink/techspecs/W-DLK-11)
* [BlackMagic DeckLink Quad](https://www.blackmagicdesign.com/uk/products/decklink/techspecs/W-DLK-02)
* [Epiphan Pearl](http://www.epiphan.com/products/pearl/)

# Supported formats and codecs for saving video streams

* [XviD](https://www.xvid.com/)-encoded video saved in an [AVI](https://msdn.microsoft.com/en-us/library/windows/desktop/dd318189(v=vs.85).aspx) container
* [HEVC/H.265](http://www.itu.int/ITU-T/recommendations/rec.aspx?rec=11885)-encoded video saved in an [MP4](http://www.iso.org/iso/catalogue_detail.htm?csnumber=38538) container

# Pre-requisites

## Required

1. Linux (tested on Ubuntu 14.04 LTS)
1. CMake 3.2+
1. C++11
1. OpenCV (tested with 2.4.12)

## Optional

* For HEVC/H.265 video-saving support:

  1. x265
  1. FFmpeg (__Make sure your OpenCV links against your custom-built FFmpeg -- see relevant information in advanced options below__)

* For Python API:

  1. Python 2.7
  1. Boost.Python

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
* `-D BUILD_TESTS=ON` for building the available test applications.
* `-D USE_EPIPHAN=ON` for building support for [Epiphan DVI2PCIe Duo framegrabber](http://www.epiphan.com/products/dvi2pcie-duo) (currently at least this option or `USE_FILES` needs to be specified for the library to build properly). __Note when using this option:__
   1. `/dev/video0` and `/dev/video1` will be probed for connecting to the DVI and SDI ports respectively.
   1. Due to the use of the generic [OpenCV VideoCapture](http://docs.opencv.org/2.4/modules/highgui/doc/reading_and_writing_images_and_video.html#VideoCapture::VideoCapture%28int%20device%29) API for grabbing frames from Epiphan cards, it is currently not possible to distinguish Epiphan framegrabbers from others.
* `-D USE_FFMPEG=ON` for building support for saving frames as video files using [FFmpeg](https://www.ffmpeg.org/). __Note when using this option:__
   1. Build and install [x265](http://x265.org/):
      1. `hg clone https://bitbucket.org/multicoreware/x265`
      1. `cd x265`
      1. `hg checkout 1.9`
      1. `cd ..` and `mkdir x265-build` and `cd x265-build`
      1. `cmake -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX=/your/x265/installation/location -D ENABLE_SHARED:bool=on ../x265/source/`
      1. `make -j` and `make install`
   1. Build and install [FFmpeg](https://www.ffmpeg.org/):
      1. `git clone https://github.com/FFmpeg/FFmpeg.git`
      1. `git checkout n3.0.1`
      1. `export PKG_CONFIG_PATH="/your/x265/installation/location/lib/pkgconfig:$PKG_CONFIG_PATH"`
      1. `mkdir ffmpeg-build` and `cd ffmpeg-build`
      1. `../ffmpeg/configure --prefix=/your/ffmpeg/installation/location --enable-shared --enable-avresample --enable-libx265 --enable-gpl --enable-muxer=mp4`
      1. `make -j` and `make install`
   1. Supply `-D FFmpeg_DIR=/your/ffmpeg/installation/location` to CMake.
* `-D BUILD_PYTHON=ON` for building the Python API.

# Use
1. Put `FIND_PACKAGE(GiftGrab CONFIG REQUIRED)` into your project's CMake file.
1. Specify `GiftGrab_DIR` as `/usr/local/include/giftgrab` for CMake.
1. See `src/cmake/GiftGrabConfig.cmake` (installed in `/usr/local/include/giftgrab/GiftGrabConfig.cmake`) to see which CMake variables to use.
