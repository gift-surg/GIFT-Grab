import time
import datetime
import pytest
import pygiftgrab as pgg

use_numpy = True
try:
    import numpy as np
except ImportError:
    use_numpy = False


class FrameRateTimer(pgg.IObserver):
    """Descendant of GIFT-Grab's `Observer`, which
    will listen to `Observable`s for some time and
    when asked, will report whether data has been
    sent at the specified frame rate.
    """

    def __init__(self, frame_rate):
        super(FrameRateTimer, self).__init__()
        self._frame_rate = frame_rate
        if use_numpy:
            self._timestamps = np.array([], dtype='datetime64[us]')
        else:
            self._timestamps = []

    def update(self, frame):
        if use_numpy:
            self._timestamps = np.append(self._timestamps,
                np.datetime64(datetime.datetime.now()))
        else:
            self._timestamps.append(datetime.datetime.now())

    def __bool__(self):
        """Check if updates have been in time intervals
        in line with defined frame rate, also resetting
        all saved timestamps, i.e. ready for next round.
        """
        global use_numpy
        if use_numpy:
            diffs = self._timestamps[1:] - self._timestamps[:-1]
            if len(diffs) == 0:
                return False
            return np.count_nonzero(
                       diffs > np.timedelta64(1000000 / self._frame_rate, 'us')
                   ) == 0
        else:
            pairs = zip(self._timestamps[:-1], self._timestamps[1:])
            diffs = map(
                (lambda p: (p[1] - p[0]).microseconds / 1000.0),
                pairs
            )
            del self._timestamps[:]
            return max(diffs) <= 1000.0 / self._frame_rate

    def __nonzero__(self):
        if self.__bool__():
            return 1
        else:
            return 0


@pytest.mark.observer_pattern
def test_frame_rate(port, colour_space, frame_rate, observers):
    timers = []
    for i in range(observers):
        timers.append(FrameRateTimer(frame_rate))
    factory = pgg.VideoSourceFactory.get_instance()
    source = factory.get_device(port, colour_space)
    for timer in timers:
        source.attach(timer)

    time.sleep(60)  # listen for data these nr. of sec
    for timer in timers:
        source.detach(timer)

    for timer in timers:
        assert timer
