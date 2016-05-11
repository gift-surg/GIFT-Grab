#!/usr/bin/env python

from threading import Thread
from time import sleep, time


class ORThread(Thread):
    def __init__(self, frame_rate):
        print 'init'
        # TODO - frame_rate health checks
        self.frame_rate = frame_rate
        self.is_running = True
        self.is_recording = False
        Thread.__init__(self)

    def run(self):
        self.is_recording = True
        inter_frame_duration = 1.0 / self.frame_rate # sec
        print 'inter-frame duration: ' + str(inter_frame_duration) + ' sec'
        while self.is_running:
            current_processing_start = time()
            print 'run'
            sleep(0.002)
            # TODO
            if self.is_recording:
                print 'record'
            elapsed = time() - current_processing_start
            print 'elapsed ' + str(elapsed) + ' sec'
            sleep(inter_frame_duration - elapsed)

    def stop(self):
        print 'stop'
        self.is_running = False

    def pause_recording(self):
        self.is_recording = False

    def resume_recording(self):
        self.is_recording = True
