#!/usr/bin/env python

from time import sleep, strptime
import pytest
import yaml
from subprocess import check_output
from os.path import isdir, dirname, isfile, join
from os import environ
from shutil import rmtree
from giftgrab.epiphan import parse, dump
import pygiftgrab
from giftgrab.utils.inspection import frame_rate, duration, resolution, codec


recorders = []


def timing_report(file_path):
    """Check whether `file_path` is a valid YAML timing report file

    @param file_path
    @return ``True`` if valid, ``False`` otherwise
    @throw IOError if `file_path` cannot be opened for reading
    @throw YAMLError if `file_path` cannot be parsed as a YAML file
    """
    with open(file_path, 'r') as stream:
        data = yaml.load(stream)
        if 'elapsed' not in data:
            return False
        try:
            strptime(data['elapsed'], '%H:%M:%S.%f')
        except ValueError:
            return False
    return True


@pytest.mark.epiphan_py_module
@pytest.mark.usefixtures('cleanup')
def test_parse(colour_space, config_dir):
    # not existing config file
    with pytest.raises(IOError):
        _ = parse(join('this', 'file', 'should', 'never', 'exist.yml'))

    # non-parseable file
    with pytest.raises(yaml.YAMLError):
        _ = parse(join(config_dir, 'yamlerror.yml'))

    # files with invalid values
    with pytest.raises(ValueError):
        _ = parse(join(config_dir, 'valueerror1.yml'))
    if colour_space == pygiftgrab.ColourSpace.BGRA:
        with pytest.raises(ValueError):
            _ = parse(join(config_dir, 'valueerror2.yml'))
    elif colour_space == pygiftgrab.ColourSpace.I420:
        with pytest.raises(ValueError):
            _ = parse(join(config_dir, 'valueerror3.yml'))
    else:
        pytest.fail('Colour space not configured properly')

    # folder that can't be created
    with pytest.raises(OSError):
        _ = parse(join(config_dir, 'oserror.yml'))


@pytest.mark.epiphan_py_module
@pytest.mark.usefixtures('cleanup')
def test_frame_grabbing(colour_space, config_dir):
    # test-input & data
    if colour_space == pygiftgrab.ColourSpace.BGRA:
        fs_config_file = join(config_dir, 'sdi.yml')
        us_config_file = join(config_dir, 'dvi.yml')
        fs_frame_rate = 28.0
        us_frame_rate = 14.0
    elif colour_space == pygiftgrab.ColourSpace.I420:
        fs_config_file = join(config_dir, 'sdi-i420.yml')
        us_config_file = join(config_dir, 'dvi-i420.yml')
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
    # disabling the offset within #83
    if colour_space == pygiftgrab.ColourSpace.I420 and environ.get('USE_EPIPHANSDK'):
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
    assert resolution(us_file_path) == (roi[2] - width_offset, roi[3])
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
    assert resolution(us_file_path) == (roi[2] - width_offset, roi[3])
    assert codec(fs_file_path) == 'hevc'
    assert codec(us_file_path) == 'hevc'

    # finish session
    fs.stop()
    us.stop()

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


@pytest.yield_fixture
def cleanup():
    # This section runs before each test

    # Run test
    yield
    # This section runs after each test
    for recorder in recorders:
        recorder.stop()
    sleep(5)

    for recorder in recorders:
        rmtree(dirname(recorder.file_path))

    del recorders[:]
