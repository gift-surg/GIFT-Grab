#!/usr/bin/env python

from or_thread import ORThread

fetoscope_thread = ORThread()
fetoscope_thread.start()
fetoscope_thread.join()
