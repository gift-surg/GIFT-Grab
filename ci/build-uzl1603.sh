#!/bin/bash
rm -rf "$GiftGrab_BUILD_DIR"
mkdir -p "$GiftGrab_BUILD_DIR"
cd "$GiftGrab_BUILD_DIR"
cmake -D USE_EPIPHAN_DVI2PCIE_DUO=ON \
      -D BUILD_PYTHON=ON \
      -D BUILD_TESTS=ON \
      -D USE_H265=ON -D USE_NVENC=ON \
      "$GiftGrab_SOURCE_DIR"
make -j
