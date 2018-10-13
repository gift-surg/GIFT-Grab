#!/usr/bin/env bash
# -*- coding: utf-8 -*-

# Build a basic GIFT-Grab capable of reading a video file,
# and encoding to a video file in real time, with Python
# and NumPy support. Subsequently run the multi-threading
# reliability check script.

CALL_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"
SOURCE_DIR="$( cd "$CALL_DIR/../.." >/dev/null && pwd )"
MTR_SCRIPT=$SOURCE_DIR/tests/pipeline/multithreading_reliability_check.py
if [ $# -ge 3 ];
then
    ROOT_DIR=$3
else
    ROOT_DIR=$CALL_DIR
fi

BUILD_DIR=$ROOT_DIR/mtr-build
CMAKE_OPTS="-D USE_FILES=ON"
CMAKE_OPTS="$CMAKE_OPTS -D USE_HEVC=ON"
CMAKE_OPTS="$CMAKE_OPTS -D ENABLE_NONFREE=ON -D USE_NVENC=ON"
CMAKE_OPTS="$CMAKE_OPTS -D BUILD_PYTHON=ON -D USE_NUMPY=ON"
CMAKE_OPTS="$CMAKE_OPTS -D CMAKE_BUILD_TYPE=Debug"
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

num_reps=1
if [ $# -ge 2 ];
then
    num_reps=$2
fi
for run_no in `seq 1 $num_reps`;
do
    WORKING_DIR=$SESSION_DIR/$(printf "%03d" $run_no)
    mkdir $WORKING_DIR
    cd $WORKING_DIR
    RUN_LOG=$WORKING_DIR/run.log
    {
        PYTHONPATH=$BUILD_DIR python $MTR_SCRIPT --input $1
    } > $RUN_LOG 2>&1
done

ulimit -c 0
