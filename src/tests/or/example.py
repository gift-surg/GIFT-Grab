#!/usr/bin/env python

from epiphan import parse, dump
from time import sleep
import yaml


if __name__ == '__main__':
    # create sessions
    fs_config = 'config/sdi.yml'
    us_config = 'config/dvi.yml'

    try:
        fs = parse(fs_config)
        us = parse(us_config)
    except (yaml.YAMLError, IOError, ValueError) as e:
        print 'Parsing configuration files failed with: ' + \
              e.message
    else:
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

        # join threads
        fs.join(timeout=fs.timeout_limit)
        if fs.isAlive():
            print 'Fetoscope thread could not be stopped'
        us.join(timeout=us.timeout_limit)
        if us.isAlive():
            print 'Ultrasound thread could not be stopped'

        # dump configuration
        try:
            dump(fs)
            dump(us)
        except (yaml.YAMLError, IOError) as e:
            print 'Dumping configuration failed with: ' + \
                  e.message
