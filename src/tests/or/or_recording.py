#!/usr/bin/env python

from or_thread import ORThread
from time import sleep

fetoscope_thread = ORThread(frame_rate=27, file_path='test-dvi')
fetoscope_thread.start()
sleep(5)
fetoscope_thread.pause_recording()
sleep(4)
fetoscope_thread.resume_recording()
sleep(5)
fetoscope_thread.pause_recording()
sleep(4)
fetoscope_thread.resume_recording()
sleep(5)
fetoscope_thread.stop()
fetoscope_thread.join()
