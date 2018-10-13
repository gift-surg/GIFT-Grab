#!/usr/bin/env bash
# -*- coding: utf-8 -*-

# Build a basic GIFT-Grab capable of reading a video file,
# and encoding to a video file in real time, with Python
# and NumPy support. Subsequently run the multi-threading
# reliability check script.

ROOT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"
BUILD_DIR=$ROOT_DIR/mtr-build
SOURCE_DIR="$( cd "$ROOT_DIR/../.." >/dev/null && pwd )"
CMAKE_OPTS="-D USE_FILES=ON"
CMAKE_OPTS="$CMAKE_OPTS -D USE_HEVC=ON"
CMAKE_OPTS="$CMAKE_OPTS -D ENABLE_NONFREE=ON -D USE_NVENC=ON"
CMAKE_OPTS="$CMAKE_OPTS -D BUILD_PYTHON=ON -D USE_NUMPY=ON"
CMAKE_OPTS="$CMAKE_OPTS -D CMAKE_BUILD_TYPE=Debug"
MTR_SCRIPT=$ROOT_DIR/multithreading_reliability_check.py
SESSION_DIR=$ROOT_DIR/$(date +"%Y-%m-%d-%H-%M-%S")
mkdir $SESSION_DIR
ulimit -c unlimited

BUILD_LOG=$SESSION_DIR/build.log
{
mkdir -p $BUILD_DIR
rm -rf $BUILD_DIR/*
cd $BUILD_DIR
cmake $CMAKE_OPTS $SOURCE_DIR
make -j4
} > $BUILD_LOG 2>&1

run_no=1
WORKING_DIR=$SESSION_DIR/$run_no
mkdir $WORKING_DIR
cd $WORKING_DIR
RUN_LOG=$WORKING_DIR/run.log
{
PYTHONPATH=$BUILD_DIR python $MTR_SCRIPT --input $1
} > $RUN_LOG 2>&1

ulimit -c 0
