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
file_path_sdi = str(recording_duration) + '-min-python-recording-from-' + str(device_type) + extension

frame_sdi = pygiftgrab.VideoFrame_BGRA(False)  # to avoid "thin wrappers" required for default args
try:
    source_sdi = pygiftgrab.Factory.connect(device_type)
    # source.set_sub_frame(660, 160, 678, 688)
    target_sdi = pygiftgrab.Factory.writer(storage_type)
    target_sdi.init(file_path_sdi, frame_rate)
    start = time.time()
    for i in range(1, num_frames+1):
        source_sdi.get_frame(frame_sdi)
        if i % 30 == 0:
            elapsed = (time.time() - start)
            print 'Frame ' + str(i) + ', ' + \
                  str(timedelta(seconds=elapsed)) + \
                  ', ' + str(num_frames)
        target_sdi.append(frame_sdi)
    elapsed = (time.time() - start)
    print 'TOTAL ' + \
          str(num_frames) + ' @ ' + \
          str(frame_sdi.cols()) + ' x ' + str(frame_sdi.rows()) + \
          ' (Epiphan SDI), ' + str(timedelta(seconds=elapsed))
    target_sdi.finalise()
    pygiftgrab.Factory.disconnect(pygiftgrab.Device.DVI2PCIeDuo_SDI)
except (RuntimeError, IOError) as e:
    print e.message
