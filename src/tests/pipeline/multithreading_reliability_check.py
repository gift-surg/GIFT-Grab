#!/usr/bin/env python
# -*- coding: utf-8 -*-

from time import (sleep, time)
import argparse
import os.path
import threading
import numpy as np
import scipy.misc
from pygiftgrab import (VideoSourceFactory, VideoFrame,
                        ColourSpace, IObservableObserver,
                        VideoTargetFactory, Codec, IObserver)

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


buffer_red, buffer_orig = None, None
lock = threading.Lock()


class SnapshotSaver(IObserver):
    """A snapshot saver for saving incoming frames to PNG files."""

    def __init__(self, root_dir, save_freq=5):
        """
        Initialise a snapshot saver with a saving frequency.

        :param root_dir: the folder where to save the snapshots
        :param save_freq: saving frequency. The default value tells
        the saver to save every 5 seconds.
        """
        super(SnapshotSaver, self).__init__()
        assert 5 <= save_freq  # to avoid flooding disk with images
        self.save_freq = save_freq
        self.root_dir = root_dir
        self.last_saved = time()
        self.num_saved = 0

    def update(self, frame):
        """Implement ``IObserver.update``."""
        if time() - self.last_saved >= self.save_freq:
            self.num_saved += 1
            out_file = os.path.join(self.root_dir,
                                    'frame-{:010d}.png'.format(self.num_saved))
            scipy.misc.imsave(out_file, frame.data(True))
            self.last_saved = time()


class Bufferer(IObservableObserver):

    def __init__(self, buffer):
        super(Bufferer, self).__init__()
        self.buffer = buffer

    def update(self, frame):
        with lock:
            data = frame.data(True)
            self.buffer[:, :, :] = data[:, :, :]


class Histogrammer(threading.Thread):

    channels = ('Blue', 'Green', 'Red', 'Alpha')

    def __init__(self, buffer, channel, tag, frame_rate, display_freq):
        super(Histogrammer, self).__init__()
        assert channel in range(3)
        assert 0 < frame_rate <= 60
        assert 0 <= display_freq
        self.channel = channel
        self.buffer = buffer
        self.tag = tag
        self.display_freq = display_freq
        self.num_skipped = 0
        self.sleep_interval = 1.0 / frame_rate
        self.running = False

    def run(self):
        if self.running:
            return

        histogram, num_bins = None, 10
        scale = np.array([i for i in range(1, num_bins + 1)], np.float)
        self.running = True
        while self.running:
            with lock:
                histogram, _ = np.histogram(
                    self.buffer[:, :, 2], bins=num_bins, range=(0, 256)
                )
            if histogram is not None:
                coloredness = np.sum(histogram * scale)
                coloredness /= np.sum(histogram)
                coloredness /= num_bins
                if self.num_skipped >= self.display_freq:
                    print('{}ness of {} image: {:.0%}'.format(
                        Histogrammer.channels[self.channel],
                        self.tag, coloredness
                    ))
                    self.num_skipped = 0
                else:
                    self.num_skipped += 1
            sleep(self.sleep_interval)

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

    red_dyer = Dyer(2, 128)
    green_dyer = Dyer(1, 64)

    buffer_red = np.zeros(frame_shape, np.uint8)
    bufferer_red = Bufferer(buffer_red)
    buffer_orig = np.zeros_like(buffer_red)
    bufferer_orig = Bufferer(buffer_orig)

    hist_red = Histogrammer(buffer_red, 2, 'red-dyed', 60.0, 10)
    hist_red.start()
    hist_orig = Histogrammer(buffer_orig, 2, 'original', 50.0, 10)
    hist_orig.start()

    red_file = os.path.join('.', ''.join([filename, '-red', ext]))
    red_writer = tfac.create_file_writer(Codec.HEVC, red_file, frame_rate)
    yellow_file = os.path.join('.', ''.join([filename, '-yellow', ext]))
    yellow_writer = tfac.create_file_writer(Codec.HEVC, yellow_file, frame_rate)

    yellow_snapshots = SnapshotSaver('.')

    reader.attach(bufferer_orig)
    bufferer_orig.attach(red_dyer)
    red_dyer.attach(red_writer)
    red_dyer.attach(bufferer_red)
    red_dyer.attach(green_dyer)
    green_dyer.attach(yellow_writer)
    green_dyer.attach(yellow_snapshots)

    sleep(20)  # operate pipeline for 20 sec
    hist_red.stop()
    hist_orig.stop()

    reader.detach(bufferer_orig)
    bufferer_orig.detach(red_dyer)
    red_dyer.detach(red_writer)
    red_dyer.detach(bufferer_red)
    red_dyer.detach(green_dyer)
    green_dyer.detach(yellow_writer)
    green_dyer.detach(yellow_snapshots)
