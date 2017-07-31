# Known issues

## Capturing network streams in I420

Please see the note in the [libVLC installation instructions](doc/tips.md#libvlc) about capturing network streams in the I420 colour space.

## Encoding VP9 on Ubuntu 16.04 LTS

The frame rate of VP9-encoded WebM files is set incorrectly on Ubuntu 16.04 LTS.

## Connecting to Epiphan DVI2PCIe Duo on Ubuntu 16.04 LTS

On Ubuntu 16.04 LTS, the first few (~1-3) attempts to connect to the DVI port of an Epiphan DVI2PCIe Duo, after connecting and turning on a video source, fail.
Subsequent attempts work as expected.

