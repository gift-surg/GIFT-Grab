from pytest import mark
try:
    # in case of PyPI installation, this will work:
    from giftgrab.tests.utils import FrameRateTimer
except ImportError:
    # in case of installation from source, this will work:
    from utils import FrameRateTimer
import pygiftgrab as pgg
import time


@mark.observer_pattern
def test_frame_rate(colour_space, frame_rate, observers):
    timers = []
    for i in range(observers):
        timers.append(FrameRateTimer(frame_rate))
    factory = pgg.VideoSourceFactory.get_instance()
    source = factory.get_device(pgg.Device.DeckLinkSDI4K, colour_space)

    for timer in timers:
        source.attach(timer)
    time.sleep(120)  # listen for data these nr. of sec
    for timer in timers:
        source.detach(timer)
    for timer in timers:
        assert timer
