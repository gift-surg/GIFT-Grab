#!/usr/bin/env python

from time import sleep, strptime
import pytest
import yaml
from subprocess import check_output
from os.path import isdir, dirname, isfile, join
from shutil import rmtree
from epiphan import parse, dump, BGR24, I420
import pygiftgrab


recorders = []


def frame_rate(file_path):
    """Use ``ffprobe`` to inspect frame rate of video file

    @param file_path
    @return
    @throw OSError if `file_path` does not exist, or if
    ``ffprobe`` cannot be run (e.g. not installed)
    @throw ValueError if frame rate cannot be inferred from
    ``ffprobe`` output
    """
    ffprobe_output = check_output(['ffprobe',
                                   '-v', 'error',
                                   '-of', 'flat=s=_',
                                   '-show_entries', 'stream=r_frame_rate',
                                   file_path])
    frame_rate_str = ffprobe_output.split('=')[1].strip()[1:-1].split('/')
    fr = -1
    if len(frame_rate_str) == 1:
        fr = float(frame_rate_str[0])
    elif len(frame_rate_str) == 2:
        fr = float(frame_rate_str[0])/float(frame_rate_str[1])
    else:
        raise ValueError('Frame rate cannot be inferred from ffprobe output')

    return fr


def duration(file_path):
    """Use ``ffprobe`` to inspect duration of video file

    @param file_path
    @return
    @throw OSError if `file_path` does not exist, or if
    ``ffprobe`` cannot be run (e.g. not installed)
    @throw ValueError if duration cannot be inferred from
    ``ffprobe`` output
    """
    ffprobe_output = check_output(['ffprobe',
                                   '-v', 'error',
                                   '-of', 'flat=s=_',
                                   '-show_entries', 'stream=duration',
                                   file_path])
    duration_str = ffprobe_output.split('=')[1].strip()[1:-1]
    dur = -1
    if duration_str is not None:
        dur = float(duration_str)
    else:
        raise ValueError('Duration cannot be inferred from ffprobe output')

    return dur


def resolution(file_path):
    """Use ``ffprobe`` to inspect resolution of video file

    @param file_path
    @return width, height
    @throw OSError if `file_path` does not exist, or if
    ``ffprobe`` cannot be run (e.g. not installed)
    @throw ValueError if resolution cannot be inferred from
    ``ffprobe`` output
    """
    width = -1
    height = -1
    ffprobe_commands = [ ['ffprobe',
                          '-v', 'error',
                          '-of', 'flat=s=_',
                          '-show_entries', 'stream=width',
                          file_path],
                         ['ffprobe',
                          '-v', 'error',
                          '-of', 'flat=s=_',
                          '-show_entries', 'stream=height',
                          file_path] ]
    res = []
    for ffprobe_command in ffprobe_commands:
        ffprobe_output = check_output(ffprobe_command)
        resolution_str = ffprobe_output.split('=')[1].strip()
        if resolution_str is not None:
            res.append(float(resolution_str))
        else:
            raise ValueError('Resolution cannot be inferred from ffprobe output')
    width = res[0]
    height = res[1]

    return width, height


def codec(file_path):
    """Use ``ffprobe`` to inspect codec of video file

    @param file_path
    @return width, height
    @throw OSError if `file_path` does not exist, or if
    ``ffprobe`` cannot be run (e.g. not installed)
    @throw ValueError if codec cannot be inferred from
    ``ffprobe`` output
    """
    ffprobe_output = check_output(['ffprobe',
                                   '-v', 'error',
                                   '-of', 'flat=s=_',
                                   '-show_entries', 'stream=codec_name',
                                   file_path])
    codec_str = ffprobe_output.split('=')[1].strip().strip('"')
    if codec_str is not None:
        return codec_str
    else:
        raise ValueError('Codec cannot be inferred from ffprobe output')


def timing_report(file_path):
    """Check whether `file_path` is a valid YAML timing report file

    @param file_path
    @return ``True`` if valid, ``False`` otherwise
    @throw IOError if `file_path` cannot be opened for reading
    @throw YAMLError if `file_path` cannot be parsed as a YAML file
    """
    with open(file_path, 'r') as stream:
        data = yaml.load(stream)
        if data['elapsed'] is None:
            return False
        try:
            strptime(data['elapsed'], '%H:%M:%S.%f')
        except ValueError:
            return False
        if data['latency'] is None:
            return False
        try:
            strptime(data['latency'], '%H:%M:%S.%f')
        except ValueError:
            return False
    return True


def test_parse(colour_space):
    # not existing config file
    with pytest.raises(IOError):
        _ = parse('/this/file/should/never/exist.yml')

    # non-parseable file
    with pytest.raises(yaml.YAMLError):
        _ = parse('config/yamlerror.yml')

    # files with invalid values
    with pytest.raises(ValueError):
        _ = parse('config/valueerror1.yml')
    if colour_space == BGR24:
        with pytest.raises(ValueError):
            _ = parse('config/valueerror2.yml')
    elif colour_space == I420:
        with pytest.raises(ValueError):
            _ = parse('config/valueerror3.yml')
    else:
        pytest.fail('Colour space not configured properly')

    # folder that can't be created
    with pytest.raises(OSError):
        _ = parse('config/oserror.yml')


