#!/bin/bash
if [ $num_lines -gt 0 ]; then
  echo "No alive device seems to be connected to Epiphan"
  echo "Please connect both ports of Storz, and retry build"
  exit 1
else
  cd "$GiftGrab_BUILD_DIR"
  cp "$GiftGrab_SOURCE_DIR/python/modules/epiphan.py" ./
  cp "$GiftGrab_SOURCE_DIR/tests/epiphan/dvi2pcieduo/$DVI2PCIEDUO_REALTIME" ./
  cp "$GiftGrab_SOURCE_DIR/tests/epiphan/dvi2pcieduo/conftest.py" ./
  cp --recursive "$GiftGrab_SOURCE_DIR/tests/epiphan/dvi2pcieduo/config" ./
  cp "$GiftGrab_SOURCE_DIR/python/modules/example_epiphan/"* ./config
  cp "$GiftGrab_SOURCE_DIR/tests/utils/inspection.py" ./

  py.test --colour-space=BGR24 $DVI2PCIEDUO_REALTIME
fi

