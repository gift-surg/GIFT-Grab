#!/usr/bin/python

import pygiftgrab

file_path = 'python-test-file.mp4'
frame_rate = 20
recording_duration = 1  # min
num_frames = recording_duration * 60 * frame_rate

print pygiftgrab.foo()
print pygiftgrab.gg_Device.DVI2PCIeDuo_SDI
print pygiftgrab.gg_Device.DVI2PCIeDuo_DVI

source_device = pygiftgrab.VideoSourceOpenCV(0)
print source_device.get_frame_rate()

source_file = pygiftgrab.VideoSourceOpenCV("/home/dzhoshkun/data/mosaic/imageUndistorted_000001.mp4")
print source_file.get_frame_rate()