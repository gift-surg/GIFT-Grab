# GIFT-Grab

An open-source C++ and Python API for acquiring, processing and encoding video streams in real time.
GIFT-Grab supports several frame-grabber hardware, standard-compliant network streams and video files.
The Python API is compatible with [NumPy][numpy] and [SciPy][scipy].

GIFT-Grab was developed as part of the [GIFT-Surg][giftsurg] project at the [Translational Imaging Group][tig] in the [Centre for Medical Image Computing][cmic] at [University College London (UCL)][ucl].

[scipy]: https://www.scipy.org/
[numpy]: http://www.numpy.org/


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

Supported video sources
-----------------------

* [Epiphan DVI2PCIe Duo](http://www.epiphan.com/products/dvi2pcie-duo/)
* [Blackmagic DeckLink SDI 4K](https://www.blackmagicdesign.com/products/decklink/models)
* Network streams

Supported video formats
-----------------------

* [Xvid](https://www.xvid.com/) (saved as [AVI](https://msdn.microsoft.com/en-us/library/windows/desktop/dd318189(v=vs.85).aspx))
* [HEVC (H.265)](http://www.itu.int/ITU-T/recommendations/rec.aspx?rec=11885) (saved as [MP4](http://www.iso.org/iso/catalogue_detail.htm?csnumber=38538))
* [VP9](http://www.webmproject.org/vp9/) (saved as [WebM](https://www.webmproject.org/users/))

How to use
----------

GIFT-Grab can be either built from source (for the C++ API) **or** installed via the Python Packaging Index (for the Python API). For either of these options, check out [our tips and tricks](doc/tips.md) for troubleshooting and how to obtain the external dependencies.

Features can be customised as per the combinations listed below. Please note that these are the tested combinations. Other combinations might result in undefined behaviour.

**To install from the Python Packaging Index** simply run `pip install giftgrab`. Note that all features are disabled by default. Enable them by appending the following to the command:

* `--install-option="--files"` for file reading support (requires FFmpeg).
* `--install-option="--epiphan-dvi2pcie-duo"` for Epiphan DVI2PCIe Duo support (requires OpenCV and libVLC). OpenCV and libVLC are needed for capturing in the BGRA and [I420](https://wiki.videolan.org/YUV/#I420) colour spaces respectively. Both options are active by default. Append `--install-option="--no-bgra"` or `--install-option="--no-i420"` to deactivate the respective option.
* `--install-option="--epiphan-dvi2pcie-duo" --install-option="--enable-nonfree" --install-option="--epiphansdk"` for Epiphan DVI2PCIe Duo support with Epiphan SDK (requires zlib, pthreads and Epiphan video grabber SDK - see [our tips and tricks](doc/tips.md) on how to install this). This currently works only with the I420 colour space capture (active by default).
* `--install-option="--blackmagic-decklink-sdi-4k" --install-option="--enable-nonfree"` for Blackmagic DeckLink SDI 4K support (requires Blackmagic Desktop Video SDK). This currently works only with the [UYVY](https://wiki.videolan.org/YUV#UYVY) colour space (active by default).
* `--install-option="--network-sources"` to activate support for network (e.g. RTP) sources (requires OpenCV and libVLC). OpenCV and libVLC are needed for capturing in the BGRA and [I420](https://wiki.videolan.org/YUV/#I420) colour spaces respectively. Both options are active by default. Append `--install-option="--no-bgra"` or `--install-option="--no-i420"` to deactivate the respective option.
* `--install-option="--xvid"` for Xvid support (requires OpenCV).
* `--install-option="--hevc"` for HEVC support (requires FFmpeg, pkg-config and kvazaar).
* `--install-option="--hevc" --install-option="--enable-gpl" --install-option="--x265"` to use x265 instead of kvazaar for HEVC support (requires FFmpeg, pkg-config and x265).
* `--install-option="--hevc" --install-option="--enable-nonfree" --install-option="--nvenc"` for hardware-accelerated HEVC support (requires a supported GPU, FFmpeg, pkg-config and NVENC).
* `--install-option="--vp9"` for VP9 support (requires FFmpeg, pkg-config and libvpx).
* `--install-option="--numpy"` for [NumPy](http://www.numpy.org/) support (**experimental, see the [tips and tricks](doc/tips.md) section**, requires [Boost.Python 1.63](http://www.boost.org/doc/libs/1_63_0_b1/libs/python/doc/html/numpy/index.html) or newer).

**To quickly test whether the GIFT-Grab Python API works** run:

* `test-giftgrab-file-<codec>-<colour_space>` to test file reading support for `<codec>` (one of `xvid`, `hevc`, or `vp9`) with `<colour_space>` (one of `bgra`, `i420` or `uyvy`), provided GIFT-Grab was built with file reading support.
* `test-giftgrab-<codec>-<colour_space>` to test video encoding support for `<codec>` (one of `xvid`, `hevc`, or `vp9`) in `<colour_space>` (one of `bgra` or `i420`), provided GIFT-Grab was built with support for specified option (Note that currently GIFT-Grab does not support Xvid encoding with I420 or UYVY frames).
* `test-giftgrab-epiphan-dvi2pcieduo-i420` to test Epiphan DVI2PCIe Duo support
* `edd-<port>-i420-<codec>` (e.g. `edd-dvi-i420-vp9`) to record 30 frames into a timestamp-named video file, where:
   * `<port>` is one of `dvi` or `sdi`
   * `<codec>` is one of `xvid`, `hevc`, or `vp9`
* `test-giftgrab-blackmagic-decklinksdi4k-uyvy` to test Blackmagic DeckLink SDI 4K support
* `test-giftgrab-numpy-<colour_space>` to test NumPy support, provided GIFT-Grab was built with it (requires NumPy, obviously).
* `test-giftgrab-network-sources-<colour_space>` to test support for network sources, provided GIFT-Grab was built with it. This requires the following environment variables to be set:
   * `TESTING_NETWORK_SOURCE_ADDRESS`: the [MRL of your network source](https://wiki.videolan.org/Media_resource_locator) for testing
   * `TESTING_NETWORK_SOURCE_FRAME_RATE`: the frame rate you want to test against
   * `TESTING_NETWORK_SOURCE_DELAY`: the initialisation time of your network source in seconds (e.g. current GIFT-Grab I420 network source support needs typically 5 to 10 sec., floating point values can be specified as well). This is optional and if not set, assumed to be 0.0 sec.

Funding
-------

This work was supported through an Innovative Engineering for Health award by the [Wellcome Trust][wellcometrust] [WT101957], the [Engineering and Physical Sciences Research Council (EPSRC)][epsrc] [NS/A000027/1] and a [National Institute for Health Research][nihr] Biomedical Research Centre [UCLH][uclh]/UCL High Impact Initiative.


[tig]: http://cmictig.cs.ucl.ac.uk
[giftsurg]: http://www.gift-surg.ac.uk
[cmic]: http://cmic.cs.ucl.ac.uk
[ucl]: http://www.ucl.ac.uk
[nihr]: http://www.nihr.ac.uk/research
[uclh]: http://www.uclh.nhs.uk
[epsrc]: http://www.epsrc.ac.uk
[wellcometrust]: http://www.wellcome.ac.uk
