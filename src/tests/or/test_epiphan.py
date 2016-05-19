#!/usr/bin/env python

from epiphan import parse
from pytest import raises
import yaml


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
