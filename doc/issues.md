# Known issues

Not all of the following are GIFT-Grab issues.
Some of them are related to the external dependencies.
However here we document all as they may be useful when getting started with GIFT-Grab.


## Capturing network streams in I420

Please see the note in the [libVLC installation instructions](doc/tips.md#libvlc) about capturing network streams in the I420 colour space.

## Encoding VP9 on Ubuntu 16.04 LTS

The frame rate of VP9-encoded WebM files is set incorrectly on Ubuntu 16.04 LTS.

## Connecting to Epiphan DVI2PCIe Duo on Ubuntu 16.04 LTS

On Ubuntu 16.04 LTS, the first few (~1-3) attempts to connect to the DVI port of an Epiphan DVI2PCIe Duo, after connecting and turning on a video source, fail.
Subsequent attempts work as expected.

## `pyconfig.h` not found

* Encountered on Debian 9 when compiling Boost.
* `CPLUS_INCLUDE_PATH` might need to be set to include the directory where `pyconfig.h` is located.

## `BOOST_ROOT` not picked by CMake

* On Debian 9 CMake was reported not to pick an `export`'ed `BOOST_ROOT`.
* Specifying `BOOST_ROOT` to CMake as `-D BOOST_ROOT=/path/to/your/boost` solves this problem.

## `ImportError` with a Boost Python `undefined symbol`

If after successfully building / installing GIFT-Grab, you encounter an error similar to

```
ImportError: /home/stefan/src/GIFT-Grab/pygiftgrab.so: undefined symbol: _ZN5boost6python6detail11init_moduleEPKcPFvv
```

this probably means that your Boost was compiled and linked against Python 3.

Please make sure:
* you are using Python 2
* your Boost is built and linked against Python 2 libs
