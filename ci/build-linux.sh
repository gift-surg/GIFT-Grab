#!/bin/bash

# Python and tests (currently all python)
cmake -D BUILD_PYTHON=ON -D BUILD_TESTS=ON .
make -j; exit_on_fail
# no ctest: no features activated to be tested yet
make install

# H265 support
cmake -D USE_H265=ON .
make -j; exit_on_fail
ctest; exit_on_fail
make install

# hardware-accelerated H265
cmake -D USE_NVENC=ON .
make -j; exit_on_fail
ctest; exit_on_fail
make install

# Xvid support (OpenCV should get switched here)
cmake -D USE_XVID=ON .
make -j; exit_on_fail
ctest; exit_on_fail
make install

# Epiphan DVI2PCIe Duo support (OpenCV)
cmake -D USE_EPIPHAN_DVI2PCIE_DUO=ON .
make -j; exit_on_fail
# no ctest for Epiphan DVI2PCIe Duo on
# general-purpose Linux test server
make install

# Epiphan DVI2PCIe Duo support (EpiphanSDK)
cmake -D USE_I420=ON .
make -j; exit_on_fail
# no ctest for Epiphan DVI2PCIe Duo on
# general-purpose Linux test server
make install

# build docs
cmake -D BUILD_DOC=ON .
make -j; exit_on_fail
# no ctest: only checking whether doc building
# properly here
make install
