# Install GIFT-Grab from the Python Packaging Index

Simply run `pip install giftgrab` by appending the options for the desired options from the list below.
All GIFT-Grab features are inactive by default.

As this installation option installs the GIFT-Grab Python API, it requires Python and Boost.Python.

Please note that **due to incompatibilities in dependency licences, options requiring proprietary libraries are mutually exclusive with options requiring GPL libraries.**
These are marked with **"makes GIFT-Grab undistributable"** and **"upgrades GIFT-Grab licence to GPL"** respectively below.
If mutually exclusive options are activated, installation will fail.


# Reading video files

`--install-option="--files"` (requires FFmpeg)


# Writing video files

## Xvid

`--install-option="--xvid"` (requires OpenCV)

## HEVC

`--install-option="--hevc"` (requires FFmpeg, pkg-config and kvazaar)

In addition, also the following two **BSD-3-incompatible** options are available:

1. `--install-option="--hevc" --install-option="--enable-gpl" --install-option="--x265"`: use x265 instead of kvazaar (requires FFmpeg, pkg-config and x265), **upgrades GIFT-Grab licence to GPL**
2. `--install-option="--hevc" --install-option="--enable-nonfree" --install-option="--nvenc"`: hardware-accelerated HEVC (requires a supported GPU, FFmpeg, pkg-config and NVENC), **makes GIFT-Grab undistributable**

## VP9

`--install-option="--vp9"` (requires FFmpeg, pkg-config and libvpx)


# Epiphan DVI2PCIe Duo

`--install-option="--epiphan-dvi2pcie-duo"` (requires OpenCV and libVLC)

OpenCV and libVLC are needed for capturing in the BGRA and [I420][i420] colour spaces respectively.
Both options are active by default.
Append `--install-option="--no-bgra"` or `--install-option="--no-i420"` to deactivate the respective option.

Also Epiphan Video Grabbing SDK can be used instead of libVLC:
`--install-option="--epiphan-dvi2pcie-duo" --install-option="--enable-nonfree" --install-option="--epiphansdk"` (requires zlib, pthreads and Epiphan video grabber SDK).
This option **makes GIFT-Grab undistributable**.

[i420]: https://wiki.videolan.org/YUV/#I420


# Blackmagic DeckLink SDI 4K

`--install-option="--blackmagic-decklink-sdi-4k" --install-option="--enable-nonfree"` (requires Blackmagic Desktop Video SDK)

This currently works only with the [UYVY][uyvy] colour space (active by default).
This option **makes GIFT-Grab undistributable**.

[uyvy]: https://wiki.videolan.org/YUV#UYVY


# Network streams

`--install-option="--network-sources"` (requires OpenCV and libVLC)

OpenCV and libVLC are needed for capturing in the BGRA and [I420][i420] colour spaces respectively.
Both options are active by default.
Append `--install-option="--no-bgra"` or `--install-option="--no-i420"` to deactivate the respective option.


# NumPy

`--install-option="--numpy"` (requires Python and [Boost.Python 1.63][bp163] or newer)

[bp163]: http://www.boost.org/doc/libs/1_63_0_b1/libs/python/doc/html/numpy/index.html


# Testing GIFT-Grab

Tests are built automatically when installing GIFT-Grab using `pip` (requires Python, Boost.Python, Catch and pytest).
Note that Catch will be automatically downloaded from the internet as part of the build process.
The subsections below list the available tests when GIFT-Grab is installed from the Python Package Index.

## Testing video file reading

Run `test-giftgrab-file-<codec>-<colour_space>` where `<codec>` is `xvid`, `hevc`, or `vp9`; `<colour_space>` is `bgra`, `i420` or `uyvy`.


## Testing video file writing

Run `test-giftgrab-<codec>-<colour_space>` where `<codec>` is `xvid`, `hevc`, or `vp9`; `<colour_space>` is `bgra` or `i420`.
Note that currently GIFT-Grab does not support Xvid encoding with [I420][i420] or [UYVY][uyvy] frames.


## Testing Epiphan DVI2PCIe Duo support

Run `test-giftgrab-epiphan-dvi2pcieduo-i420`.


## Testing Blackmagic DeckLink SDI 4K support

Run `test-giftgrab-blackmagic-decklinksdi4k-uyvy`.


## Testing NumPy support

Run `test-giftgrab-numpy-<colour_space>` (requires NumPy, obviously).


## Testing network stream support

Run `test-giftgrab-network-sources-<colour_space>`.
This requires the following environment variables to be set:
* `TESTING_NETWORK_SOURCE_ADDRESS`: the [MRL of your network source](https://wiki.videolan.org/Media_resource_locator) for testing
* `TESTING_NETWORK_SOURCE_FRAME_RATE`: the frame rate you want to test against
* (optional) `TESTING_NETWORK_SOURCE_DELAY`: the initialisation time of your network source in seconds (e.g. current GIFT-Grab I420 network source support needs typically 5 to 10 sec., floating point values can be specified as well). If not set, this parameter is assumed to be 0.0 sec.
