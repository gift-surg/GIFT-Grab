#!/usr/bin/env python

from threading import Thread
from time import sleep, time


class ORThread(Thread):
    def __init__(self, frame_rate, file_path):
        print 'init'
        # TODO - filename health checks
        self.file_path = file_path
        self.recording_index = 0
        # TODO - frame_rate health checks
        self.frame_rate = frame_rate
        self.is_running = True
        self.is_recording = False
        Thread.__init__(self)

    def run(self):
        self.is_recording = True
        inter_frame_duration = 1.0 / self.frame_rate # sec
        print 'filename: ' + self.__next_filename()
        print 'inter-frame duration: ' + str(inter_frame_duration) + ' sec'
        while self.is_running:
            current_processing_start = time()
            # print 'run'
            sleep(0.02)
            # TODO
            if self.is_recording:
                # TODO
                2 + 2
            elapsed = time() - current_processing_start
            # print 'elapsed ' + str(elapsed) + ' sec'
            sleep(inter_frame_duration - elapsed)

    def stop(self):
        print 'stop'
        self.is_running = False

    def pause_recording(self):
        # TODO finalise current file
        self.is_recording = False

    def resume_recording(self):
        print 'new filename: ' + self.__next_filename()
        self.is_recording = True

    def __next_filename(self):
        self.recording_index += 1
        return self.file_path + '-' + '{0:06d}'.format(self.recording_index) + '.mp4'
