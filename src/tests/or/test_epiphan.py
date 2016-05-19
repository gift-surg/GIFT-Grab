#!/usr/bin/env python

from time import sleep
from pytest import raises
import yaml
from os.path import isdir, dirname
from epiphan import parse
import pygiftgrab


def test_epiphan_parse():
    # not existing config file
    with raises(IOError):
        _ = parse('/this/file/should/never/exist.yml')

    # non-parseable file
    with raises(yaml.YAMLError):
        _ = parse('config/yamlerror.yml')

    # file with invalid value
    with raises(ValueError):
        _ = parse('config/valueerror.yml')

    # folder that can't be created
    with raises(OSError):
        _ = parse('config/oserror.yml')


def test_frame_grabbing():
    # create actual threads
    fs = parse('config/sdi.yml')
    us = parse('config/dvi.yml')
    assert fs is not None
    assert us is not None
    assert fs.frame_rate == 28.0 and \
        fs.port == pygiftgrab.Device.DVI2PCIeDuo_SDI and \
        fs.timeout_limit == 20.0
    assert us.frame_rate == 14.0 and \
        us.port == pygiftgrab.Device.DVI2PCIeDuo_DVI and \
        us.timeout_limit == 15.0

    # test actual output now
    fs.start()
    us.start()
    sleep(5)
    fs.pause_recording()
    us.pause_recording()
    sleep(4)
    assert isdir(dirname(fs.file_path))
    assert isdir(dirname(us.file_path))

    # finish session
    fs.stop()
    us.stop()

    # join threads
    fs.join(timeout=fs.timeout_limit)
    assert not fs.isAlive()
    us.join(timeout=us.timeout_limit)
    assert not us.isAlive()
