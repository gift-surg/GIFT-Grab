**Build GIFT-Grab from source** using CMake. Use the following options to customise features as desired (all options are disabled by default):

* `-D USE_FILES=ON` for file reading support (requires FFmpeg).
* `-D USE_EPIPHAN_DVI2PCIE_DUO=ON` for Epiphan DVI2PCIe Duo support (requires OpenCV and libVLC). OpenCV and libVLC are needed for capturing in the BGRA and [I420](https://wiki.videolan.org/YUV/#I420) colour spaces respectively. Both options are active by default. Append `-D USE_BGRA=OFF` or `-D USE_I420=OFF` to deactivate the respective option.
* `-D USE_EPIPHAN_DVI2PCIE_DUO=ON -D ENABLE_NONFREE=ON -D USE_EPIPHANSDK=ON` for Epiphan DVI2PCIe Duo support with Epiphan SDK (requires zlib, pthreads and Epiphan video grabber SDK - see [our tips and tricks](doc/tips.md) on how to install this). This currently works only with the I420 colour space capture (active by default).
* `-D USE_BLACKMAGIC_DECKLINK_SDI_4K=ON -D ENABLE_NONFREE=ON` for Blackmagic DeckLink SDI 4K support (requires Blackmagic Desktop Video SDK). This currently works only with the [UYVY](https://wiki.videolan.org/YUV#UYVY) colour space (active by default).
* `-D USE_NETWORK_SOURCES=ON` to activate support for network (e.g. RTP) sources (requires OpenCV and libVLC). OpenCV and libVLC are needed for capturing in the BGRA and [I420](https://wiki.videolan.org/YUV/#I420) colour spaces respectively. Both options are active by default. Append `-D USE_BGRA=OFF` or `-D USE_I420=OFF` to deactivate the respective option.
* `-D USE_XVID=ON` for Xvid support (requires OpenCV).
* `-D USE_HEVC=ON` for HEVC support (requires FFmpeg, pkg-config and kvazaar).
* `-D USE_HEVC=ON -D ENABLE_GPL=ON -D USE_X265=ON` to use x265 instead of kvazaar for HEVC support (requires FFmpeg, pkg-config and x265).
* `-D USE_HEVC=ON -D ENABLE_NONFREE=ON -D USE_NVENC=ON` for hardware-accelerated HEVC support (requires a supported GPU, FFmpeg, pkg-config and NVENC).
* `-D USE_VP9=ON` for VP9 support (requires FFmpeg, pkg-config and libvpx).
* `-D BUILD_PYTHON=ON` for GIFT-Grab Python API (requires Python and Boost.Python).
* `-D BUILD_PYTHON=ON -D USE_NUMPY=ON` for [NumPy](http://www.numpy.org/) support (**experimental, see the [tips and tricks](doc/tips.md) section**, requires Python and [Boost.Python 1.63](http://www.boost.org/doc/libs/1_63_0_b1/libs/python/doc/html/numpy/index.html) or newer).
* To quickly see whether GIFT-Grab works on your system, turn on tests with `-D BUILD_TESTS=ON -D BUILD_PYTHON=ON` (requires Python, Boost.Python, Catch [will be automatically downloaded from the internet as part of the build process] and pytest). If activating network sources, you will also need to provide `-D TESTING_NETWORK_SOURCE_ADDRESS=`[`<mrl_of_your_network_source>`](https://wiki.videolan.org/Media_resource_locator)`-D TESTING_NETWORK_SOURCE_FRAME_RATE=<frame_rate_you_want_to_test_against> -D TESTING_NETWORK_SOURCE_DELAY=<initialisation_time_of_your_network_source_in_seconds>` (e.g. current GIFT-Grab I420 network source support needs typically 5 to 10 sec., floating point values can be specified as well). The last parameter `TESTING_NETWORK_SOURCE_DELAY` is optional and if not set, assumed to be 0.0 sec. To execute the tests run `ctest` or `make test` in the build directory.

**To use the GIFT-Grab C++ API** in your software: include `FIND_PACKAGE(GiftGrab)` in your CMake file to discover the `GiftGrab_INCLUDE_DIRS` and `GiftGrab_LIBS` CMake variables (respectively for GIFT-Grab header directories to include and GIFT-Grab libraries to link against).

