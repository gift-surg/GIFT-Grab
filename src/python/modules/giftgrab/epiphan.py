#!/usr/bin/env python

from time import sleep, time, strftime
from datetime import timedelta
import yaml
from os.path import join, split, dirname, abspath
from os import makedirs
from random import choice
from string import ascii_uppercase
import logging
import pygiftgrab


MAX_X = 1920
MAX_Y = 1080


class Recorder:

    """A video recorder that can be started, paused, resumed, and stopped.

    When everything just works, this recorder will connect to either the DVI or SDI
    `port` of an Epiphan DVI2PCIe Duo, start grabbing frames there at the specified
    `frame_rate`, and save them using H265 (HEVC) encoding in an MP4 container
    pointed to by the specified `file_path`, which serves only as a prefix, i.e.
    for each resume-pause a new file will be created with a recording index and
    the file extension appended. After each video file has been finalised, a text
    file will be created as well that will contain the total duration for that
    video file.

    This recorder class will attempt to perform the requested operations within a
    `timeout_limit`. As such, after the timeout, the operations will fail and this
    will be propagated to the client through error messages. The recorder will
    simply stop at this point. Note that this recorder class does not attempt to be
    intelligent at all. As such, it is the client's responsibility to properly
    set the mentioned parameters.

    """

    def __init__(self, port, colour_space, frame_rate, file_path, timeout_limit=10):
        """Initialise the recorder with desired configuration.

        The recorder will not run if file writer cannot be created.

        @param port `pygiftgrab.Device.DVI2PCIeDuo_SDI` or
        `pygiftgrab.Device.DVI2PCIeDuo_DVI`
        @param colour_space `pygiftgrab.ColourSpace.BGRA` or
        `pygiftgrab.ColourSpace.I420`
        @param frame_rate Epiphan DVI2PCIe Duo supports up to 60 fps
        at ``1920 x 1080``, when using a single port, and 37 fps when using
        both ports (when capturing in the RGB24 colour space)
        @param file_path any referenced directories must exist and be
        writable by user. If a relative directory is specified, it is
        internally converted to an absolute one
        @param timeout_limit 5 attempts will be made for critical operations
        within this number of seconds (recorder will not start if this is
        negative)
        """

        self.timeout_limit = timeout_limit
        self.max_num_attempts = 5
        self.inter_attempt_duration = self.timeout_limit / self.max_num_attempts  # sec
        self.port = port
        self.colour_space = colour_space
        self.name = str(self.port)
        self.file = None
        self.is_running = False
        self.file_path = abspath(file_path)
        self.recording_index = 0
        self.frame_rate = frame_rate
        self.is_recording = False
        self.started_at = 0
        self.sub_frame = None
        self.device = None
        self.black_frame = None
        if self.timeout_limit > 0 \
           and (self.colour_space == pygiftgrab.ColourSpace.BGRA or \
                self.colour_space == pygiftgrab.ColourSpace.I420):
            self.is_running = True

    def start(self):
        """Connect to specified `port` and start data acquisition until
        `stop()`ped.

        Will simply do nothing if not `is_running`.
        """

        try:
            # mitigate re-start risk
            if not self.is_running:
                return

            if not self.__connect_device():
                return

            self.resume_recording()  # i.e. start recording

        except BaseException as e:
            logging.error('Unspecific exception on starting: ' + e.message +
                          ', aborting recording.')

    def stop(self):
        """Stop a `start()`ed recorder and disconnect from the
        Epiphan `port`.

        Will simply do nothing if not `is_running`.
        """
        if not self.is_running:
            return

        self.pause_recording()

        if self.__disconnect_device():
            self.is_running = False

    def pause_recording(self):
        """Pause a `start()`ed recorder and finalise the current
        video `file`.

        @see resume_recording()
        """
        # TODO - this line is quick hack because of GiftGrab#37
        if not self.is_running:
            return

        if not self.is_recording:
            return

        if self.__stop_acquisition():
            self.is_recording = False

            try:
                self.file.finalise()
                del self.file
                self.file = None
            except RuntimeError as e:
                logging.error(e.message)

            # write timing report as well
            try:
                report_file = open(self.__video_filename() + '.timing.yml', 'w')
                timing_report = dict(elapsed=str(timedelta(seconds=time() - self.started_at)))
                report_file.write(yaml.dump(timing_report, default_flow_style=False))
                report_file.close()
            except (IOError, yaml.YAMLError) as e:
                logging.error(
                    'Intermediate report generation failed with: ' +
                    e.message
                )


    def resume_recording(self):
        """Resume a `start()`ed and paused recorder.

        This will create and open a new video `file`.

        @see pause_recording()
        """
        # TODO - this line is quick hack because of GiftGrab#37
        if not self.is_running:
            return

        if self.is_recording:
            return

        if self.device:
            # TODO - rows and cols setting because of GiftGrab#40
            if self.sub_frame:
                self.device.set_sub_frame(self.sub_frame[0], self.sub_frame[1],
                                          self.sub_frame[2], self.sub_frame[3])
                cols = self.sub_frame[2]
                rows = self.sub_frame[3]
            else:
                self.device.get_full_frame()
                cols = MAX_X
                rows = MAX_Y

            tmp_frame = pygiftgrab.VideoFrame(self.colour_space, False)
            got_frame = self.device.get_frame(tmp_frame)
            if got_frame:
                cols = tmp_frame.cols()
                rows = tmp_frame.rows()
            self.black_frame = pygiftgrab.VideoFrame(self.colour_space, cols, rows)
        else:
            return

        if self.__create_video_writer():
            if self.__init_video_writer():
                if self.__start_acquisition():
                    self.started_at = time()
                    self.is_recording = True

    def set_sub_frame(self, x, y, width, height):
        """Set region of interest to record.

        This will have no effect if called in the middle of
        a recording.

        @param x upper-left corner coordinate
        @param y upper-left corner coordinate
        @param width
        @param height
        @throw ValueError if requested ROI values invalid,
        either due to negativity, or ouf-of-bounds (currently
        only up to ``1920 x 1080`` supported)
        """
        # TODO - hard-coded because of GiftGrab#42
        if not self.is_recording:
            if 0 <= x <= MAX_X and 0 <= y <= MAX_Y and \
               0 < width <= MAX_X and 0 < height <= MAX_Y and \
               x + width <= MAX_X and y + height <= MAX_Y:
                self.sub_frame = [x, y, width, height]
            else:
                raise ValueError('ROI ' + str(x) + ', ' + str(y) +
                                 ', ' + str(width) + ', ' + str(height) +
                                 ' invalid')

    def set_full_frame(self):
        """Tell recorder to use full resolution frames.

        This will have no effect if called in the middle of
        a recording.
        """
        if not self.is_recording:
            self.sub_frame = None

    def __start_acquisition(self):
        """Attach the current file writer to current device.

        @return ``True`` on success, ``False`` otherwise
        """
        try:
            self.device.attach(self.file)
        except RuntimeError as e:
            logging.error(
                'Could not start acquisition due to {}'.format(e.message)
            )
            return False
        else:
            return True

    def __stop_acquisition(self):
        """Detach the current file writer from current device.

        @return ``True`` on success, ``False`` otherwise
        """
        try:
            self.device.detach(self.file)
        except RuntimeError as e:
            logging.error(
                'Could not stop acquisition due to {}'.format(e.message)
            )
            return False
        else:
            return True

    def __video_filename(self):
        """Report what video file to record to.

        Automatically generate the full file path from
        `file_path` and `recording_index`.

        @return e.g. ``file_path-000003.mp4``
        """
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
                self.file = pygiftgrab.Factory.writer(pygiftgrab.Storage.File_HEVC)
            except RuntimeError as e:
                logging.error(
                    'Attempt #' + str(attempts) + ' of ' +
                    str(self.max_num_attempts) +
                    ' to get a video writer failed with: ' +
                    e.message
                )
                sleep(self.inter_attempt_duration)
                continue
            else:
                return True
        return False

    def __init_video_writer(self):
        """Attempt to initialise video writer with a new filename.

        @return ``True`` upon success, ``False`` otherwise
        """
        self.recording_index += 1
        filename = self.__video_filename()
        attempts = 0
        while attempts < self.max_num_attempts:
            attempts += 1
            try:
                self.file.init(filename, self.frame_rate)
            except RuntimeError as e:
                logging.error(
                    'Attempt #' + str(attempts) + ' of ' +
                    str(self.max_num_attempts) +
                    ' to initialise video writer failed with: ' +
                    e.message
                )
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
                self.device = pygiftgrab.Factory.connect(self.port, self.colour_space)
            except IOError as e:
                logging.error(
                    'Attempt #' + str(attempts) + ' of ' +
                    str(self.max_num_attempts) +
                    ' to connect to ' + str(self.port) +
                    ' failed with: ' + e.message
                )
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
                logging.error(
                    'Attempt #' + str(attempts) + ' of ' +
                    str(self.max_num_attempts) +
                    ' to disconnect from ' + str(self.port) +
                    ' failed with: ' + e.message
                )
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


