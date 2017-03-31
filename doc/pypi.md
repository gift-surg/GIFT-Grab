# Install GIFT-Grab from the Python Packaging Index

**Hint:** In case of any errors (such as `option not recognized`) during installation, please see the [these tips](tips.md#gift-grab-caveats).

Simply run `pip install gift-grab` by appending the options for the desired options from the list below.
All GIFT-Grab features are inactive by default.

As this installation option installs the GIFT-Grab Python API, it requires Python and Boost.Python.

Please note that **due to incompatibilities in dependency licences, options requiring proprietary libraries are mutually exclusive with options requiring GPL libraries.**
These are marked with **"makes GIFT-Grab undistributable"** and **"upgrades GIFT-Grab licence to GPL"** respectively below.
If mutually exclusive options are activated, installation will fail.


# Reading video files

`--install-option="--files"` (requires [FFmpeg](tips.md#ffmpeg))


# Writing video files

## Xvid

`--install-option="--xvid"` (requires [OpenCV](tips.md#opencv))

## HEVC

`--install-option="--hevc"` (requires [FFmpeg](tips.md#ffmpeg), pkg-config and [kvazaar](tips.md#kvazaar))

In addition, also the following two **BSD-3-incompatible** options are available:

1. `--install-option="--hevc" --install-option="--enable-gpl" --install-option="--x265"`: use x265 instead of kvazaar (requires [FFmpeg](tips.md#ffmpeg), pkg-config and [x265](tips.md#x265)), **upgrades GIFT-Grab licence to GPL**
2. `--install-option="--hevc" --install-option="--enable-nonfree" --install-option="--nvenc"`: hardware-accelerated HEVC (requires a supported GPU, [FFmpeg](tips.md#ffmpeg), pkg-config and [NVENC](tips.md#nvenc)), **makes GIFT-Grab undistributable**

## VP9

`--install-option="--vp9"` (requires [FFmpeg](tips.md#ffmpeg), pkg-config and [libvpx](tips.md#libvpx))


# Epiphan DVI2PCIe Duo

`--install-option="--epiphan-dvi2pcie-duo"` (requires [OpenCV](tips.md#opencv) and [libVLC](tips.md#libvlc))

OpenCV and libVLC are needed for capturing in the BGRA and [I420][i420] colour spaces respectively.
Both options are active by default.
Append `--install-option="--no-bgra"` or `--install-option="--no-i420"` to deactivate the respective option.

Also Epiphan Video Grabber SDK can be used instead of libVLC:
`--install-option="--epiphan-dvi2pcie-duo" --install-option="--enable-nonfree" --install-option="--epiphansdk"` (requires zlib, pthreads and [Epiphan Video Grabber SDK](tips.md#epiphan-video-grabbing-sdk)).
This option **makes GIFT-Grab undistributable**.

[i420]: https://wiki.videolan.org/YUV/#I420


# Blackmagic DeckLink SDI 4K

`--install-option="--blackmagic-decklink-sdi-4k" --install-option="--enable-nonfree"` (requires [Blackmagic Desktop Video SDK](tips.md#blackmagic-drivers-and-blackmagic-desktop-video-sdk))

This currently works only with the [UYVY][uyvy] colour space (active by default).
This option **makes GIFT-Grab undistributable**.

[uyvy]: https://wiki.videolan.org/YUV#UYVY


# Network streams

`--install-option="--network-sources"` (requires [OpenCV](tips.md#opencv) and [libVLC](tips.md#libvlc))

OpenCV and libVLC are needed for capturing in the BGRA and [I420][i420] colour spaces respectively.
Both options are active by default.
Append `--install-option="--no-bgra"` or `--install-option="--no-i420"` to deactivate the respective option.


# NumPy

`--install-option="--numpy"` (requires Python and [Boost.Python 1.63 or newer](tips.md#numpy-support-and-boostpython))


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
