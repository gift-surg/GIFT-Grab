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


class VideoFrameDesc:
    """Descriptor that memorises the specs of a
    given video frame, without copying its data.
    """

    def __init__(self, frame):
        self.cols = frame.cols()
        self.rows = frame.rows()
        self.colour = frame.colour()
        self.data_length = frame.data_length()


class FileChecker(pgg.IObserver):
    """Descendant of GIFT-Grab's `Observer`, which
    listens to an `Observable` reading video frames
    from files and reports the file reader's specs,
    e.g. frame rate, frame count.
    """

    def __init__(self, file_reader):
        super(FileChecker, self).__init__()
        self._file_reader = file_reader
        self._frame_descs = []

    def attach(self):
        if self._file_reader is not None:
            self._file_reader.attach(self)

    def detach(self):
        if self._file_reader is not None:
            self._file_reader.detach(self)

    def update(self, frame):
        self._frame_descs.append(VideoFrameDesc(frame))

    def assert_data(self):
        return len(self._frame_descs) > 0

    def assert_colour(self, colour):
        for frame_desc in self._frame_descs:
            if frame_desc.colour != colour:
                return False
        return True

    def assert_frame_rate(self, frame_rate):
        if self._file_reader.get_frame_rate() != frame_rate:
            return False
        else:
            return True

    def assert_frame_dimensions(self,
                                frame_width, frame_height):
        for frame_desc in self._frame_descs:
            if frame_desc.cols != frame_width or\
               frame_desc.rows != frame_height:
                return False
        return True

    def assert_frame_data_lengths(self, colour,
                                  frame_width, frame_height):
        exp_data_length = pgg.VideoFrame.required_data_length(
            colour, frame_width, frame_height
            )

        for frame_desc in self._frame_descs:
            data_length = frame_desc.data_length
            if data_length != exp_data_length:
                return False
        return True
