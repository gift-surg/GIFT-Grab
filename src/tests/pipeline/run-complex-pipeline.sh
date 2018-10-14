#!/usr/bin/env bash
# -*- coding: utf-8 -*-

# This script provides a multi-threading reliability check.
# The background is issue #16. It looks like in applications
# where multiple Python threads are involved, occasionally
# the acquisition of the Global Interpreter Lock leads to a
# deadlocks, which crashes the whole application with a
# non-specific segmentation fault.
#
# It builds a basic GIFT-Grab capable of reading a video file,
# and encoding to a video file in real time, with Python and
# NumPy support. It subsequently runs a multi-threaded
# GIFT-Grab pipeline a number of times, recording the exit
# status each time. This is essentially a stress-test that
# should serve as a validation that issue #16 is fixed.

if [ $# -lt 1 ] || [ $# -gt 3 ];
then
    THIS_SCRIPT="$(basename "$(test -L "${BASH_SOURCE[0]}" && readlink "$0" || echo "$0")")"
    printf "Usage: $THIS_SCRIPT video_file [ num_reps [ output_dir ] ]\n"
    printf "\tvideo_file: path to an HEVC-encoded MP4 file\n"
    printf "\tnum_reps:   how many times to run the Python script "
    printf "(default: once)\n"
    printf "\toutput_dir: where to save all the generated output "
    printf "(default: current directory)\n"
    exit 1
fi

CALL_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"
SOURCE_DIR="$( cd "$CALL_DIR/../.." >/dev/null && pwd )"
MTR_SCRIPT=$SOURCE_DIR/tests/pipeline/multithreading_reliability_check.py
if [ $# -ge 3 ];
then
    ROOT_DIR="$( cd "$3" >/dev/null && pwd )"
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
echo "Session directory: $SESSION_DIR"
ulimit -c unlimited

BUILD_LOG=$SESSION_DIR/build.log
{
    git describe --dirty
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

declare -a exit_code_freqs
for run_no in `seq 1 $num_reps`;
do
    WORKING_DIR=$SESSION_DIR/$(printf "%03d" $run_no)
    mkdir $WORKING_DIR
    cd $WORKING_DIR
    RUN_LOG=$WORKING_DIR/run.log
    {
        PYTHONPATH=$BUILD_DIR python $MTR_SCRIPT --input $1

        exit_code=$?
        echo "Exit code was: $exit_code"
        freq=${exit_code_freqs[$exit_code]}
        let freq=$freq+1
        exit_code_freqs[$exit_code]=$freq
    } > $RUN_LOG 2>&1

    if [ $(($run_no % 10)) -eq 0 ] || [ $run_no -eq $num_reps ];
    then
        EXIT_CODES_LOG=$SESSION_DIR/exit-codes-$run_no.csv
        printf "%10s" "exit-code" >> $EXIT_CODES_LOG
        printf "%10s" "frequency" >> $EXIT_CODES_LOG
        printf "\n" >> $EXIT_CODES_LOG
        for exit_code in "${!exit_code_freqs[@]}";
        do
            printf "%10d" $exit_code >> $EXIT_CODES_LOG
            printf "%10d" ${exit_code_freqs[$exit_code]} >> $EXIT_CODES_LOG
            printf "\n" >> $EXIT_CODES_LOG
        done
    fi

    sleep 5
done

ulimit -c 0
