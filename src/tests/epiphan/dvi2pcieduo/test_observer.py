import pytest
try:
    # in case of PyPI installation, this will work:
    from giftgrab.tests.utils import FrameRateTimer
except ImportError:
    # in case of installation from source, this will work:
    from utils import FrameRateTimer
import pygiftgrab as pgg
import time


@pytest.mark.observer_pattern
def test_frame_rate(port, colour_space, frame_rate, observers):
    timers = []
    init_time = 1  # sec
    for i in range(observers):
        # discard 1 sec of initial data
        timers.append(FrameRateTimer(frame_rate, init_time))
    factory = pgg.VideoSourceFactory.get_instance()
    source = factory.get_device(port, colour_space)
    for timer in timers:
        source.attach(timer)

    time.sleep(init_time + 60)  # listen for data these nr. of sec
    for timer in timers:
        source.detach(timer)

    for timer in timers:
        assert timer
