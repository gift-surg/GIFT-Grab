from pygiftgrab import IObservableObserver
from pygiftgrab import VideoTargetFactory
from pygiftgrab import Codec
from pygiftgrab import VideoSourceOpenCV
import numpy as np
import time
import os
import os.path
from pytest import mark


class ChannelSwapperBGRA(IObservableObserver):

    def __init__(self, channel_1, channel_2):
        super(ChannelSwapperBGRA, self).__init__()
        self._displayed = False
        if channel_1 > 4 or channel_2 > 4:
            raise ValueError('Channels: 0, 1, 2, 3 (B, G, R, A)')
        if channel_1 == channel_2:
            raise ValueError('Swapping same channel has no effect')
        self._channel_1 = channel_1
        self._channel_2 = channel_2

    def update(self, frame):
        data_np = frame.data()
        if not self._displayed:
            self._displayed = True
        data_np[self._channel_1::4], data_np[self._channel_2::4] = \
            data_np[self._channel_2::4], data_np[self._channel_1::4]


@mark.observer_pattern
def test_file_to_file_pipeline(filepath):
    """Read a file, process its frames, and save them to a new
    file.

    Only checks new video file has positive size. That is, its
    content is not checked.
    """
    channel_1 = 0
    channel_2 = 1
    swapper = ChannelSwapperBGRA(channel_1, channel_2)
    file_reader = VideoSourceOpenCV(filepath)
    target_fac = VideoTargetFactory.get_instance()
    filename, extension = os.path.splitext(os.path.basename(filepath))
    out_filepath = '{}-swapped-{}-{}{}'.format(
        filename, channel_1, channel_2, extension
    )
    if os.path.isfile(out_filepath):
        os.remove(out_filepath)
    assert not os.path.exists(out_filepath)
    file_writer = target_fac.create_file_writer(
        Codec.Xvid, out_filepath,
        file_reader.get_frame_rate()
    )
    swapper.attach(file_writer)
    file_reader.attach(swapper)

    time.sleep(2)

    file_reader.detach(swapper)
    swapper.detach(file_writer)
    assert os.path.exists(out_filepath)
    assert os.path.getsize(out_filepath) > 0
