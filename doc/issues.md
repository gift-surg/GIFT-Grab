# Known issues

Not all of the following are GIFT-Grab issues.
Some of them are related to the external dependencies.
However here we document all as they may be useful when getting started with GIFT-Grab.


## GIFT-Grab caveats

* In case GIFT-Grab is built with a custom `CMAKE_INSTALL_PREFIX` that is **not** a standard system path, `${CMAKE_INSTALL_PREFIX}/lib/giftgrab` needs to be included in CMake's `CMAKE_MODULE_PATH` for `FIND_PACKAGE(GiftGrab)` to work.
* If you encounter an `option not recognized` error while `pip install`ling GIFT-Grab, run `pip install pytest` and `pip install PyYAML` before running `pip install gift-grab`.

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

## `boost_numpy` library not found by CMake

If (although you've successfully installed Boost) you encounter an error similar to

```
Could not find the following Boost libraries:

          boost_numpy

  Some (but not all) of the required Boost libraries were found.  You may
  need to install these additional Boost libraries.  Alternatively, set
  BOOST_LIBRARYDIR to the directory containing Boost libraries or BOOST_ROOT
  to the location of Boost.
```

this means that your Boost was built with Python support, but does not include NumPy support.
Boost seems not to build the `boost_numpy` library if NumPy is not installed on the system **when building Boost**.

This issue can be resolved by:
1. installing NumPy
1. building and installing Boost from scratch
