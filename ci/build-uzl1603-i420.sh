#!/bin/bash
cd "$GiftGrab_BUILD_DIR"
cmake -D USE_COLOUR_SPACE_I420=ON \
      -D EpiphanSDK_DIR=/home/gitlab-runner/environments/giftgrab/opt/include/epiphansdk \
      .
make -j
make install
