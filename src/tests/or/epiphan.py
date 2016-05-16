#!/usr/bin/env python

from threading import Thread
from time import sleep, time, strftime
from datetime import timedelta
import yaml
from os.path import join, split, dirname
from os import makedirs
from random import choice
from string import ascii_uppercase
import pygiftgrab


class Recorder(Thread):

    """A video recording thread that can be started, paused, resumed, and stopped.

    When everything just works, this thread will connect to either the DVI or SDI
    `port` of an Epiphan DVI2PCIe Duo, grab frames there at the specified
    `frame_rate`, and save them using H265 (HEVC) encoding in an MP4 container
    pointed to by the specified `file_path`, which serves only as a prefix, i.e.
    for each resume-pause a new file will be created with a recording index and
    the file extension appended. After each video file has been finalised, a text
    file will be created as well that will contain the total latency for that
    video file.

    This thread class will attempt to perform the requested operations within a
    `timeout_limit`. As such, after the timeout, the operations will fail and this
    will be propagated to the client through error messages. The thread will
    simply stop at this point. Note that this thread class does not try to be
    intelligent at all. As such, it is the client's responsibility to properly
    set the mentioned parameters.

    """

    def __init__(self, port, frame_rate, file_path, timeout_limit=10):
        """Initialise thread with desired configuration.

        Thread will not run if file writer cannot be created.

        @param port `pygiftgrab.Device.DVI2PCIeDuo_SDI` or
        `pygiftgrab.Device.DVI2PCIeDuo_DVI`
        @param frame_rate Epiphan DVI2PCIe Duo supports up to 60 fps
        at ``1920 x 1080``, when using a single port, and 37 fps when using
        both ports (when capturing in the RGB24 colour space)
        @param file_path any referenced directories must exist and be
        writable by user
        @param timeout_limit 5 attempts will be made for critical operations
        within this number of seconds (thread will not start if this is
        negative)
        """

        self.timeout_limit = timeout_limit
        self.max_num_attempts = 5
        self.inter_attempt_duration = self.timeout_limit / self.max_num_attempts  # sec
        self.port = port
        self.file = None
        self.is_running = False
        self.file_path = file_path
        self.recording_index = 0
        self.frame_rate = frame_rate
        self.is_recording = False
        self.latency = 0.0  # sec
        self.started_at = 0
        self.sub_frame = None
        self.device = None
        self.black_frame = None
        if self.__create_video_writer() and self.timeout_limit > 0:
            self.is_running = True
        Thread.__init__(self)

    def run(self):
        """Connect to specified `port` and start looping until `stop()`ped.

        Will simply do nothing if not `is_running`.
        """

        # mitigate re-start risk
        if not self.is_running:
            return

        if not self.__connect_device():
            return

        inter_frame_duration = self.__inter_frame_duration()
        frame = pygiftgrab.VideoFrame_BGRA(False)
        self.resume_recording()  # i.e. start recording

        while self.is_running:
            start = time()
            if self.is_recording:
                got_frame = self.device.get_frame(frame)

                try:
                    if got_frame:
                        self.file.append(frame)
                    else:
                        print 'Could not read video stream, appending black frame'
                        self.file.append(self.black_frame)
                except RuntimeError as e:
                    # TODO - consider pausing and resuming instead
                    print 'Appending frame failed with: ' + e.message +\
                          ', aborting recording from ' + str(self.port)
                    self.stop()
                    continue

            sleep_duration = inter_frame_duration - (time() - start)
            if sleep_duration > 0:
                sleep(sleep_duration)
            else:
                self.latency -= sleep_duration

        self.__disconnect_device()

    def stop(self):
        """Tell a `run()`ning thread to stop.

        Will simply do nothing if not `is_running`.
        """
        if not self.is_running:
            return

        self.pause_recording()
        self.is_running = False

    def pause_recording(self):
        """Tell a `run()`ning thread to pause recording.

        This will finalise the current video `file`.

        @see resume_recording()
        """
        # TODO - this line is quick hack because of GiftGrab#37
        if not self.is_running:
            return

        if not self.is_recording:
            return

        self.is_recording = False
        # sleep to allow for stop to be picked up
        sleep(2 * self.__inter_frame_duration())

        try:
            self.file.finalise()
        except RuntimeError as e:
            print e.message
        # write timing report as well
        report_file = open(self.__next_filename(increment_index=False) +
                           '.timing.yml', 'w')
        timing_report = dict(elapsed=str(timedelta(seconds=time() - self.started_at)),
                             latency=str(timedelta(seconds=self.latency)))
        report_file.write(yaml.dump(timing_report, default_flow_style=False))
        report_file.close()

        self.latency = 0.00

    def resume_recording(self):
        """Tell a `run()`ning and paused thread to resume recording.

        This will create and open a new video `file`.

        @see pause_recording()
        """
        # TODO - this line is quick hack because of GiftGrab#37
        if not self.is_running:
            return

        if self.is_recording:
            return

        if self.device:
            if self.sub_frame:
                self.device.set_sub_frame(self.sub_frame[0], self.sub_frame[1],
                                          self.sub_frame[2], self.sub_frame[3])
            else:
                self.device.get_full_frame()

            # TODO - following two lines because of GiftGrab#40
            tmp_frame = pygiftgrab.VideoFrame_BGRA(False)
            self.device.get_frame(tmp_frame)
            self.black_frame = pygiftgrab.VideoFrame_BGRA(tmp_frame.rows(), tmp_frame.cols())
        else:
            return

        if self.__init_video_writer():
            self.started_at = time()
            self.is_recording = True
        else:
            self.is_recording = False

    def set_sub_frame(self, x, y, width, height):
        """Set region of interest to record.

        This will have no effect if called in the middle of
        a recording. Also no safety checks are made for the
        boundaries.

        @param x upper-left corner coordinate
        @param y upper-left corner coordinate
        @param width
        @param height
        """
        if not self.is_recording:
            self.sub_frame = [x, y, width, height]

    def set_full_frame(self):
        """Tell thread to use full resolution frames.

        This will have no effect if called in the middle of
        a recording.
        """
        if not self.is_recording:
            self.sub_frame = None

    def __next_filename(self, increment_index=True):
        """Report what video file to record to.

        Automatically generate the full file path from
        `file_path` and `recording_index`.

        @param increment_index if ``True``, then increments
        index as well, i.e. use ``False`` only when checking
        what video file currently recording to.
        @return e.g. ``file_path-000003.mp4``
        """
        if increment_index:
            self.recording_index += 1
        return self.file_path + '-' + \
            '{0:06d}'.format(self.recording_index) + \
            '.mp4'

    def __inter_frame_duration(self):
        """Compute time needed between frames based on `frame_rate`.

        @return
        """
        return 1.0 / self.frame_rate  # sec

    def __create_video_writer(self):
        """Attempt to create a writer for recording video files.

        @return: ``True`` if video writer created, ``False`` otherwise
        """
        attempts = 0
        while attempts < self.max_num_attempts:
            attempts += 1
            try:
                self.file = pygiftgrab.Factory.writer(pygiftgrab.Storage.File_H265)
            except RuntimeError as e:
                print 'Attempt #' + str(attempts) + ' of ' + \
                      str(self.max_num_attempts) + \
                      ' to get a video writer failed with: ' + \
                      e.message
                sleep(self.inter_attempt_duration)
                continue
            else:
                return True
        return False

    def __init_video_writer(self):
        filename = self.__next_filename()
        attempts = 0
        while attempts < self.max_num_attempts:
            attempts += 1
            try:
                self.file.init(filename, self.frame_rate)
            except RuntimeError as e:
                print 'Attempt #' + str(attempts) + ' of ' + \
                      str(self.max_num_attempts) + \
                      ' to initialise video writer failed with: ' + \
                      e.message
                sleep(self.inter_attempt_duration)
                continue
            else:
                return True
        return False

    def __connect_device(self):
        """Attempt to connect to device specified by `port`.

        @return: ``True`` if connected to `port`, ``False`` otherwise
        """
        attempts = 0
        while attempts < self.max_num_attempts:
            attempts += 1
            try:
                self.device = pygiftgrab.Factory.connect(self.port)
            except IOError as e:
                print 'Attempt #' + str(attempts) + ' of ' +\
                      str(self.max_num_attempts) +\
                      ' to connect to ' + str(self.port) +\
                      ' failed with: ' + e.message
                sleep(self.inter_attempt_duration)
                continue
            else:
                return True
        return False

    def __disconnect_device(self):
        """Attempt to disconnect from device specified by `port`.

        @return: ``True`` if disconnected from `port`, ``False`` otherwise
        """
        attempts = 0
        while attempts < self.max_num_attempts:
            attempts += 1
            try:
                pygiftgrab.Factory.disconnect(self.port)
            except IOError as e:
                print 'Attempt #' + str(attempts) + ' of ' +\
                      str(self.max_num_attempts) +\
                      ' to disconnect from ' + str(self.port) +\
                      ' failed with: ' + e.message
                sleep(self.inter_attempt_duration)
                continue
            else:
                return True
        return False


