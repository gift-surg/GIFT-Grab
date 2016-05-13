#!/usr/bin/env python

from epiphan import EpiphanRecorder
from time import sleep, strftime
from os.path import exists, join, split, dirname
from os import makedirs
from random import choice
from string import ascii_uppercase
from epiphan_config import parse_config, write_config
import pygiftgrab


def session_folder(path_prefix):
    """Attempt to create a session folder with a unique name.

    @param path_prefix where to create session folder, this
    must be a path pointing to an existing folder, and some
    file name prefix, i.e. no trailing slash (`/`)
    @return ``True`` and updated `path_prefix` upon success,
    ``False`` and session folder path attempted to create
    otherwise
    """
    path_prefix_folder, path_prefix_filename = split(path_prefix)
    attempted_folder = None
    attempts = 0
    while attempts < 5:
        attempts += 1
        unique_suffix = ''.join(choice(ascii_uppercase) for _ in range(5))
        attempted_folder = join(path_prefix_folder,
                                strftime('%Y-%m-%d-%H-%M-%S') +
                                '-' + unique_suffix)

        if not exists(attempted_folder):
            makedirs(attempted_folder)
            return True, join(attempted_folder, path_prefix_filename)
        else:
            print 'Folder ' + attempted_folder + \
                  ' exists, will make a ' + \
                  str(attempts) + '. attempt'
    print 'Made ' + str(attempts) + \
          ' unsuccessful attempts to create ' + \
          'a uniquely-named folder'
    return False, attempted_folder

if __name__ == '__main__':
    fs_config_file_name = 'sdi.yml'
    fs_port, fs_frame_rate,\
        fs_file_path, fs_timeout_limit = parse_config(fs_config_file_name)
    ret, file_path = session_folder(fs_file_path)
    fs_config_file_name_out = join(dirname(file_path), fs_config_file_name)
    if not ret:
        exit(1)
    fs = EpiphanRecorder(port=fs_port,
                         frame_rate=fs_frame_rate,
                         file_path=file_path,
                         timeout_limit=fs_timeout_limit)

    us_config_file_name = 'dvi.yml'
    us_port, us_frame_rate,\
        us_file_path, us_timeout_limit = parse_config(us_config_file_name)
    ret, file_path = session_folder(us_file_path)
    us_config_file_name_out = join(dirname(file_path), us_config_file_name)
    if not ret:
        exit(1)
    us = EpiphanRecorder(port=us_port,
                         frame_rate=us_frame_rate,
                         file_path=file_path,
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
    write_config(fs, fs_config_file_name_out)
    write_config(us, us_config_file_name_out)
