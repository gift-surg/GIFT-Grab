System requirements
-------------------

Required:

* Linux (tested on Ubuntu 14.04 LTS)
* C++11
* [CMake](https://cmake.org/) (tested with 3.2)

Optional (depending on desired features, see "How to use" below):

* [OpenCV](http://www.opencv.org/) (tested with 2.4.12)
* [FFmpeg](https://ffmpeg.org/) (tested with 3.0.1)
* [NVENC (NVIDIA Video Codec SDK)](https://developer.nvidia.com/nvidia-video-codec-sdk) (tested with 6.0.1, **note that NVENC has a proprietary licence, so "non-free" components need to be enabled - see below - when building GIFT-Grab with NVENC support**)
* [A GPU supporting NVENC](https://developer.nvidia.com/nvidia-video-codec-sdk)
* [kvazaar](https://github.com/ultravideo/kvazaar) (tested with 0.8.3)
* [x265](http://x265.org/) (tested with 1.9, **note that x265 is a GPL library, so GPL needs to be enabled - see below - when building GIFT-Grab with x265 support**)
* [libvpx](https://www.webmproject.org/code/) (tested with 1.3.0)
* [libVLC (VLC SDK)](https://wiki.videolan.org/LibVLC/) (tested with 3.0.0 [nighly build ID: 20160913-0237](http://nightlies.videolan.org/build/source/?C=M;O=D))
* [Epiphan video grabber SDK](https://www.epiphan.com/support/) (tested with 3.30.3.0007, **note that Epiphan SDK has a proprietary licence, so "non-free" components need to be enabled - see below - when building GIFT-Grab with Epiphan SDK**)
* [Blackmagic Desktop Video SDK](https://www.blackmagicdesign.com/support) (tested with 10.4, **note that Blackmagic Desktop Video SDK has a proprietary licence, so "non-free" components need to be enabled - see below - when building GIFT-Grab with support for Blackmagic cards**)
* [Python](https://www.python.org/) (tested with 2.7)
* [Boost.Python](http://www.boost.org/doc/libs/release/libs/python/) (tested with 1.54.0, and 1.63.0 beta 1 for the NumPy support)
* [pkg-config](https://www.freedesktop.org/wiki/Software/pkg-config/)
* [pytest](http://doc.pytest.org/en/latest/)
* [Catch](https://github.com/philsquared/Catch) (tested with 1.5.8)
* [zlib](http://www.zlib.net/)
* [POSIX threads (pthreads)](http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/pthread.h.html)

Please note that there are cross-dependencies between some of these external libraries. Check out [our tips and tricks](doc/tips.md) for details.


