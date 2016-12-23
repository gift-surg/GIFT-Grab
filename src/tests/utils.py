import time
import datetime
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

    def __init__(self, frame_rate, init_time = 0):
        super(FrameRateTimer, self).__init__()
        self._frame_rate = frame_rate
        # all data for this initial period (in sec) will be discarded
        self._init_time = init_time
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
        n_init_items = int(self._init_time * self._frame_rate)
        if use_numpy:
            if n_init_items >= self._timestamps.size:
                raise IndexError('Not enough data collected')
        else:
            if n_init_items >= len(self._timestamps):
                raise IndexError('Not enough data collected')
        timestamps = self._timestamps[n_init_items:]

        if use_numpy:
            intervals = timestamps[1:] - timestamps[0]
            frame_rates = np.array(
                [(i + 1) / (interval / np.timedelta64(1, 's')) 
                    for i, interval in enumerate(intervals)]
            )
            return np.min(frame_rates) >= self._frame_rate
        else:
            intervals = [
                (timestamp - timestamps[0]).total_seconds()
                    for timestamp in timestamps[1:]
            ]
            frame_rates = [(i + 1) / interval for i, interval in enumerate(intervals)]
            return min(frame_rates) >= self._frame_rate

    def __nonzero__(self):
        if self.__bool__():
            return 1
        else:
            return 0
