# Building GIFT-Grab from source

**Hint:** In case of any errors while building GIFT-Grab, please see the [known issues and limitations][gg-issues].

1. Configure using CMake and the `src` sub-directory of the cloned repository as the CMake source directory: `cmake <GIFT-Grab-clone>/src`
    * All GIFT-Grab features are inactive by default.
    * Use the below-listed options to choose features to install.
    * The list below indicates the requirements for each option.
1. Run the build process: `make -j`
1. Run the install process: `make install`

Please also note that **due to incompatibilities in the relevant dependencies' licences, options requiring proprietary libraries are mutually exclusive with options requiring GPL libraries.**
These are marked with **"makes GIFT-Grab undistributable"** and **"upgrades GIFT-Grab licence to GPL"** respectively below.
If mutually exclusive options are activated, CMake will give an error and GIFT-Grab will not be built.

Please also note that [GIFT-Grab has been tested only against specific versions of its dependencies][gg-reqs].

[gg-reqs]: doc/requirements.md
[gg-issues]: doc/issues.md


# Reading video files

`-D USE_FILES=ON` (requires [FFmpeg](tips.md#ffmpeg))


# Writing video files

## Xvid

`-D USE_XVID=ON` (requires [OpenCV](tips.md#opencv))


## HEVC

`-D USE_HEVC=ON` (requires [FFmpeg](tips.md#ffmpeg), pkg-config and [kvazaar](tips.md#kvazaar))

In addition, also the following two **BSD-3-incompatible** options are available:

1. `-D USE_HEVC=ON -D ENABLE_GPL=ON -D USE_X265=ON`: use x265 instead of kvazaar (requires [FFmpeg](tips.md#ffmpeg), pkg-config and [x265](tips.md#x265)), **upgrades GIFT-Grab licence to GPL**
2. `-D USE_HEVC=ON -D ENABLE_NONFREE=ON -D USE_NVENC=ON`: hardware-accelerated HEVC (requires [a supported GPU](https://developer.nvidia.com/nvidia-video-codec-sdk), [FFmpeg](tips.md#ffmpeg), pkg-config and [NVENC](tips.md#nvenc)), **makes GIFT-Grab undistributable**

## VP9

`-D USE_VP9=ON` (requires [FFmpeg](tips.md#ffmpeg), pkg-config and [libvpx](tips.md#libvpx))


# Epiphan DVI2PCIe Duo

`-D USE_EPIPHAN_DVI2PCIE_DUO=ON` (requires [OpenCV](tips.md#opencv) and [libVLC](tips.md#libvlc))

OpenCV and libVLC are needed for capturing in the BGRA and [I420][i420] colour spaces respectively.
Both options are active by default.
Append `-D USE_BGRA=OFF` or `-D USE_I420=OFF` to deactivate the respective option.

Also Epiphan Video Grabber SDK can be used instead of libVLC:
`-D USE_EPIPHAN_DVI2PCIE_DUO=ON -D ENABLE_NONFREE=ON -D USE_EPIPHANSDK=ON` (requires zlib, pthreads and [Epiphan Video Grabber SDK](tips.md#epiphan-video-grabbing-sdk)).
This option **makes GIFT-Grab undistributable**.

[i420]: https://wiki.videolan.org/YUV/#I420


# Blackmagic DeckLink SDI 4K

`-D USE_BLACKMAGIC_DECKLINK_SDI_4K=ON -D ENABLE_NONFREE=ON` (requires [Blackmagic Desktop Video SDK](tips.md#blackmagic-drivers-and-blackmagic-desktop-video-sdk))

This currently works only with the [UYVY][uyvy] colour space (active by default).
This option **makes GIFT-Grab undistributable**.

[uyvy]: https://wiki.videolan.org/YUV#UYVY


# Network streams

`-D USE_NETWORK_SOURCES=ON` (requires [OpenCV](tips.md#opencv) and [libVLC](tips.md#libvlc))

OpenCV and libVLC are needed for capturing in the BGRA and [I420][i420] colour spaces respectively.
Both options are active by default.
Append `-D USE_BGRA=OFF` or `-D USE_I420=OFF` to deactivate the respective option.


# Python API

Depending on whether [NumPy][numpy] support is required:

* Without NumPy: `-D BUILD_PYTHON=ON` (requires Python and [Boost.Python][bp])
* With NumPy: `-D BUILD_PYTHON=ON -D USE_NUMPY=ON` (requires Python and [Boost.Python 1.63 or newer](tips.md#numpy-support-and-boostpython))

[numpy]: http://www.numpy.org/
[bp]: http://www.boost.org/doc/libs/release/libs/python/doc/


# Testing GIFT-Grab

`-D BUILD_TESTS=ON -D BUILD_PYTHON=ON` (requires Python, [Boost.Python][bp], [Catch][catch] and [pytest][pytest]).
Note that Catch will be automatically downloaded from the internet as part of the build process.

If you would like to test network streams as well, you will also need to provide:
1. `-D TESTING_NETWORK_SOURCE_ADDRESS=`[`<mrl_of_your_network_source>`](https://wiki.videolan.org/Media_resource_locator)
1. `-D TESTING_NETWORK_SOURCE_FRAME_RATE=<frame_rate_you_want_to_test_against>`
1. (optional) `-D TESTING_NETWORK_SOURCE_DELAY=<initialisation_time_of_your_network_source_in_seconds>` (e.g. current GIFT-Grab I420 network source support needs typically 5 to 10 sec., floating point values can be specified as well). If not set, this parameter is assumed to be 0.0 sec.

To execute the tests run `ctest` or `make test` in the build directory.

[catch]: https://github.com/philsquared/Catch
[pytest]: http://doc.pytest.org/

# GIFT-Grab discovery

Including `FIND_PACKAGE(GiftGrab)` in your project's CMake file will automatically discover the `GiftGrab_INCLUDE_DIRS` and `GiftGrab_LIBS` CMake variables.
These are respectively for GIFT-Grab header directories to include and GIFT-Grab libraries to link against.

