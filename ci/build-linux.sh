#!/bin/bash
rm -rf "$GiftGrab_BUILD_DIR"
mkdir -p "$GiftGrab_BUILD_DIR"
cd "$GiftGrab_BUILD_DIR"

# bare-bones build
cmake -D CMAKE_INSTALL_PREFIX="$INSTALL_DIR" \
      -D BUILD_DOC=ON \
      "$GiftGrab_SOURCE_DIR"
make -j
make install

# Python and tests (some of which depend on python)
cmake -D BUILD_PYTHON=ON \
      -D BUILD_TESTS=ON \
      .
make -j
make install

# H265 support
cmake -D USE_H265=ON \
      -D FFmpeg_DIR=$FFmpeg_DIR \
      .
make -j
make install

# Xvid support (OpenCV should get switched here)
cmake -D USE_XVID=ON .
make -j
make install

# Epiphan DVI2PCIe Duo support (OpenCV)
cmake -D USE_EPIPHAN_DVI2PCIE_DUO=ON .
make -j
make install

# Epiphan DVI2PCIe Duo support (EpiphanSDK)
cmake -D USE_COLOUR_SPACE_I420=ON \
      -D EpiphanSDK_DIR=$EpiphanSDK_DIR \
      .
make -j
make install
