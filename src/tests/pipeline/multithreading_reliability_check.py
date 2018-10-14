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


# global NumPy buffers
np_buffer_red, np_buffer_orig = None, None

# mutex protecting frame data passed from node to node
# in the GIFT-Grab processing pipeline
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
    """GIFT-Grab processing node that updates a frame buffer."""

    def __init__(self, np_buffer):
        """Initialise a bufferer that will update given buffer."""
        super(Bufferer, self).__init__()
        self.buffer = np_buffer

    def update(self, frame):
        """Implement ``IObservableObserver.update``."""
        with lock:
            data = frame.data(True)
            self.buffer[:, :, :] = data[:, :, :]


class Histogrammer(threading.Thread):
    """
    GIFT-Grab processing node that computes the histogram of
    an image channel and prints how "colored" that channel is.
    """

    channels = ('Blue', 'Green', 'Red', 'Alpha')

    def __init__(self, np_buffer, channel, tag, frame_rate, display_freq):
        """
        :param np_buffer: image buffer to use
        :param channel: image channel to compute coloredness for
        :param tag: a tag describing what this image is, e.g.
        how it's been processed within a GIFT-Grab pipeline
        :param frame_rate: the rate at which to compute the
        coloredness (unit: frames-per-second)
        :param display_freq: how many times to skip the display
        of computed coloredness, e.g. if 5 is provided, the
        coloredness of every 5th frame will be printed to the
        console
        """
        super(Histogrammer, self).__init__()
        assert channel in range(3)
        assert 0 < frame_rate <= 60
        assert 0 <= display_freq
        self.channel = channel
        self.buffer = np_buffer
        self.tag = tag
        self.display_freq = display_freq
        self.num_skipped = 0
        self.sleep_interval = 1.0 / frame_rate
        self.running = False

    def run(self):
        """Override ``Thread.run``."""
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
        """Stop the thread."""
        self.running = False


class Dyer(IObservableObserver):
    """Dyes specified channel of an incoming video frame."""

    def __init__(self, channel, increment):
        """
        :param channel: image channel to dye
        :param increment: how much to dye the image channel
        """
        super(Dyer, self).__init__()
        assert 0 <= channel < 3
        assert 0 <= increment < 256
        self.channel = channel
        self.increment = increment

    def update(self, frame):
        """Implement ``IObservableObserver.update``."""
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

    # initialise reading of passed file
    sfac = VideoSourceFactory.get_instance()
    reader = sfac.create_file_reader(in_file, ColourSpace.BGRA)
    frame = VideoFrame(ColourSpace.BGRA, False)
    reader.get_frame(frame)
    frame_shape = (frame.rows(), frame.cols(), 4)

    # prepare for creating encoders (writers)
    tfac = VideoTargetFactory.get_instance()
    frame_rate = reader.get_frame_rate()

    # create a red and green Dyer
    red_dyer = Dyer(2, 128)
    green_dyer = Dyer(1, 64)

    # create the bufferer for the red and green Dyers
    np_buffer_red = np.zeros(frame_shape, np.uint8)
    bufferer_red = Bufferer(np_buffer_red)
    np_buffer_orig = np.zeros_like(np_buffer_red)
    bufferer_orig = Bufferer(np_buffer_orig)

    # create the histogrammers for the red-dyed and
    # the original video frames, and start them
    hist_red = Histogrammer(np_buffer_red, 2, 'red-dyed', 60.0, 10)
    hist_red.start()
    hist_orig = Histogrammer(np_buffer_orig, 2, 'original', 50.0, 10)
    hist_orig.start()

    # create encoders for the red-dyed and yellow-dyed (as green
    # is applied on top of red) video streams
    red_file = os.path.join('.', ''.join([filename, '-red', ext]))
    red_writer = tfac.create_file_writer(Codec.HEVC, red_file, frame_rate)
    yellow_file = os.path.join('.', ''.join([filename, '-yellow', ext]))
    yellow_writer = tfac.create_file_writer(Codec.HEVC, yellow_file, frame_rate)

    # create a SnapshotSavers for saving a number of yellow-dyed
    # video frames
    yellow_snapshots = SnapshotSaver('.')

    # assemble the GIFT-Grab pipeline
    reader.attach(bufferer_orig)
    bufferer_orig.attach(red_dyer)
    red_dyer.attach(red_writer)
    red_dyer.attach(bufferer_red)
    red_dyer.attach(green_dyer)
    green_dyer.attach(yellow_writer)
    green_dyer.attach(yellow_snapshots)

    sleep(20)  # operate pipeline for 20 sec

    # stop the histogrammers
    hist_red.stop()
    hist_orig.stop()

    # disassemble the GIFT-Grab pipeline
    reader.detach(bufferer_orig)
    bufferer_orig.detach(red_dyer)
    red_dyer.detach(red_writer)
    red_dyer.detach(bufferer_red)
    red_dyer.detach(green_dyer)
    green_dyer.detach(yellow_writer)
    green_dyer.detach(yellow_snapshots)