def __colour_space_to_str(epiphan_colour_space):
    """Get string representation of given `epiphan_colour_space`.

    @param epiphan_colour_space
    @return
    @throw ValueError if invalid value given
    """
    if epiphan_colour_space == pygiftgrab.ColourSpace.BGRA:
        return 'BGRA'
    elif epiphan_colour_space == pygiftgrab.ColourSpace.I420:
        return 'I420'
    else:
        raise ValueError('Could not recognise colour space ' +
                         str(epiphan_colour_space))


def __str_to_colour_space(epiphan_colour_space):
    """Get actual colour space value from given `epiphan_colour_space` string.

    @param epiphan_colour_space
    @return
    @throw ValueError if invalid string given
    """
    if epiphan_colour_space == 'BGRA':
        return pygiftgrab.ColourSpace.BGRA
    elif epiphan_colour_space == 'I420':
        return pygiftgrab.ColourSpace.I420
    else:
        raise ValueError('Could not recognise colour space ' +
                         epiphan_colour_space)


def parse(file_path):
    """Parse `Recorder` configuration from given YAML file.

    Also create a session folder for this `Recorder`, using
    corresponding tag of given YAML file.

    @param file_path
    @return a ready-to-start `Recorder` on success
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
           or not data['timeout_limit'] or not data['port'] \
           or not data['colour_space']:
            raise yaml.YAMLError('Could not parse ' + file_path)
        colour_space = __str_to_colour_space(data['colour_space'])
        frame_rate = data['frame_rate']
        if colour_space == pygiftgrab.ColourSpace.BGRA:
            max_frame_rate = 29
        elif colour_space == pygiftgrab.ColourSpace.I420:
            max_frame_rate = 59
        if not 0 < frame_rate <= max_frame_rate:
            raise ValueError('Supporting only up to ' + str(max_frame_rate) +
                             ' fps when using colour space ' +
                             data['colour_space'] + ' (' + str(frame_rate) + ' given)')
        timeout_limit = data['timeout_limit']
        if not 0 < timeout_limit <= 20:
            raise ValueError('Timeout should be positive, ' +
                             'and up to 20 sec.')
        port = __str_to_port(data['port'])
        data_file_path = abspath(data['file_path'])
        unique_file_path = None
        max_attempts = 5
        attempt = 0
        while True:
            attempt += 1
            try:
                unique_file_path = __session_folder(path_prefix=data_file_path)
            except OSError as e:
                if attempt >= max_attempts:
                    raise e
                else:
                    logging.error(e.message)
            else:
                break

        # create and return recorder
        return Recorder(port=port, colour_space=colour_space, frame_rate=frame_rate,
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
        colour_space = __colour_space_to_str(recorder.colour_space)
        data = dict(port=port,
                    colour_space=colour_space,
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