def __port_to_str(epiphan_port):
    """Get string representation of given `epiphan_port`.

    @param epiphan_port
    @return
    @throw ValueError if invalid value given
    """
    if epiphan_port == pygiftgrab.Device.DVI2PCIeDuo_SDI:
        return 'SDI'
    elif epiphan_port == pygiftgrab.Device.DVI2PCIeDuo_DVI:
        return 'DVI'
    else:
        raise ValueError('Could not recognise port ' + epiphan_port)


def __str_to_port(epiphan_port):
    """Get actual port value from given `epiphan_port` string.

    @param epiphan_port
    @return
    @throw ValueError if invalid string given
    """
    if epiphan_port == 'SDI':
        return pygiftgrab.Device.DVI2PCIeDuo_SDI
    elif epiphan_port == 'DVI':
        return pygiftgrab.Device.DVI2PCIeDuo_DVI
    else:
        raise ValueError('Could not recognise port ' + epiphan_port)


def parse(file_path):
    """Parse `Recorder` configuration from given YAML file.

    Also create a session folder for this `Recorder`, using
    corresponding tag of given YAML file.

    @param file_path
    @return a ready-to-start `Recorder` thread on success
    @throw YAMLError if YAML file cannot be parsed
    @throw IOError if `file_path` cannot be opened for
    reading
    @throw ValueError if YAML file contains invalid values
    @throw OSError if unique session folder cannot be
    created
    """
    with open(file_path, 'r') as stream:
        # parse YAML file
        data = yaml.load(stream)

        # check everything specified properly
        if not data['file_path'] or not data['frame_rate'] \
           or not data['timeout_limit'] or not data['port']:
            raise yaml.YAMLError('Could not parse ' + file_path)
        frame_rate = data['frame_rate']
        if not 0 < frame_rate <= 29:
            raise ValueError('Supporting only up to 29 fps currently ' +
                             '(' + str(frame_rate) + ' given)')
        timeout_limit = data['timeout_limit']
        if not 0 < timeout_limit <= 20:
            raise ValueError('Timeout should be positive, ' +
                             'and up to 20 sec.')
        port = __str_to_port(data['port'])
        file_path = data['file_path']
        unique_file_path = None
        max_attempts = 5
        attempt = 0
        while True:
            attempt += 1
            try:
                unique_file_path = __session_folder(path_prefix=file_path)
            except OSError as e:
                if attempt >= max_attempts:
                    raise e
                else:
                    print e.message
            else:
                break

        # create and return thread
        return Recorder(port=port, frame_rate=frame_rate,
                        file_path=unique_file_path, timeout_limit=timeout_limit)


