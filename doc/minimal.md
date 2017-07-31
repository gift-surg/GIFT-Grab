# Summary

This file documents the instructions needed to build a minimal version of GIFT-Grab such that the [SciPy example][scipy-ex] can be executed.
Here we focus on Debian 9.
The steps are mostly identical to Ubuntu; however wherever applicable we explain any differences.

[scipy-ex]: http://gift-grab.readthedocs.io/en/latest/scipy.html#full-source-code


# Installing the needed tools and libraries

## GCC

`apt install build-essential`

## Python libraries

`apt install python-dev`

## Git

`apt install git`

## CMake

`apt install cmake`

## FFmpeg

```
apt install libavfilter-dev
apt install libavutil-dev
apt install libswscale-dev
apt install libavcodec-dev
apt install libavformat-dev
```

For Ubuntu, please see [these instructions][ffmpeg-ubuntu].

[ffmpeg-ubuntu]: doc/tips.md#ubuntu

## NumPy

`apt install python-numpy`

## SciPy

`apt install python-scipy`

## Boost

```
wget https://sourceforge.net/projects/boost/files/boost/1.63.0/boost_1_63_0.tar.bz2
tar xvfj boost_1_63_0.tar.bz2
cd boost_1_63_0
./bootstrap.sh --with-libraries=python
./b2
./b2 install
```

# Obtaining and building GIFT-Grab

## Clone the repository

* `git clone https://github.com/gift-surg/GIFT-Grab.git`
* This fetches the default branch. In case you'd like to build a specific version, use e.g. `cd GIFT-Grab; git checkout v1705; cd ..`

## Configure using CMake

```
mkdir gift-grab-build; cd gift-grab-build
cmake -D BUILD_PYTHON=ON -D USE_FILES=ON -D USE_HEVC=ON -D USE_NUMPY=ON -D ENABLE_GPL=ON -D USE_X265=ON ../../GIFT-Grab/src
```

## Compile and link

In the same `gift-grab-build` as the step above: `make -j`
This will create a `pygiftgrab.so` in the build folder.


# Running the SciPy example

## Download a sample HEVC video file

For instance see [this website][hevc-website].
Save one of the sample x265 (HEVC) files as `/tmp/myinput.mp4`.

[hevc-website]: https://x265.com/hevc-video-files/

## Execute the SciPy example

Save the [SciPy example][scipy-ex] as `ggscipyex.py` inside the build folder.
In the build folder execute it using `python2 ggscipyex.py`.

## View the result

Executing the SciPy example should produce an output file `/tmp/myoutput.mp4`.
This should contain a Gaussian-smoothed version of (part of) the downloaded HEVC file.
Note that this output file should always contain the first frames from the downloaded file; however it may not contain all frames to the end.
This is because encoding is a computationally intensive task and to be able to encode all frames you will need GPU-accelerated encoding.
You can use [NVENC][gg-nvenc] for that.

[gg-nvenc]: doc/tips.md#nvenc
