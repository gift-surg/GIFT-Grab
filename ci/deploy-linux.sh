#!/bin/bash

# NiftyMatch
rm -rf "$NiftyMatch_REPO_DIR"
git clone git@cmiclab.cs.ucl.ac.uk:GIFT-Surg/NiftyMatch.git \
          "$NiftyMatch_REPO_DIR" \
          --branch dev
rm -rf "$NiftyMatch_BUILD_DIR"
mkdir -p "$NiftyMatch_BUILD_DIR"
cd "$NiftyMatch_BUILD_DIR"
cmake -D CMAKE_INSTALL_PREFIX="$INSTALL_DIR" \
      -D CMAKE_BUILD_TYPE=Release \
      "$NiftyMatch_SOURCE_DIR"
make -j
make install

# And finally gpu-mosaicing
rm -rf "$GpuMosaicing_REPO_DIR"
git clone git@cmiclab.cs.ucl.ac.uk:GIFT-Surg/GIFT-Nav-Mosaic.git \
    "$GpuMosaicing_REPO_DIR" --branch dev
rm -rf "$GpuMosaicing_BUILD_DIR"
mkdir -p "$GpuMosaicing_BUILD_DIR"
cd "$GpuMosaicing_BUILD_DIR"
cmake -D NiftyMatch_DIR="$NiftyMatch_DIR" \
      -D CMAKE_MODULE_PATH="$GiftGrab_DIR" \
      -D Qt5Widgets_DIR="$Qt5Widgets_DIR" \
      -D Qt5OpenGL_DIR="$Qt5OpenGL_DIR" \
      -D CMAKE_BUILD_TYPE=Release \
      -D BUILD_TESTS=OFF \
      "$GpuMosaicing_SOURCE_DIR"
make -j
