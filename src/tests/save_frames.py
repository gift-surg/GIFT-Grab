#!/usr/bin/python

import pygiftgrab

file_path = 'python-test-file.mp4'
frame_rate = 20
recording_duration = 1  # min
num_frames = recording_duration * 60 * frame_rate

print pygiftgrab.Device.DVI2PCIeDuo_SDI
print pygiftgrab.Device.DVI2PCIeDuo_DVI

try:
    source_device = pygiftgrab.VideoSourceOpenCV(0)
    print source_device.get_frame_rate()
except (RuntimeError, IOError) as e:
    print e.message

try:
    source_file = pygiftgrab.VideoSourceOpenCV("/home/dzhoshkun/data/mosaic/imageUndistorted_000001.mp4")
    print source_file.get_frame_rate()
except (RuntimeError, IOError) as e:
    print e.message

frame = pygiftgrab.VideoFrame_BGRA(False)  # to avoid "thin wrappers" required for default args
try:
    source = pygiftgrab.Factory.connect(pygiftgrab.Device.DVI2PCIeDuo_SDI)
    # TODO
    # target = pygiftgrab.gg_Factory.writer(pygiftgrab.gg_Storage.File_H265)
    # target.init(file_path, frame_rate)
    for i in range(1, num_frames+1):
        source.get_frame(frame)
        print 'Frame ' + str(i) + '/' + str(num_frames) + \
              ' is ' + str(frame.cols()) + ' x ' + str(frame.rows())
    #     target.append(frame)
    #     sleep(inter_frame_msec)
    # target.finalise()
    pygiftgrab.Factory.disconnect(pygiftgrab.Device.DVI2PCIeDuo_SDI)
except (RuntimeError, IOError) as e:
    print e.message
