#!/bin/bash
rm -rf "$GiftGrab_BUILD_DIR"
mkdir -p "$GiftGrab_BUILD_DIR"
cd "$GiftGrab_BUILD_DIR"
cmake -D CMAKE_INSTALL_PREFIX="$INSTALL_DIR" \
      -D USE_FILES=ON \
      -D BUILD_DOC=ON \
      -D BUILD_PYTHON=ON \
      "$GiftGrab_SOURCE_DIR"
make -j
make install
