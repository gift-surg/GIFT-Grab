#!/bin/bash
rm -rf "$GiftGrab_BUILD_DIR"
mkdir -p "$GiftGrab_BUILD_DIR"
cd "$GiftGrab_BUILD_DIR"

# so that CI script will fail when e.g. make -j fails:
function exit_on_fail {
    exit_status=$?
    if [[ "$exit_status" -ne "0" ]]; then
        echo "Build failed due to last exit status being $exit_status"
        exit $exit_status
    fi
}

# bare-bones build
cmake -D CMAKE_INSTALL_PREFIX="$INSTALL_DIR" \
      "$GiftGrab_SOURCE_DIR"
make -j; exit_on_fail
make install

# build docs
cmake -D BUILD_DOC=ON .
make -j; exit_on_fail
make install

# Python and tests (currently all python)
cmake -D BUILD_PYTHON=ON \
      -D BUILD_TESTS=ON \
      .
make -j; exit_on_fail
make install

# H265 support
cmake -D USE_H265=ON \
      -D FFmpeg_DIR=$FFmpeg_DIR \
      .
make -j; exit_on_fail
make install

# Xvid support (OpenCV should get switched here)
cmake -D USE_XVID=ON .
make -j; exit_on_fail
make install

# Epiphan DVI2PCIe Duo support (OpenCV)
cmake -D USE_EPIPHAN_DVI2PCIE_DUO=ON .
make -j; exit_on_fail
make install

# Epiphan DVI2PCIe Duo support (EpiphanSDK)
cmake -D USE_COLOUR_SPACE_I420=ON \
      -D EpiphanSDK_DIR=$EpiphanSDK_DIR \
      .
make -j; exit_on_fail
make install
