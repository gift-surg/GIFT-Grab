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
