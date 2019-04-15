# Dependency installation guidelines

In this document we aim to provide guidelines for installing the GIFT-Grab dependencies.
A full list of the required and optional dependencies is available in the [required tools and libraries](requirements.md) section.
If you encounter problems installing any dependency, please have a look at the [known issues and limitations][gg-issues].

[gg-issues]: issues.md


## Epiphan DVI2PCIe Duo

* When using this frame-grabber card, `/dev/video0` and `/dev/video1` will be probed for connecting to its DVI and SDI ports respectively.
* The Linux user account used for capturing video must be in the group these two devices belong to. For instance if these belong to the `video` group: `sudo usermod -a -G video myuser`.
* Due to the use of the generic [OpenCV VideoCapture](http://docs.opencv.org/2.4/modules/highgui/doc/reading_and_writing_images_and_video.html#VideoCapture::VideoCapture%28int%20device%29) API, GIFT-Grab can currently not distinguish Epiphan frame-grabbers from others (i.e. in the case where multiple frame-grabber cards are installed on a system).
* Due to hardware-specific limitations, capturing video at 60 fps is possible only if the [I420 colour space](https://wiki.videolan.org/YUV#I420) is used instead of BGRA.


## Epiphan Video Grabbing SDK

These instructions are provided for convenience only.
Always check the manufacturer's manuals before proceeding.

1. Download Epiphan video grabbing SDK from [Epiphan support](https://www.epiphan.com/support/) and unpack it, e.g. `wget https://www.epiphan.com/downloads/products/epiphan_sdk-3.30.3.0007.zip; unzip epiphan_sdk-3.30.3.0007.zip`.
1. Change into the sub-folder `epiphan/samples/v2u` and run `make`. This should create a `build` folder here.
1. (Optional) Move the top folder (i.e. the one resulting from unpacking the archive) to a system folder, e.g. `/opt`, for easy access by all users.
1. Specify this folder's **absolute** path as the `EpiphanSDK_DIR` environment variable, e.g. `export EpiphanSDK_DIR="/opt/epiphan_sdk-3.30.3.0007"`.


## Blackmagic Drivers and Blackmagic Desktop Video SDK

These instructions are provided for convenience only.
Always check the manufacturer's manuals before proceeding.

1. Download and unpack [Blackmagic Desktop Video SDK][blackmagic-support].
1. If the resulting folder name has spaces e.g. `Blackmagic DeckLink SDK 10.11.1`, replace spaces with an underscore,
e.g. `Blackmagic_DeckLink_SDK_10.11.1`.
1. (Optional) Move the resulting folder to a system folder, e.g. `/opt`, for easy access by all users.
1. Specify the **absolute** path of this folder as the `BlackmagicSDK_DIR` environment variable, e.g.
`export BlackmagicSDK_DIR="/opt/Blackmagic_DeckLink_SDK_10.11.1"`.
1. Download and unpack [Blackmagic Desktop Video][blackmagic-support].
1. Install the driver package appropriate for your system, e.g.
`dpkg -i Blackmagic_Desktop_Video_Linux_10.11.1/deb/x86_64/desktopvideo_10.11.1a4_amd64.deb`
1. (Optional) Install the GUI components, e.g.
`dpkg -i Blackmagic_Desktop_Video_Linux_10.11.1/deb/x86_64/desktopvideo-gui_10.11.1a4_amd64.deb` and
`dpkg -i Blackmagic_Desktop_Video_Linux_10.11.1/deb/x86_64/mediaexpress_3.5.6a2_amd64.deb`
1. Check your Blackmagic card's firmware status: `BlackmagicFirmwareUpdater status`, and update if necessary, e.g.
`BlackmagicFirmwareUpdater update 0`

[blackmagic-support]: https://www.blackmagicdesign.com/support

## OpenCV

Please see the [OpenCV website](http://opencv.org/) for possible installation routes.
If using FFmpeg, make sure your OpenCV is properly linked against your FFmpeg installation.


## FFmpeg

### Ubuntu 14.04 and 16.04

On Ubuntu, FFmpeg needs to be compiled from source code and installed:

1. `git clone https://github.com/FFmpeg/FFmpeg.git`
1. `git checkout n3.1.2`
1. `mkdir ffmpeg-build` and `cd ffmpeg-build`
1. If using kvazaar: `../FFmpeg/configure --enable-shared --enable-avresample --enable-libkvazaar --enable-muxer=mp4`
1. If using x265: `../FFmpeg/configure --enable-shared --enable-avresample --enable-libx265 --enable-gpl --enable-muxer=mp4`
1. If using NVENC: `../FFmpeg/configure --enable-shared --enable-avresample --enable-nvenc --enable-nonfree --enable-muxer=mp4`
1. If using libvpx: `../FFmpeg/configure --enable-shared --enable-avresample --enable-libvpx --enable-muxer=webm`
1. `make -j` and `make install`

**Note:** In case you install FFmpeg in a non-standard location, you will need to appropriately augment your `PKG_CONFIG_PATH` for GIFT-Grab to be able to locate FFmpeg.

### Debian 9

On Debian 9, the APT versions of the FFmpeg libraries can be used.
Simply install them using:

1. `apt install libavfilter-dev`
1. `apt install libavutil-dev`
1. `apt install libswscale-dev`
1. `apt install libavcodec-dev`
1. `apt install libavformat-dev`


## NVENC

Installation instructions:

1. Download the [SDK](https://developer.nvidia.com/video-sdk-601)
1. Extract its contents and copy `nvidia_video_sdk_6.0.1/Samples/common/inc/nvEncodeAPI.h` to a standard system include folder (e.g. `/usr/local/include`)


## kvazaar

Installation instructions:

1. `git clone https://github.com/ultravideo/kvazaar.git`
1. `cd kvazaar`
1. `git checkout v0.8.3`
1. `./autogen.sh`
1. `./configure`
1. `make -j` and `make install`

Caveats:

* If you encounter an error of the form `error while loading shared libraries: libkvazaar.so.3: cannot open shared object file: No such file or directory`:
   1. Create a file `/etc/ld.so.conf.d/kvazaar.conf`
   1. Put only the following line into it: `/usr/local/lib`
   1. Run `ldconfig`


## x265

Installation instructions:

1. `hg clone https://bitbucket.org/multicoreware/x265`
1. `cd x265`
1. `hg checkout 1.9`
1. `cd ..` and `mkdir x265-build` and `cd x265-build`
1. `cmake -D ENABLE_SHARED:bool=on ../x265/source/`
1. `make -j` and `make install`


## libvpx

Install by `apt-get install libvpx-dev`.


## libVLC

**Note:** Capturing network video streams in the I420 colour space works only with the stable libVLC releases that can be installed via `apt-get install libvlc-dev` (**only on Ubuntu 14.04 LTS**).
We are working to get this feature operational on Ubuntu 16.04 LTS as well.
The instructions below are for other features that require libVLC such as acquiring video using Epiphan cards.

1. Download a [nighly build](http://nightlies.videolan.org/build/source/?C=M;O=D), e.g. `wget http://nightlies.videolan.org/build/source/vlc-3.0.0-20160913-0237-git.tar.xz`
1. `tar xf vlc-3.0.0-20160913-0237-git.tar.xz`
1. `mkdir vlc-3-build` and `cd vlc-3-build`
1. `../vlc-3.0.0-git/bootstrap`
1. `../vlc-3.0.0-git/configure`
1. `./compile` and `make install`


## NumPy support and Boost.Python

**NB:** Video data can be exposed as a flat as well as structured [`ndarray`](https://docs.scipy.org/doc/numpy/reference/arrays.ndarray.html).
The latter currently works **only for BGRA data** and the resulting `ndarray`'s shape is compatible with the axis ordering used in [SciPy routines](https://docs.scipy.org/doc/scipy/reference/).
Support for this feature with the other colour spaces GIFT-Grab uses is under construction.

### Linux

The current Boost.Python version installable from the Ubuntu repositories does not provide the functionality GIFT-Grab needs for its NumPy support.
However you can install the 1.63.0 beta version as follows:

1. `wget https://sourceforge.net/projects/boost/files/boost/1.63.0.beta.1/boost_1_63_0_b1.tar.bz2`
1. `tar xvfj boost_1_63_0_b1.tar.bz2`
1. `cd boost_1_63_0`
1. `./bootstrap.sh --with-libraries=python` (`--with-libraries=python` deactivates all other Boost components, ensuring minimum compilation time)
1. `./b2`
1. `./b2 install` (running this without the `./b2` step above might result in the Boost components being installed even if the build fails)

### Windows (**experimental**)

1. Download the source code from the [Boost website][boost]
1. Go to the `tools\build` sub-folder
1. (In Windows PowerShell) run `.\bootstrap.bat --with-libraries=python` to configure Boost
1. Run `.\b2.exe install` to install Boost

[boost]: https://www.boost.org/
