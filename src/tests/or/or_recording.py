#!/usr/bin/env python

from epiphan import EpiphanRecorder
from time import sleep, strftime
from os.path import exists
from os import makedirs
from random import choice
from string import ascii_uppercase
from epiphan_config import parse_config, write_config
import pygiftgrab


def session_folder():
    attempts = 0
    while attempts < 5:
        attempts += 1
        unique_suffix = ''.join(choice(ascii_uppercase) for _ in range(5))
        folder_name = strftime('%Y-%m-%d-%H-%M-%S') + '-' +\
            unique_suffix + '/'

        if not exists(folder_name):
            makedirs(folder_name)
            return True, folder_name
        else:
            print 'Folder ' + folder_name + \
                  ' exists, will make a ' + \
                  str(attempts) + '. attempt'
    print 'Made ' + str(attempts) + \
          ' unsuccessful attempts to create ' + \
          'a uniquely-named folder'
    return False, folder_name

if __name__ == '__main__':
    ret, folder = session_folder()
    if not ret:
        exit(1)

    fs_config_file_name = 'sdi.yml'
    fs_port, fs_frame_rate,\
        fs_file_path, fs_timeout_limit = parse_config(fs_config_file_name)
    fs = EpiphanRecorder(port=fs_port,
                         frame_rate=fs_frame_rate,
                         file_path=folder+fs_file_path,
                         timeout_limit=fs_timeout_limit)
    us_config_file_name = 'dvi.yml'
    us_port, us_frame_rate,\
        us_file_path, us_timeout_limit = parse_config(us_config_file_name)
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
    write_config(fs, folder + fs_config_file_name)
    write_config(us, folder + us_config_file_name)
