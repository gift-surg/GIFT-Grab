#!/usr/bin/env python

from or_thread import ORThread
from time import sleep
from datetime import timedelta
import pygiftgrab

fetoscope = ORThread(port=pygiftgrab.Device.DVI2PCIeDuo_SDI, frame_rate=27, file_path='test-fetoscope')
us = ORThread(port=pygiftgrab.Device.DVI2PCIeDuo_DVI, frame_rate=15, file_path='test-us')
fetoscope.start()
us.start()
sleep(5)
fetoscope.pause_recording()
us.pause_recording()
sleep(4)
fetoscope.resume_recording()
us.resume_recording()
sleep(5)
fetoscope.pause_recording()
us.pause_recording()
sleep(4)
fetoscope.resume_recording()
us.resume_recording()
sleep(5)
fs_latency = fetoscope.stop()
us_latency = us.stop()
fetoscope.join()
us.join()

print 'Total latencies were ' + \
    str(timedelta(seconds=fs_latency)) + ' (FS) and ' + \
      str(timedelta(seconds=us_latency)) + ' (US) ' + \
    'respectively.'
