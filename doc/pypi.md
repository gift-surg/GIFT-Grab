# Install GIFT-Grab from the Python Packaging Index

**Hint:** In case of any errors (such as `option not recognized`) during installation, please see the [known issues and limitations][gg-issues].

The subsequent sections of this page outline the GIFT-Grab features.
All features are disabled by default.
**Desired GIFT-Grab features must be enabled WHEN CALLING `pip install gift-grab`.**
Enabling a feature requires appending one or more [`pip --install-option`s][pip-install-ops] to `pip install gift-grab`.
For instance to install GIFT-Grab with support for reading video files and video encoding with HEVC:

```
pip install gift-grab --install-option="--files" --install-option="--hevc"
```

[gg-issues]: issues.md
[pip-install-ops]: https://pip.pypa.io/en/stable/reference/pip_install/#cmdoption-install-option

As this route installs the GIFT-Grab Python API, it requires Python and Boost.Python, **in addition to the dependencies of the desired GIFT-Grab features**.
The dependencies for each feature are listed alongside the required `pip` install option(s) below.

Please also note that **due to incompatibilities in the relevant dependencies' licences, options requiring proprietary libraries are mutually exclusive with options requiring GPL libraries.**
These are marked with **"makes GIFT-Grab undistributable"** and **"upgrades GIFT-Grab licence to GPL"** respectively below.
If mutually exclusive options are activated, installation will fail.

Please also note that [GIFT-Grab has been tested only against specific versions of its dependencies][gg-reqs].

[gg-reqs]: requirements.md


## Reading video files

`--install-option="--files"` (requires [FFmpeg](tips.md#ffmpeg))


## Writing video files

You can enable one or more of the codecs listed below for encoding video files.

### Xvid

`--install-option="--xvid"` (requires [OpenCV](tips.md#opencv))

### HEVC

`--install-option="--hevc"` (requires [FFmpeg](tips.md#ffmpeg), pkg-config and [kvazaar](tips.md#kvazaar))

In addition, also the following two **BSD-3-incompatible** options are available:

1. `--install-option="--hevc" --install-option="--enable-gpl" --install-option="--x265"`: use x265 instead of kvazaar (requires [FFmpeg](tips.md#ffmpeg), pkg-config and [x265](tips.md#x265)), **upgrades GIFT-Grab licence to GPL**
2. `--install-option="--hevc" --install-option="--enable-nonfree" --install-option="--nvenc"`: hardware-accelerated HEVC (requires a supported GPU, [FFmpeg](tips.md#ffmpeg), pkg-config and [NVENC](tips.md#nvenc)), **makes GIFT-Grab undistributable**

### VP9

`--install-option="--vp9"` (requires [FFmpeg](tips.md#ffmpeg), pkg-config and [libvpx](tips.md#libvpx))


## Epiphan DVI2PCIe Duo

`--install-option="--epiphan-dvi2pcie-duo"` (requires [OpenCV](tips.md#opencv) and [libVLC](tips.md#libvlc))

OpenCV and libVLC are needed for capturing in the BGRA and [I420][i420] colour spaces respectively.
Both options are active by default.
Append `--install-option="--no-bgra"` or `--install-option="--no-i420"` to deactivate the respective option.

Also Epiphan Video Grabber SDK can be used instead of libVLC:
`--install-option="--epiphan-dvi2pcie-duo" --install-option="--enable-nonfree" --install-option="--epiphansdk"` (requires zlib, pthreads and [Epiphan Video Grabber SDK](tips.md#epiphan-video-grabbing-sdk)).
This option **makes GIFT-Grab undistributable**.

[i420]: https://wiki.videolan.org/YUV/#I420


## Blackmagic DeckLink SDI 4K

`--install-option="--blackmagic-decklink-sdi-4k" --install-option="--enable-nonfree"` (requires [Blackmagic Desktop Video SDK](tips.md#blackmagic-drivers-and-blackmagic-desktop-video-sdk))

This currently works only with the [UYVY][uyvy] colour space (active by default).
This option **makes GIFT-Grab undistributable**.

[uyvy]: https://wiki.videolan.org/YUV#UYVY


## Network streams

`--install-option="--network-sources"` (requires [OpenCV](tips.md#opencv) and [libVLC](tips.md#libvlc))

OpenCV and libVLC are needed for capturing in the BGRA and [I420][i420] colour spaces respectively.
Both options are active by default.
Append `--install-option="--no-bgra"` or `--install-option="--no-i420"` to deactivate the respective option.


## NumPy

`--install-option="--numpy"` (requires Python and [Boost.Python 1.63 or newer](tips.md#numpy-support-and-boostpython))
