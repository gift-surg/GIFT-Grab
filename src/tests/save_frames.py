#!/usr/bin/python

# imports
import pygiftgrab
import time
from datetime import timedelta

# passed parameters
frame_rate = 60
recording_duration = 1  # min
num_frames = int(recording_duration * 60 * frame_rate)
storage_type = pygiftgrab.Storage.File_H265
lap = 30
sub_frame = False

# report header
print 'Recording video stream from Epiphan SDI + DVI ports to ' + str(pygiftgrab.Storage.File_H265)

# derived parameters
if storage_type == pygiftgrab.Storage.File_H265:
    extension = '.mp4'
elif storage_type == pygiftgrab.Storage.File_XviD:
    extension = '.avi'

file_path_sdi = str(recording_duration) + \
                '-min-python-recording-from-' + \
                str(pygiftgrab.Device.DVI2PCIeDuo_SDI) + \
                extension
file_path_dvi = str(recording_duration) + \
                '-min-python-recording-from-' + \
                str(pygiftgrab.Device.DVI2PCIeDuo_DVI) + \
                extension

frame_sdi = pygiftgrab.VideoFrame_BGRA(False)  # to avoid "thin wrappers" required for default args
frame_dvi = pygiftgrab.VideoFrame_BGRA(False)
try:
    # connect to both ports
    source_sdi = pygiftgrab.Factory.connect(pygiftgrab.Device.DVI2PCIeDuo_SDI)
    source_dvi = pygiftgrab.Factory.connect(pygiftgrab.Device.DVI2PCIeDuo_DVI)

    # set sub frames
    if sub_frame:
        x = 660
        y = 160
        width = 678
        height = 688
        source_sdi.set_sub_frame(x, y, width, height)
        source_dvi.set_sub_frame(x, y, width, height)

    # initialise file writers
    target_sdi = pygiftgrab.Factory.writer(storage_type)
    target_sdi.init(file_path_sdi, frame_rate)
    target_dvi = pygiftgrab.Factory.writer(storage_type)
    target_dvi.init(file_path_dvi, frame_rate)

    # actual work
    start = time.time()
    for i in range(1, num_frames+1):
        source_sdi.get_frame(frame_sdi)
        source_dvi.get_frame(frame_dvi)
        if i % lap == 0:
            elapsed = (time.time() - start)
            print 'Frame, ' + str(i) + ', ' + \
                  str(timedelta(seconds=elapsed)) + \
                  ', ' + str(num_frames)
        target_sdi.append(frame_sdi)
        target_dvi.append(frame_dvi)
    elapsed = (time.time() - start)

    # report results
    total_num_frames = 2*num_frames
    real_frame_rate = num_frames / elapsed
    print 'TOTAL Epiphan frames, total time, total expected time, ' + \
          'real framerate, expected framerate, SDI w x h, DVI w x h, ' + \
          str(total_num_frames) + ', ' + \
          str(timedelta(seconds=elapsed)) + ', ' + \
          str(timedelta(minutes=recording_duration)) + ', ' + \
          str(real_frame_rate) + ', ' + \
          str(frame_rate) + ', ' + \
          str(frame_sdi.cols()) + ', ' + str(frame_sdi.rows()) + ', ' + \
          str(frame_dvi.cols()) + ', ' + str(frame_dvi.rows())

    # finalise recording
    target_sdi.finalise()
    target_dvi.finalise()

    # disconnect from ports
    pygiftgrab.Factory.disconnect(pygiftgrab.Device.DVI2PCIeDuo_SDI)
    pygiftgrab.Factory.disconnect(pygiftgrab.Device.DVI2PCIeDuo_DVI)

except (RuntimeError, IOError) as e:
    print e.message
