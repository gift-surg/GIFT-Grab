from pygiftgrab import IObservableObserver
from pygiftgrab import VideoTargetFactory
from pygiftgrab import Codec
from pygiftgrab import ColourSpace
from pygiftgrab import VideoSourceFactory
import numpy as np
import time
import os
import os.path
from pytest import mark, yield_fixture
import scipy.ndimage as ndimage


class ChannelSwapperBGRA(IObservableObserver):

    def __init__(self, channel_1, channel_2):
        super(ChannelSwapperBGRA, self).__init__()
        if channel_1 > 4 or channel_2 > 4:
            raise ValueError('Channels: 0, 1, 2, 3 (B, G, R, A)')
        if channel_1 == channel_2:
            raise ValueError('Swapping same channel has no effect')
        self._channel_1 = channel_1
        self._channel_2 = channel_2

    def update(self, frame):
        data_np = frame.data()
        data_np[self._channel_1::4], data_np[self._channel_2::4] = \
            data_np[self._channel_2::4], data_np[self._channel_1::4]


class GaussianSmootherBGRA(IObservableObserver):

    def __init__(self):
        super(GaussianSmootherBGRA, self).__init__()

    def update(self, frame):
        data_np = frame.data(True)
        # 3. sigma component 0 => we don't want to smooth
        # across the BGRA channels
        ndimage.gaussian_filter(input=data_np, sigma=(5, 15, 0),
                                order=0, output=data_np)


factory = None


@yield_fixture(scope='session')
def peri_test():
    global factory
    factory = VideoSourceFactory.get_instance()
    yield


@mark.observer_pattern
@mark.usefixtures('peri_test')
def test_file_to_file_pipeline_simple(filepath):
    """Read a file, process its frames very simply in NumPy,
    i.e. not using any advanced image processing features, and
    save them to a new file.

    Only checks new video file has positive size. That is, its
    content is not checked.
    """
    channel_1 = 0
    channel_2 = 1
    swapper = ChannelSwapperBGRA(channel_1, channel_2)
    global factory
    file_reader = factory.create_file_reader(
        filepath, ColourSpace.BGRA
    )
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
        15  # was: file_reader.get_frame_rate() (changed due to issue #154)
    )
    swapper.attach(file_writer)
    file_reader.attach(swapper)

    time.sleep(2)

    file_reader.detach(swapper)
    swapper.detach(file_writer)
    assert os.path.exists(out_filepath)
    assert os.path.getsize(out_filepath) > 0


@mark.observer_pattern
@mark.usefixtures('peri_test')
def test_file_to_file_pipeline_scipy(filepath):
    """Read a file, process its frames using advanced image
    processing features of SciPy, and save them to a new file.

    Only checks new video file has positive size. That is, its
    content is not checked.
    """
    smoother = GaussianSmootherBGRA()
    global factory
    file_reader = factory.create_file_reader(
        filepath, ColourSpace.BGRA
    )
    target_fac = VideoTargetFactory.get_instance()
    filename, extension = os.path.splitext(os.path.basename(filepath))
    out_filepath = '{}-smoothed{}'.format(filename, extension)
    if os.path.isfile(out_filepath):
        os.remove(out_filepath)
    assert not os.path.exists(out_filepath)
    file_writer = target_fac.create_file_writer(
        Codec.Xvid, out_filepath,
        15  # was: file_reader.get_frame_rate() (changed due to issue #154)
    )
    smoother.attach(file_writer)
    file_reader.attach(smoother)

    time.sleep(2)

    file_reader.detach(smoother)
    smoother.detach(file_writer)
    assert os.path.exists(out_filepath)
    assert os.path.getsize(out_filepath) > 0
