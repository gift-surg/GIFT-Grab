# Building GIFT-Grab from source

1. Configure using CMake and the `src` sub-directory of the cloned repository as the CMake source directory: `cmake <GIFT-Grab-clone>/src`
    * All GIFT-Grab features are inactive by default.
    * Use the below-listed options to choose features to install.
    * The list below indicates the requirements for each option.
1. Run the build process: `make -j`
1. Run the install process: `make install`

Please note that due to differences in dependency licences, some options are mutually exclusive.
In such a case CMake will give an error and GIFT-Grab will not be built.


# Reading video files

`-D USE_FILES=ON` (requires FFmpeg)


# Writing video files

## Xvid

`-D USE_XVID=ON` (requires OpenCV)


## HEVC

`-D USE_HEVC=ON` (requires FFmpeg, pkg-config and kvazaar)

In addition, also the following two **BSD-3-incompatible** options are available:

1. `-D USE_HEVC=ON -D ENABLE_GPL=ON -D USE_X265=ON`: use x265 instead of kvazaar (requires FFmpeg, pkg-config and x265), **upgrades GIFT-Grab licence to GPL**
2. `-D USE_HEVC=ON -D ENABLE_NONFREE=ON -D USE_NVENC=ON`: hardware-accelerated HEVC (requires a supported GPU, FFmpeg, pkg-config and NVENC), **makes GIFT-Grab undistributable**

## VP9

`-D USE_VP9=ON` (requires FFmpeg, pkg-config and libvpx)


# Epiphan DVI2PCIe Duo

`-D USE_EPIPHAN_DVI2PCIE_DUO=ON` (requires OpenCV and libVLC)

OpenCV and libVLC are needed for capturing in the BGRA and [I420][i420] colour spaces respectively.
Both options are active by default.
Append `-D USE_BGRA=OFF` or `-D USE_I420=OFF` to deactivate the respective option.

Also Epiphan Video Grabbing SDK can be used instead of libVLC:
`-D USE_EPIPHAN_DVI2PCIE_DUO=ON -D ENABLE_NONFREE=ON -D USE_EPIPHANSDK=ON` (requires zlib, pthreads and Epiphan Video Grabber SDK).
This option **makes GIFT-Grab undistributable**.

[i420]: https://wiki.videolan.org/YUV/#I420


# Blackmagic DeckLink SDI 4K

`-D USE_BLACKMAGIC_DECKLINK_SDI_4K=ON -D ENABLE_NONFREE=ON` (requires Blackmagic Desktop Video SDK)

This currently works only with the [UYVY][uyvy] colour space (active by default).

[uyvy]: https://wiki.videolan.org/YUV#UYVY


# Network streams

`-D USE_NETWORK_SOURCES=ON` (requires OpenCV and libVLC)

OpenCV and libVLC are needed for capturing in the BGRA and [I420][i420] colour spaces respectively.
Both options are active by default.
Append `-D USE_BGRA=OFF` or `-D USE_I420=OFF` to deactivate the respective option.


# Python API

Depending on whether [NumPy][numpy] support is required:

* Without NumPy: `-D BUILD_PYTHON=ON` (requires Python and Boost.Python)
* With NumPy: `-D BUILD_PYTHON=ON -D USE_NUMPY=ON` (requires Python and [Boost.Python 1.63][bp163] or newer)

[numpy]: http://www.numpy.org/
[bp163]: http://www.boost.org/doc/libs/1_63_0_b1/libs/python/doc/html/numpy/index.html


# Testing GIFT-Grab

`-D BUILD_TESTS=ON -D BUILD_PYTHON=ON` (requires Python, Boost.Python, Catch and pytest).
Note that Catch will be automatically downloaded from the internet as part of the build process.

If you would like to test network streams as well, you will also need to provide:
1. `-D TESTING_NETWORK_SOURCE_ADDRESS=`[`<mrl_of_your_network_source>`](https://wiki.videolan.org/Media_resource_locator)
1. `-D TESTING_NETWORK_SOURCE_FRAME_RATE=<frame_rate_you_want_to_test_against>`
1. (optional) `-D TESTING_NETWORK_SOURCE_DELAY=<initialisation_time_of_your_network_source_in_seconds>` (e.g. current GIFT-Grab I420 network source support needs typically 5 to 10 sec., floating point values can be specified as well). If not set, this parameter is assumed to be 0.0 sec.

To execute the tests run `ctest` or `make test` in the build directory.


# GIFT-Grab discovery

Including `FIND_PACKAGE(GiftGrab)` in your project's CMake file will automatically discover the `GiftGrab_INCLUDE_DIRS` and `GiftGrab_LIBS` CMake variables.
These are respectively for GIFT-Grab header directories to include and GIFT-Grab libraries to link against.

