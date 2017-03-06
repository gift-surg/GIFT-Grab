# GIFT-Grab

An open-source C++ and Python API for acquiring, processing and encoding video streams in real time.
GIFT-Grab supports several frame-grabber hardware, standard-compliant network streams and video files.
The Python API is compatible with [NumPy][numpy] and [SciPy][scipy].

GIFT-Grab was developed as part of the [GIFT-Surg][giftsurg] project at the [Translational Imaging Group][tig] in the [Centre for Medical Image Computing][cmic] at [University College London (UCL)][ucl].

[scipy]: https://www.scipy.org/
[numpy]: http://www.numpy.org/


# Installation

GIFT-Grab can be installed by [building from source][build-from-source] or from the Python Package Index [using `pip`][pip].
Please see the features section below for quick links to the relevant installation documentation.

[build-from-source]: doc/build.md
[pip]: doc/pypi.md


# Features

The table below summarises GIFT-Grab features.
It also provides shortcuts to installation HOWTOs.

| **Feature** | **C++ API howto** | **Python API howto** |
| --- | :---: | :---: |
| **Frame-grabbers** | | |
| [Epiphan DVI2PCIe Duo][e-dd] | **TODO** |  **TODO** |
| [Blackmagic DeckLink SDI 4K][bm-dls4] | **TODO** |  **TODO** |
| **Network streams** | **TODO** | **TODO** |
| **Video files** | | |
| [Xvid][xvid]-encoded [AVI][avi] | **TODO** | **TODO** |
| [HEVC (H.265)][hevc]-encoded [MP4][mp4] | **TODO** | **TODO** |
| [VP9][vp9]-encoded [WebM][webm] | **TODO** | **TODO** |
| [NumPy][numpy] / [SciPy][scipy] compatibility |  | **TODO** |


[e-dd]: http://www.epiphan.com/products/dvi2pcie-duo/
[bm-dls4]: https://www.blackmagicdesign.com/products/decklink/models
[xvid]: https://www.xvid.com/
[avi]: https://msdn.microsoft.com/en-us/library/windows/desktop/dd318189(v=vs.85).aspx
[hevc]: http://www.itu.int/ITU-T/recommendations/rec.aspx?rec=11885
[mp4]: http://www.iso.org/iso/catalogue_detail.htm?csnumber=38538
[vp9]:http://www.webmproject.org/vp9/
[webm]: https://www.webmproject.org/users/


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


# Funding

This work was supported through an Innovative Engineering for Health award by the [Wellcome Trust][wellcometrust] [WT101957], the [Engineering and Physical Sciences Research Council (EPSRC)][epsrc] [NS/A000027/1] and a [National Institute for Health Research][nihr] Biomedical Research Centre [UCLH][uclh]/UCL High Impact Initiative.


[tig]: http://cmictig.cs.ucl.ac.uk
[giftsurg]: http://www.gift-surg.ac.uk
[cmic]: http://cmic.cs.ucl.ac.uk
[ucl]: http://www.ucl.ac.uk
[nihr]: http://www.nihr.ac.uk/research
[uclh]: http://www.uclh.nhs.uk
[epsrc]: http://www.epsrc.ac.uk
[wellcometrust]: http://www.wellcome.ac.uk
