#!/usr/bin/python

import pygiftgrab
import time
from datetime import timedelta

frame_rate = 60
recording_duration = 1  # min
num_frames = int(recording_duration * 60 * frame_rate)
device_type = pygiftgrab.Device.DVI2PCIeDuo_SDI
storage_type = pygiftgrab.Storage.File_H265
if storage_type == pygiftgrab.Storage.File_H265:
    extension = '.mp4'
elif storage_type == pygiftgrab.Storage.File_XviD:
    extension = '.avi'
file_path = str(recording_duration) + '-min-python-recording-from-' + str(device_type) + extension

frame = pygiftgrab.VideoFrame_BGRA(False)  # to avoid "thin wrappers" required for default args
try:
    source = pygiftgrab.Factory.connect(device_type)
    # source.set_sub_frame(660, 160, 678, 688)
    target = pygiftgrab.Factory.writer(storage_type)
    target.init(file_path, frame_rate)
    start = time.time()
    for i in range(1, num_frames+1):
        source.get_frame(frame)
        if i % 30 == 0:
            elapsed = (time.time() - start)
            print 'Frame ' + str(i) + ', ' + \
                  str(timedelta(seconds=elapsed)) + \
                  ', ' + str(num_frames)
        target.append(frame)
    elapsed = (time.time() - start)
    print 'TOTAL ' + \
          str(num_frames) + ' @ ' + \
          str(frame.cols()) + ' x ' + str(frame.rows()) + \
          ' (Epiphan SDI), ' + str(timedelta(seconds=elapsed))
    target.finalise()
    pygiftgrab.Factory.disconnect(pygiftgrab.Device.DVI2PCIeDuo_SDI)
except (RuntimeError, IOError) as e:
    print e.message
