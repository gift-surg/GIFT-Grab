#!/usr/bin/env python

from or_thread import ORThread
from time import sleep

fetoscope_thread = ORThread()
fetoscope_thread.start()
sleep(5)
fetoscope_thread.stop()
fetoscope_thread.join()
