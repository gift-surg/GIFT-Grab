#!/usr/bin/env bash

function check_epiphan_alive
{
    if [ $num_lines -gt 0 ]; then
        echo "No alive device seems to be connected to Epiphan"
        echo "Please connect both ports of Storz, and retry build"
        exit 1
    fi
}

# first test with BGR24
rm -rf "$GiftGrab_BUILD_DIR"
mkdir -p "$GiftGrab_BUILD_DIR"
cd "$GiftGrab_BUILD_DIR"
cmake -D USE_EPIPHAN_DVI2PCIE_DUO=ON \
      -D BUILD_PYTHON=ON \
      -D BUILD_TESTS=ON \
      -D USE_H265=ON -D USE_NVENC=ON \
      "$GiftGrab_SOURCE_DIR"
make -j
# check smt connected to Epiphan
check_epiphan_alive
# copy needed files
cd "$GiftGrab_BUILD_DIR"
cp "$GiftGrab_SOURCE_DIR/python/modules/epiphan.py" ./
cp "$GiftGrab_SOURCE_DIR/tests/epiphan/dvi2pcieduo/$DVI2PCIEDUO_REALTIME" ./
cp "$GiftGrab_SOURCE_DIR/tests/epiphan/dvi2pcieduo/conftest.py" ./
cp --recursive "$GiftGrab_SOURCE_DIR/tests/epiphan/dvi2pcieduo/config" ./
cp "$GiftGrab_SOURCE_DIR/python/modules/example_epiphan/"* ./config
cp "$GiftGrab_SOURCE_DIR/tests/utils/inspection.py" ./
# actual test
py.test --colour-space=BGR24 $DVI2PCIEDUO_REALTIME

# and now with I420
cmake -D USE_COLOUR_SPACE_I420=ON .
make -j
# check smt connected to Epiphan
check_epiphan_alive
py.test --colour-space=I420 $DVI2PCIEDUO_REALTIME