def test_frame_grabbing(colour_space):
    # test-input & data
    if colour_space == BGR24:
        fs_config_file = 'config/sdi.yml'
        us_config_file = 'config/dvi.yml'
        fs_frame_rate = 28.0
        us_frame_rate = 14.0
    elif colour_space == I420:
        fs_config_file = 'config/sdi-i420.yml'
        us_config_file = 'config/dvi-i420.yml'
        fs_frame_rate = 40.0
        us_frame_rate = 28.0
    else:
        pytest.fail('Colour space not configured properly')

    # create actual threads
    fs = parse(fs_config_file)
    us = parse(us_config_file)
    assert fs is not None
    recorders.append(fs)
    assert us is not None
    recorders.append(us)
    assert fs.frame_rate == fs_frame_rate and \
        fs.port == pygiftgrab.Device.DVI2PCIeDuo_SDI and \
        fs.timeout_limit == 20.0 and \
        fs.colour_space == colour_space
    assert us.frame_rate == us_frame_rate and \
        us.port == pygiftgrab.Device.DVI2PCIeDuo_DVI and \
        us.timeout_limit == 15.0 and \
        us.colour_space == colour_space

    # test actual output now
    roi = [426, 40, 1064, 1040]
    width_offset = 0
    if colour_space == I420:
        width_offset = 4  # see GiftGrab issues #54 and #67
    fs.start()
    us.start()
    fs.set_sub_frame(roi[0], roi[1], roi[2], roi[3])  # should have no effect
    recording_duration = 5
    sleep(recording_duration+2)
    fs.pause_recording()
    us.pause_recording()
    sleep(4)

    # has output directory been created?
    assert isdir(dirname(fs.file_path))
    assert isdir(dirname(us.file_path))

    # have videos been recorded?
    fs_file_path = fs.file_path + '-000001.mp4'
    assert timing_report(fs_file_path + '.timing.yml')
    assert isfile(fs_file_path)
    us_file_path = us.file_path + '-000001.mp4'
    assert timing_report(us_file_path + '.timing.yml')
    assert isfile(us_file_path)
    assert frame_rate(fs_file_path) == fs.frame_rate
    assert frame_rate(us_file_path) == us.frame_rate
    assert duration(fs_file_path) >= recording_duration
    assert duration(us_file_path) >= recording_duration
    assert resolution(fs_file_path) == (1920, 1080)
    assert resolution(us_file_path) == (1920, 1080)
    assert codec(fs_file_path) == 'hevc'
    assert codec(us_file_path) == 'hevc'

    # another recording
    with pytest.raises(ValueError):
        fs.set_sub_frame(roi[0], -roi[1], roi[2], roi[3])
    fs.set_sub_frame(roi[0], roi[1], roi[2], roi[3])
    us.set_sub_frame(roi[0], roi[1], roi[2], roi[3])
    recording_duration = 10
    fs.resume_recording()
    us.resume_recording()
    sleep(recording_duration+2)
    fs.pause_recording()
    us.pause_recording()
    sleep(4)
    fs_file_path = fs.file_path + '-000002.mp4'
    assert timing_report(fs_file_path + '.timing.yml')
    assert isfile(fs_file_path)
    us_file_path = us.file_path + '-000002.mp4'
    assert isfile(us_file_path)
    assert timing_report(us_file_path + '.timing.yml')
    assert frame_rate(fs_file_path) == fs.frame_rate
    assert frame_rate(us_file_path) == us.frame_rate
    assert duration(fs_file_path) >= recording_duration
    assert duration(us_file_path) >= recording_duration
    assert resolution(fs_file_path) == (roi[2] - width_offset, roi[3])
    assert resolution(us_file_path) == (roi[2], roi[3])
    assert codec(fs_file_path) == 'hevc'
    assert codec(us_file_path) == 'hevc'

    # yet another recording
    recording_duration = 25
    fs.set_full_frame()
    fs.resume_recording()
    us.resume_recording()
    us.set_full_frame()  # should have no effect
    sleep(recording_duration+2)
    fs.pause_recording()
    us.pause_recording()
    sleep(4)
    fs_file_path = fs.file_path + '-000003.mp4'
    assert timing_report(fs_file_path + '.timing.yml')
    assert isfile(fs_file_path)
    us_file_path = us.file_path + '-000003.mp4'
    assert timing_report(us_file_path + '.timing.yml')
    assert isfile(us_file_path)
    assert frame_rate(fs_file_path) == fs.frame_rate
    assert frame_rate(us_file_path) == us.frame_rate
    assert duration(fs_file_path) >= recording_duration
    assert duration(us_file_path) >= recording_duration
    assert resolution(fs_file_path) == (1920, 1080)
    assert resolution(us_file_path) == (roi[2], roi[3])
    assert codec(fs_file_path) == 'hevc'
    assert codec(us_file_path) == 'hevc'

    # finish session
    fs.stop()
    us.stop()

    # join threads
    fs.join(timeout=fs.timeout_limit)
    assert not fs.isAlive()
    us.join(timeout=us.timeout_limit)
    assert not us.isAlive()

    # test dumped Recorder configuration
    dump(fs)
    dumped_fs = parse(join(dirname(fs_file_path), 'config.yml'))
    assert dirname(dirname(dumped_fs.file_path)) == dirname(fs.file_path)
    assert dumped_fs.frame_rate == fs.frame_rate
    assert dumped_fs.port == fs.port
    assert dumped_fs.timeout_limit == fs.timeout_limit
    dump(us)
    dumped_us = parse(join(dirname(us_file_path), 'config.yml'))
    assert dirname(dirname(dumped_us.file_path)) == dirname(us.file_path)
    assert dumped_us.frame_rate == us.frame_rate
    assert dumped_us.port == us.port
    assert dumped_us.timeout_limit == us.timeout_limit


@pytest.yield_fixture(autouse=True)
def cleanup():
    # This section runs before each test

    # Run test
    yield
    # This section runs after each test
    for recorder in recorders:
        recorder.stop()
    sleep(5)

    for recorder in recorders:
        recorder.join(timeout=recorder.timeout_limit)
    sleep(5)

    for recorder in recorders:
        rmtree(dirname(recorder.file_path))

    for recorder in recorders:
        assert not recorder.isAlive()
