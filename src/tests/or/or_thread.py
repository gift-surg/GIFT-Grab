#!/usr/bin/env python

from threading import Thread
from time import sleep


class ORThread(Thread):
    def __init__(self):
        print 'init'
        self.is_running = True
        Thread.__init__(self)

    def run(self):
        while self.is_running:
            print 'run'
            sleep(0.5)
            # TODO

    def stop(self):
        print 'stop'
        self.is_running = False
