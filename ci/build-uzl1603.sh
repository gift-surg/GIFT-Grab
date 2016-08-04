#!/bin/bash
rm -rf "$GiftGrab_BUILD_DIR"
mkdir -p "$GiftGrab_BUILD_DIR"
cd "$GiftGrab_BUILD_DIR"
cmake -D CMAKE_INSTALL_PREFIX="$INSTALL_DIR" \
      -D USE_EPIPHAN=ON \
      -D BUILD_DOC=ON \
      -D BUILD_PYTHON=ON \
      -D BUILD_TESTS=ON \
      -D USE_FFMPEG=ON -D FFMPEG_HWACCEL=ON \
      -D FFmpeg_DIR=${FFmpeg_DIR} \
      "$GiftGrab_SOURCE_DIR"
make -j
make install

