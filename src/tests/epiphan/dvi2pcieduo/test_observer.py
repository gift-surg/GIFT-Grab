import time
import datetime
import pytest
import pygiftgrab as pgg


class FrameRateTimer(pgg.Observer):
    """Descendant of GIFT-Grab's `Observer`, which
    will listen to `Observable`s for some time and
    when asked, will report whether data has been
    sent at the specified frame rate.
    """

    def __init__(self, frame_rate):
        self._frame_rate = frame_rate
        self._timestamps = []

    def update(self):
        self._timestamps.append(
            datetime.datetime.now()
        )

    def __bool__(self):
        """Check if updates have been in time intervals
        in line with defined frame rate, also resetting
        all saved timestamps, i.e. ready for next round.
        """
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
    source = pgg.Factory.connect(port, colour_space)
    for timer in timers:
        source.attach(timer)

    time.sleep(120)  # listen for data these nr. of sec
    for timer in timers:
        assert timer

    for timer in timers:
        source.detach(timer)
    del timers[:]
    pgg.Factory.disconnect(port)