def dump(recorder):
    """Dump given `recorder`'s configuration to a YAML file.

    The file is deduced from `recorder` configuration.

    @param recorder
    @sa parse as same format is used
    @throw YAMLError if dumping fails
    @throw IOError if `file_path` cannot be opened for
    writing
    """
    file_path = join(dirname(recorder.file_path), 'config.yml')
    with open(file_path, 'w') as stream:
        port = __port_to_str(recorder.port)
        data = dict(port=port,
                    frame_rate=recorder.frame_rate,
                    file_path=recorder.file_path,
                    timeout_limit=recorder.timeout_limit)
        stream.write(yaml.dump(data, default_flow_style=False))
        stream.close()


def __session_folder(path_prefix):
    """Attempt to create a session folder with a unique name.

    @param path_prefix last element treated as a file prefix,
    e.g. ``/my/dir/file``
    @return updated `path_prefix` after folder created
    @throw OSError if the attempt fails
    """
    path_prefix_folder, path_prefix_filename = split(path_prefix)
    unique_suffix = ''.join(choice(ascii_uppercase) for _ in range(5))
    attempted_folder = join(path_prefix_folder,
                            strftime('%Y-%m-%d-%H-%M-%S') +
                            '-' + unique_suffix)
    makedirs(attempted_folder)
    return join(attempted_folder, path_prefix_filename)
