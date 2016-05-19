#!/bin/bash
export GIT_SSL_NO_VERIFY=1
rm -rf "$GiftGrab_BUILD_DIR"
mkdir -p "$GiftGrab_BUILD_DIR"
cd "$GiftGrab_BUILD_DIR"
cmake -D CMAKE_INSTALL_PREFIX="$INSTALL_DIR" \
      -D USE_FILES=ON \
      -D USE_EPIPHAN=ON \
      -D BUILD_DOC=ON \
      -D BUILD_PYTHON=ON \
      -D BUILD_TESTS=ON \
      -D USE_FFMPEG=ON -D FFMPEG_HWACCEL=ON \
      -D FFmpeg_DIR=/opt/ffmpeg/3.0.1 \
      "$GiftGrab_SOURCE_DIR"
make -j
make install

