#!/bin/bash
if [ $num_lines -gt 0 ]; then
  echo "No alive device seems to be connected to Epiphan"
  echo "Please connect both ports of Storz, and retry build"
  exit 1
else
  cd "$GiftGrab_BUILD_DIR"

  py.test --colour-space=I420 $DVI2PCIEDUO_REALTIME
fi

