#!/usr/bin/env python

import threading


class ORThread(threading.Thread):
    def __init__(self):
        print 'init'
        threading.Thread.__init__(self)

    def run(self):
        print 'run'
