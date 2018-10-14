#!/usr/bin/env bash

CALL_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"
SOURCE_DIR="$( cd "$CALL_DIR/../../.." >/dev/null && pwd )"
BM3D_SCRIPT=$CALL_DIR/bm3d.py
if [ $# -ge 1 ];
then
    ROOT_DIR="$( cd "$1" >/dev/null && pwd )"
else
    ROOT_DIR=$CALL_DIR
fi
BUILD_DIR=$ROOT_DIR/bm3d-build
CMAKE_OPTS="-D USE_BLACKMAGIC_DECKLINK_SDI_4K=ON -D ENABLE_NONFREE=ON"
CMAKE_OPTS="$CMAKE_OPTS -D BUILD_PYTHON=ON -D USE_NUMPY=ON"
CMAKE_OPTS="$CMAKE_OPTS -D CMAKE_BUILD_TYPE=Debug"
SESSION_DIR=$ROOT_DIR/$(date +"%Y-%m-%d-%H-%M-%S")
export BlackmagicSDK_DIR="/home/gitlab-runner/environments/giftgrab/blackmagic_sdk/sdk"

mkdir $SESSION_DIR
echo "Session directory: $SESSION_DIR"
ulimit -c unlimited

mkdir -p $BUILD_DIR
rm -rf $BUILD_DIR/*
cd $BUILD_DIR
cmake $CMAKE_OPTS $SOURCE_DIR
make -j4

cd $SESSION_DIR
PYTHONPATH=$BUILD_DIR python $BM3D_SCRIPT
