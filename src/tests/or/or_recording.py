#!/usr/bin/env python

from or_thread import ORThread
from time import sleep
import pygiftgrab

fs = ORThread(port=pygiftgrab.Device.DVI2PCIeDuo_SDI, frame_rate=27, file_path='test-fetoscope')
us = ORThread(port=pygiftgrab.Device.DVI2PCIeDuo_DVI, frame_rate=15, file_path='test-us')
fs.start()
us.start()
sleep(5)
fs.pause_recording()
us.pause_recording()
sleep(4)
fs.resume_recording()
us.resume_recording()
sleep(5)
fs.pause_recording()
us.pause_recording()
sleep(4)
fs.resume_recording()
us.resume_recording()
sleep(5)
fs.stop()
us.stop()
fs.join()
us.join()
