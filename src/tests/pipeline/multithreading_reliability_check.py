#!/usr/bin/env python
# -*- coding: utf-8 -*-

from time import sleep
import argparse
import os.path
import threading
import numpy as np
from pygiftgrab import (VideoSourceFactory, VideoFrame,
                        ColourSpace, IObservableObserver,
                        VideoTargetFactory, Codec)

"""
This script provides a multi-threading reliability check.
The background is issue #16. It looks like in applications
where multiple Python threads are involved, occasionally
the acquisition of the Global Interpreter Lock leads to a
deadlocks, which crashes the whole application with a
non-specific segmentation fault.

In this script we run a number of multi-threaded GIFT-Grab
pipelines, which should serve as a validation that this
problem is fixed.
"""


buffer_red = None
lock = threading.Lock()


class Bufferer(IObservableObserver):

    def __init__(self, buffer):
        super(Bufferer, self).__init__()
        self.buffer = buffer

    def update(self, frame):
        with lock:
            data = frame.data(True)
            self.buffer[:, :, :] = data[:, :, :]


class HistogrammerRed(threading.Thread):

    def __init__(self, buffer):
        super(HistogrammerRed, self).__init__()
        self.buffer = buffer
        self.running = False

    def run(self):
        if self.running:
            return

        histogram, num_bins = None, 10
        redness_scale = np.array([i for i in range(1, num_bins + 1)], np.float)
        self.running = True
        while self.running:
            with lock:
                histogram, _ = np.histogram(
                    self.buffer[:, :, 2], bins=num_bins, range=(0, 256)
                )
            if histogram is not None:
                redness = np.sum(histogram * redness_scale)
                redness /= np.sum(histogram)
                redness /= num_bins
                redness *= 100
                print('Redness: {} %'.format(int(round(redness))))
            sleep(0.100)

    def stop(self):
        self.running = False


class Dyer(IObservableObserver):

    def __init__(self, channel, increment):
        super(Dyer, self).__init__()
        assert 0 <= channel < 3
        assert 0 <= increment < 256
        self.channel = channel
        self.increment = increment

    def update(self, frame):
        with lock:
            data = frame.data(True)
            channel_data = data[:, :, self.channel]
            channel_data[channel_data < 255 - self.increment] += self.increment


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-i', '--input', type=str, required=True,
                        metavar='VIDEO_FILE',
                        help='Input video file (HEVC-encoded MP4)')
    args = parser.parse_args()
    in_file = args.input

    filename = os.path.basename(in_file)
    filename, ext = os.path.splitext(filename)
    assert filename
    assert ext == '.mp4'

    sfac = VideoSourceFactory.get_instance()
    reader = sfac.create_file_reader(in_file, ColourSpace.BGRA)
    frame = VideoFrame(ColourSpace.BGRA, False)
    reader.get_frame(frame)
    frame_shape = (frame.rows(), frame.cols(), 4)

    tfac = VideoTargetFactory.get_instance()
    frame_rate = reader.get_frame_rate()

    red_dyer = Dyer(2, 64)
    green_dyer = Dyer(1, 64)

    buffer_red = np.zeros(frame_shape, np.uint8)
    bufferer_red = Bufferer(buffer_red)

    hist = HistogrammerRed(buffer_red)
    hist.start()

    red_file = os.path.join('.', ''.join([filename, '-red', ext]))
    red_writer = tfac.create_file_writer(Codec.HEVC, red_file, frame_rate)
    yellow_file = os.path.join('.', ''.join([filename, '-yellow', ext]))
    yellow_writer = tfac.create_file_writer(Codec.HEVC, yellow_file, frame_rate)

    reader.attach(red_dyer)
    red_dyer.attach(red_writer)
    red_dyer.attach(bufferer_red)
    red_dyer.attach(green_dyer)
    green_dyer.attach(yellow_writer)

    sleep(20)  # operate pipeline for 20 sec
    hist.stop()

    reader.detach(red_dyer)
    red_dyer.detach(red_writer)
    red_dyer.detach(bufferer_red)
    red_dyer.detach(green_dyer)
    green_dyer.detach(yellow_writer)
