#!/usr/bin/env python
import pygiftgrab as gg

def record_epiphan_dvi2pcieduo():
    num_frames = 30  # TODO
    port = gg.Device.DVI2PCIeDuo_SDI  # TODO
    codec = gg.Storage.File_VP9  # TODO
    epiphan = gg.Factory.connect(port)
    frame = gg.VideoFrame_I420(True)  # TODO
    file_path = './record_epiphan_dvi2pcieduo.webm'  # TODO

    writer = gg.Factory.writer(codec)
    writer.init(file_path, 30)  # TODO
    for _ in range(num_frames):
        epiphan.get_frame(frame)
        writer.append(frame)
    writer.finalise()
    gg.Factory.disconnect(port)

    print('Recorded {} frames to {}  '.format(num_frames, file_path))
