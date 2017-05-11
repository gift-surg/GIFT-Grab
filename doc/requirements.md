# Operating system

GIFT-Grab has so far been tested only on Linux:
* Ubuntu 14.04 LTS, kernel: 3.19.0-59-generic
* Ubuntu 16.04 LTS, kernel: 4.4.0-72-generic (**experimental:** please see the [known issues](README.md#known-issues))


# Required dependencies

GIFT-Grab requires:

* A C++ compiler supporting C++11 (tested with GCC 4.9.3)
* [CMake](https://cmake.org/) (tested with 3.2)


# Optional dependencies

GIFT-Grab requires a subset of the following libraries **based on activated [features](../README.md#features)**.
Please note that there are cross-dependencies between some of the optional dependencies.
Check out [the documentation](tips.md) for details.

| **Dependency** | **GIFT-Grab tested with version / model** | **Remarks** |
| :--- | :--- | :--- |
| [OpenCV](http://www.opencv.org/) | 2.4.12 (Ubuntu 14.04 LTS), 3.2.0 (Ubuntu 16.04 LTS) | Each listed version has been tested only on the platform within the respective parantheses. |
| [FFmpeg](https://ffmpeg.org/) | 3.1.2 | |
| [NVENC (NVIDIA Video Codec SDK)](https://developer.nvidia.com/nvidia-video-codec-sdk) | 6.0.1 | NVENC has a proprietary licence: enabling NVENC support makes GIFT-Grab undistributable. |
| [A GPU supporting NVENC](https://developer.nvidia.com/nvidia-video-codec-sdk) | GeForce GTX TITAN X and GeForce GTX 980 Ti | |
| [kvazaar](https://github.com/ultravideo/kvazaar) | 0.8.3 | |
| [x265](http://x265.org/) | 1.9 | x265 is a GPL library: enabling x265 support upgrades the GIFT-Grab licence to GPL. |
| [libvpx](https://www.webmproject.org/code/) | 1.3.0 | |
| [libVLC (VLC SDK)](https://wiki.videolan.org/LibVLC/) | 3.0.0 release candidate: [nighly build ID: 20160913-0237](http://nightlies.videolan.org/build/source/?C=M;O=D) | Please see the note in the [libVLC installation instructions](tips.md#libvlc) about using libVLC for capturing network streams. |
| [Epiphan Video Grabber SDK](https://www.epiphan.com/support/) | 3.30.3.0007 | Epiphan Video Grabber SDK has a proprietary licence: enabling Epiphan Video Grabber SDK makes GIFT-Grab undistributable. |
| [Blackmagic Desktop Video SDK](https://www.blackmagicdesign.com/support) | 10.4 | Blackmagic Desktop Video SDK has a proprietary licence: enabling support for Blackmagic cards makes GIFT-Grab undistributable. |
| [Python](https://www.python.org/) | 2.7 | |
| [Boost.Python](http://www.boost.org/doc/libs/release/libs/python/) | 1.54.0, and 1.63.0 beta 1 for NumPy support | |
| [pkg-config](https://www.freedesktop.org/wiki/Software/pkg-config/) | | |
| [pytest](http://doc.pytest.org/en/latest/) | | |
| [Catch](https://github.com/philsquared/Catch) | 1.5.8 | |
| [zlib](http://www.zlib.net/) | | |
| [POSIX threads (pthreads)](http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/pthread.h.html) | | |
