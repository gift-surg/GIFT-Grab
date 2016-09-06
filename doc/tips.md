Below are troubleshooting tips and tricks, as well as detailed installation instructions for some GiftGrab dependencies. **These were tested only on an Ubuntu LTS 14.04.**

GiftGrab Caveats
----------------

* In case GiftGrab is built with a custom `CMAKE_INSTALL_PREFIX` that is **not** a standard system path, `${CMAKE_INSTALL_PREFIX}/lib/giftgrab` needs to be included in CMake's `CMAKE_MODULE_PATH` for `FIND_PACKAGE(GiftGrab)` to work.
* If you encounter an `option not recognized` error while `pip install`ling GiftGrab, run `pip install pytest` and `pip install PyYAML` before running `pip install giftgrab`.

Epiphan DVI2PCIe Duo
--------------------

* When using this frame-grabber card, `/dev/video0` and `/dev/video1` will be probed for connecting to its DVI and SDI ports respectively.
* The Linux user account used for capturing video must be in the group these two devices belong to. For instance if these belong to the `video` group: `sudo usermod -a -G video myuser`.
* Due to the use of the generic [OpenCV VideoCapture](http://docs.opencv.org/2.4/modules/highgui/doc/reading_and_writing_images_and_video.html#VideoCapture::VideoCapture%28int%20device%29) API, GiftGrab can currently not distinguish Epiphan frame-grabbers from others (i.e. in the case where multiple frame-grabber cards are installed on a system).
* Due to hardware-specific limitations, capturing video at 60 fps is possible only if the [I420 colour space](https://wiki.videolan.org/YUV#I420) is used instead of BGR24.

OpenCV
------

If using FFmpeg, make sure your OpenCV is properly linked against your FFmpeg installation.

FFmpeg
------

Installation instructions:

1. `git clone https://github.com/FFmpeg/FFmpeg.git`
1. `git checkout n3.0.1`
1. `mkdir ffmpeg-build` and `cd ffmpeg-build`
1. If using kvazaar: `../FFmpeg/configure --enable-shared --enable-avresample --enable-libkvazaar --enable-muxer=mp4`
1. If using NVENC: `../FFmpeg/configure --enable-shared --enable-avresample --enable-nvenc --enable-nonfree --enable-muxer=mp4`
1. If using libvpx: `../FFmpeg/configure --enable-shared --enable-avresample --enable-libvpx --enable-muxer=webm`
1. `make -j` and `make install`

**Note:** In case you install FFmpeg in a non-standard location, you will need to appropriately augment your `PKG_CONFIG_PATH` for GiftGrab to be able to locate FFmpeg.

NVENC
-----

Installation instructions:

1. Download the [SDK](https://developer.nvidia.com/video-sdk-601)
1. Extract its contents and copy `nvidia_video_sdk_6.0.1/Samples/common/inc/nvEncodeAPI.h` to a standard system include folder (e.g. `/usr/local/include`)

kvazaar
-------

Installation instructions:

1. `git clone https://github.com/ultravideo/kvazaar.git`
1. `cd kvazaar`
1. `git checkout v0.8.3`
1. `./autogen.sh`
1. `./configure`
1. `make -j` and `make install`

libvpx
------

Install by `apt-get install libvpx-dev`.
