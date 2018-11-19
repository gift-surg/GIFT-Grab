#!/usr/bin/env python2
# -*- coding: utf-8 -*-

"""
Example demonstrating how stereo video frames can be captured
using a frame grabber card that supports this feature.
"""

import time
import cv2
import numpy as np
from pygiftgrab import (IObserver, VideoSourceFactory,
                        ColourSpace, Device, VideoFrame)


class StereoFrameSaver(IObserver):

    def __init__(self):
        super(StereoFrameSaver, self).__init__()
        self.current = 0

    def update(self, frame):
        self.current += 1

        if self.current <= 3:  # do not flood terminal
            print(
                'Got {} stereo frames\n'.format(
                    frame.stereo_count()
                )
            )
            print(
                'Stereo data length (bytes):\n'
                '\tdata_length(): {}\n'
                '\tdata_length(0): {}\n'
                '\tdata_length(1): {}\n'.format(
                    frame.data_length(), frame.data_length(0),
                    frame.data_length(1)
                )
            )

        frame_shape = (frame.rows(), frame.cols(), 4)

        if self.current == 1:
            # all three calls below save the same frame,
            # that is the first of the two stereo frames
            cv2.imwrite(
                'mono-frame.data.png',
                np.reshape(frame.data(), frame_shape)
            )
            cv2.imwrite(
                'mono-frame.data-False.png',
                np.reshape(frame.data(False), frame_shape)
            )
            cv2.imwrite(
                'mono-frame.data-False-0.png',
                np.reshape(frame.data(False, 0), frame_shape)
            )

        elif self.current == 2:
            # the two calls below save the two stereo frames,
            # however the data needs to be reshaped, as the
            # call to the data method yields a flat NumPy array
            cv2.imwrite(
                'stereo-frame.data-False-0.png',
                np.reshape(frame.data(False, 0), frame_shape)
            )
            cv2.imwrite(
                'stereo-frame.data-False-1.png',
                np.reshape(frame.data(False, 1), frame_shape)
            )

        elif self.current == 3:
            # the two calls below save the two stereo frames,
            # without the need for reshaping the data, as the
            # call to the data method already yields a
            # structured NumPy array
            cv2.imwrite(
                'stereo-frame.data-True-0.png',
                frame.data(True, 0)
            )
            cv2.imwrite(
                'stereo-frame.data-True-1.png',
                frame.data(True, 1)
            )


if __name__ == '__main__':
    sfac = VideoSourceFactory.get_instance()
    source = sfac.get_device(
        Device.DeckLink4KExtreme12G, ColourSpace.BGRA
    )

    saver = StereoFrameSaver()

    source.attach(saver)

    time.sleep(2)  # operate pipeline for 2 sec

    source.detach(saver)
