#!/usr/bin/env python

from threading import Thread
from time import sleep


class ORThread(Thread):
    def __init__(self):
        print 'init'
        self.is_running = True
        self.is_recording = False
        Thread.__init__(self)

    def run(self):
        self.is_recording = True
        while self.is_running:
            print 'run'
            sleep(0.5)
            # TODO
            if self.is_recording:
                print 'record'

    def stop(self):
        print 'stop'
        self.is_running = False

    def pause_recording(self):
        self.is_recording = False

    def resume_recording(self):
        self.is_recording = True
