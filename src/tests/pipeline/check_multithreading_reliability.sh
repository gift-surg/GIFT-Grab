#!/usr/bin/env bash
# -*- coding: utf-8 -*-

# Build a basic GIFT-Grab capable of reading a video file,
# and encoding to a video file in real time, with Python
# and NumPy support. Subsequently run the multi-threading
# reliability check script.

CURRENT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"
BUILD_DIR=$CURRENT_DIR/mtr-build
SOURCE_DIR="$( cd "$CURRENT_DIR/../.." >/dev/null && pwd )"
CMAKE_OPTS="-D USE_FILES=ON"
CMAKE_OPTS="$CMAKE_OPTS -D USE_HEVC=ON"
CMAKE_OPTS="$CMAKE_OPTS -D ENABLE_NONFREE=ON -D USE_NVENC=ON"
CMAKE_OPTS="$CMAKE_OPTS -D BUILD_PYTHON=ON -D USE_NUMPY=ON"
CMAKE_OPTS="$CMAKE_OPTS -D CMAKE_BUILD_TYPE=Debug"
MTR_SCRIPT=$CURRENT_DIR/multithreading_reliability_check.py

mkdir -p $BUILD_DIR
rm -rf $BUILD_DIR/*
cd $BUILD_DIR
cmake $CMAKE_OPTS $SOURCE_DIR
make -j4
PYTHONPATH=$BUILD_DIR python $MTR_SCRIPT --input $1
