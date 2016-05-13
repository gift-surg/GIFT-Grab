#!/usr/bin/env python

from epiphan import EpiphanRecorder
from time import sleep, strftime
from os.path import exists
from os import makedirs
from random import choice
from string import ascii_uppercase
from epiphan_config import parse_config
import pygiftgrab


def session_folder():
    attempts = 0
    while attempts < 5:
        attempts += 1
        unique_suffix = ''.join(choice(ascii_uppercase) for _ in range(5))
        folder = strftime('%Y-%m-%d-%H-%M-%S') + '-' +\
                 unique_suffix + '/'

        if not exists(folder):
            makedirs(folder)
            return True, folder
        else:
            print 'Folder ' + folder + \
                  ' exists, will make a ' + \
                  str(attempts) + '. attempt'
    print 'Made ' + str(attempts) + \
          ' unsuccessful attempts to create ' + \
          'a uniquely-named folder'
    return False, folder

if __name__ == '__main__':
    ret, folder = session_folder()
    if not ret:
        exit(1)

    fs_port, fs_frame_rate,\
        fs_file_path, fs_timeout_limit = parse_config('sdi.yml')
    fs = EpiphanRecorder(port=fs_port,
                         frame_rate=fs_frame_rate,
                         file_path=folder+fs_file_path,
                         timeout_limit=fs_timeout_limit)
    us_port, us_frame_rate,\
        us_file_path, us_timeout_limit = parse_config('dvi.yml')
    us = EpiphanRecorder(port=us_port,
                         frame_rate=us_frame_rate,
                         file_path=folder+us_file_path,
                         timeout_limit=us_timeout_limit)
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
    fs.stop()
    us.stop()
    fs.join()
    us.join()
