#!/usr/bin/env python

from threading import Thread
from time import sleep, time
from datetime import timedelta
import pygiftgrab


class ORThread(Thread):
    def __init__(self, port, frame_rate, file_path):
        self.port = port
        # TODO - exception
        self.file = pygiftgrab.Factory.writer(pygiftgrab.Storage.File_H265)
        self.file_path = file_path
        self.recording_index = 0
        # TODO - frame_rate health checks
        self.frame_rate = frame_rate
        self.is_running = True
        self.is_recording = False
        self.latency = 0.0  # sec
        Thread.__init__(self)

    def run(self):
        # mitigate re-start risk
        if not self.is_running:
            return

        inter_frame_duration = self.__inter_frame_duration()

        # TODO - exception
        device = pygiftgrab.Factory.connect(self.port)
        frame = pygiftgrab.VideoFrame_BGRA(False)
        self.resume_recording()  # i.e. start recording

        while self.is_running:
            start = time()
            if self.is_recording:
                device.get_frame(frame)
                self.file.append(frame)
            sleep_duration = inter_frame_duration - (time() - start)
            if sleep_duration > 0:
                sleep(sleep_duration)
            else:
                self.latency -= sleep_duration

    def stop(self):
        self.pause_recording()
        self.is_running = False
        pygiftgrab.Factory.disconnect(self.port)

    def pause_recording(self):
        # TODO - exception
        self.is_recording = False
        # sleep to allow for stop to be picked up
        sleep(2 * self.__inter_frame_duration())
        self.file.finalise()
        # write latency as well
        latency_file = open(self.__next_filename(increment_index=False) +
                            '.latency.txt', 'w')
        latency_file.write(str(timedelta(seconds=self.latency)) + '\n')
        self.latency = 0.00

    def resume_recording(self):
        filename = self.__next_filename()
        self.file.init(filename, self.frame_rate)
        self.is_recording = True

    def __next_filename(self, increment_index=True):
        if increment_index:
            self.recording_index += 1
        return self.file_path + '-' + \
            '{0:06d}'.format(self.recording_index) + \
            '.mp4'

    def __inter_frame_duration(self):
        return 1.0 / self.frame_rate  # sec
