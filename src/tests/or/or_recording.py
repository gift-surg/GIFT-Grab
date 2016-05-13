#!/usr/bin/env python

from epiphan import EpiphanRecorder
from time import sleep, strftime
from os.path import exists
from os import makedirs
from random import choice
from string import ascii_uppercase
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

    fs = EpiphanRecorder(port=pygiftgrab.Device.DVI2PCIeDuo_SDI,
                         frame_rate=27, file_path=folder+'test-fetoscope')
    us = EpiphanRecorder(port=pygiftgrab.Device.DVI2PCIeDuo_DVI,
                         frame_rate=15, file_path=folder+'test-us')
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
