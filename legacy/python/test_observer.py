# import pygiftgrab as gg
import time
import datetime
import pytest


class Observer:
    """To be replaced by pygiftgrab.Observer
    """

    def update(self):
        print('Observer-update')


class Observable:
    """To be replaced by pygiftgrab.Observable
    """

    def __init__(self):
        self._observers = []

    def attach(self, observer):
        if observer not in self._observers:
            self._observers.append(observer)

    def detach(self, observer):
        try:
            self._observers.remove(observer)
        except ValueError:
            pass

    def notify(self, modifier=None):
        for observer in self._observers:
            if modifier != observer:
                observer.update()


class DummyObservable(Observable):
    def dummy_run(self, frame_rate, duration):
        d_time = (1.0 - 0.1) / frame_rate
        n = duration * frame_rate
        for i in range(n):
            self.notify()
            time.sleep(d_time)


class FrameRateTimer(Observer):
    """Descendant of GIFT-Grab's `Observer`, which
    will listen to Observable's for some time and
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
def test_frame_rate(frame_rate, observers):
    timers = []
    for i in range(observers):
        timers.append(FrameRateTimer(frame_rate))
    source = DummyObservable()
    # source = gg.Factory.connect(gg.Device.DVI2PCIeDuo_DVI,
    #                             gg.ColourSpace.I420)
    for timer in timers:
        source.attach(timer)
    source.dummy_run(frame_rate, 30)  # TODO remove
    for timer in timers:
        assert timer
    # time.sleep(120)
    for timer in timers:
        source.detach(timer)
    del timers[:]
    # gg.Factory.disconnect(gg.Device.DVI2PCIeDuo_DVI)
