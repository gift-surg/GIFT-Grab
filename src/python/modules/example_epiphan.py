#!/usr/bin/env python

from epiphan import parse, dump
from time import sleep
import yaml
import logging


if __name__ == '__main__':
    try:
        # create sessions
        fs_config = 'example_epiphan/sdi.yml'
        us_config = 'example_epiphan/dvi.yml'
        logging.basicConfig(format='%(threadName)s -- %(asctime)s -- %(message)s',
                            datefmt='%m/%d/%Y %H:%M:%S')

        try:
            fs = parse(fs_config)
            us = parse(us_config)
        except (yaml.YAMLError, IOError, ValueError, OSError) as e:
            logging.error(
                'Parsing configuration files failed with: ' +
                e.message
            )
        else:
            # do actual work
            fs.start()
            us.start()
            sleep(5)
            fs.pause_recording()
            us.pause_recording()
            sleep(4)
            try:
                fs.set_sub_frame(426, -40, 1067, 1040)
            except ValueError as e:
                logging.error(e.message)
            try:
                us.set_sub_frame(426, 40, 1067, 1040)
            except ValueError as e:
                logging.error(e.message)
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
                logging.error('Fetoscope thread could not be stopped')
            us.join(timeout=us.timeout_limit)
            if us.isAlive():
                logging.error('Ultrasound thread could not be stopped')

            # dump configuration
            try:
                dump(fs)
                dump(us)
            except (yaml.YAMLError, IOError) as e:
                logging.error(
                    'Dumping configuration failed with: ' +
                    e.message
                )
    except BaseException as e:
        logging.error('Unspecific exception: ' + e.message)