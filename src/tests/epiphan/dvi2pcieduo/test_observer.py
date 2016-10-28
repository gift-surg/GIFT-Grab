# import pygiftgrab as gg
import time
import datetime
import numpy as np


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
        d_time = (1.0 - 0.5) / frame_rate
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
        diffs = np.array(map(
            (lambda p: (p[1] - p[0]).microseconds / 1000.0),
            pairs)
        )
        del self._timestamps[:]
        return np.max(diffs) <= 1000.0 / self._frame_rate

    def __nonzero__(self):
        if self.__bool__():
            return 1
        else:
            return 0


def test_frame_rate(frame_rate):
    timer_1 = FrameRateTimer(frame_rate)
    timer_2 = FrameRateTimer(frame_rate)
    source = DummyObservable()
    # source = gg.Factory.connect(gg.Device.DVI2PCIeDuo_DVI,
    #                             gg.ColourSpace.I420)
    source.attach(timer_1)
    source.attach(timer_2)
    source.dummy_run(frame_rate, 30)  # TODO remove
    assert timer_1
    assert timer_2
    source.dummy_run(frame_rate / 2, 30)  # TODO remove
    assert not timer_1
    assert not timer_2
    source.dummy_run(frame_rate * 2, 30)  # TODO remove
    assert timer_1
    assert timer_2
    # time.sleep(120)
    source.detach(timer_1)
    source.detach(timer_2)
    # gg.Factory.disconnect(gg.Device.DVI2PCIeDuo_DVI)


test_frame_rate(60)
test_frame_rate(30)
