#!/usr/bin/env python

from epiphan import parse, dump
from time import sleep


if __name__ == '__main__':
    # create sessions
    fs_config = 'sdi.yml'
    fs = parse(fs_config)

    us_config = 'dvi.yml'
    us = parse(us_config)

    # do actual work
    fs.start()
    us.start()
    sleep(5)
    fs.pause_recording()
    us.pause_recording()
    sleep(4)
    fs.set_sub_frame(426, 40, 1067, 1040)
    fs.resume_recording()
    us.resume_recording()
    sleep(5)
    fs.pause_recording()
    us.pause_recording()
    sleep(4)
    fs.set_full_frame()
    fs.resume_recording()
    us.resume_recording()
    sleep(5)

    # finish session
    fs.stop()
    us.stop()
    fs.join()
    us.join()

    # dump configuration
    dump(fs)
    dump(us)
