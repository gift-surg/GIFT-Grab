import time
from pytest import mark
from utils import StereoFrameChecker
import pygiftgrab as pgg


@mark.stereo_frames
def test_stereo_frames(device, colour_space):
    factory = pgg.VideoSourceFactory.get_instance()
    source = factory.get_device(device, colour_space)
    checker = StereoFrameChecker()
    source.attach(checker)

    time.sleep(15)

    source.detach(checker)
    assert checker
