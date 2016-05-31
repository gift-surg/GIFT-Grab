#!/bin/bash
num_lines=$(/opt/installers/epiphan/sdk/3.30.3.0007/epiphan/samples/v2u/build/release/v2u | grep --count "no signal detected")
if [ $num_lines -gt 0 ]; then
  echo "No alive device seems to be connected to Epiphan"
  echo "Please connect both ports of Storz, and retry build"
  exit 1
else
  cd "$GiftGrab_BUILD_DIR"
  cp "$GiftGrab_SOURCE_DIR/python/modules/epiphan.py" ./
  cp "$GiftGrab_SOURCE_DIR/tests/epiphan/dvi2pcieduo.py" ./
  cp "$GiftGrab_SOURCE_DIR/tests/epiphan/conftest.py" ./
  cp --recursive "$GiftGrab_SOURCE_DIR/tests/epiphan/config" ./
  cp "$GiftGrab_SOURCE_DIR/python/modules/example_epiphan/"* ./config

  py.test --bgr24 dvi2pcieduo.py
fi

