#!/usr/bin/env python
# -*- coding: utf-8 -*-

from time import sleep
import argparse
import os.path
import threading
from pygiftgrab import (VideoSourceFactory,
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


class BuffererRed(IObservableObserver):

    def __init__(self):
        super(BuffererRed, self).__init__()

    def update(self, frame):
        global buffer_red
        # TODO


class HistogrammerRed(threading.Thread):

    def __init__(self):
        super(HistogrammerRed, self).__init__()
        self.running = False

    def run(self):
        if self.running:
            return

        # TODO
        self.running = True
        while self.running:
            print('Running')
            # TODO
            sleep(0.100)

    def stop(self):
        self.running = False


class Dyer(IObservableObserver):

    def __init__(self, channel, value):
        super(Dyer, self).__init__()
        assert 0 <= channel < 3
        assert 0 <= value < 256
        self.channel = channel
        self.value = value

    def update(self, frame):
        data = frame.data(True)
        data[:, :, self.channel] = self.value


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

    tfac = VideoTargetFactory.get_instance()
    frame_rate = reader.get_frame_rate()

    red = Dyer(2, 127)
    green = Dyer(1, 191)
    hist = HistogrammerRed()
    hist.start()

    red_file = os.path.join('.', ''.join([filename, '-red', ext]))
    red_writer = tfac.create_file_writer(Codec.HEVC, red_file, frame_rate)
    yellow_file = os.path.join('.', ''.join([filename, '-yellow', ext]))
    yellow_writer = tfac.create_file_writer(Codec.HEVC, yellow_file, frame_rate)

    reader.attach(red)
    red.attach(red_writer)
    red.attach(green)
    green.attach(yellow_writer)

    sleep(20)  # operate pipeline for 20 sec
    hist.stop()

    reader.detach(red)
    red.detach(red_writer)
    red.detach(green)
    green.detach(yellow_writer)
