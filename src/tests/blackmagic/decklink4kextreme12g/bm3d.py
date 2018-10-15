#!/usr/bin/env python2

import time
import os
import cv2
import numpy as np
from pygiftgrab import IObservableObserver
from pygiftgrab import VideoSourceFactory
from pygiftgrab import ColourSpace
from pygiftgrab import Device
from pygiftgrab import VideoFrame


class FrameSaver(IObservableObserver):

    def __init__(self, num_to_save):
        super(FrameSaver, self).__init__()
        self.num_saved = 0
        self.num_to_save = num_to_save
        self.frames_np_data = []
        self.frame_dims = None

    def update(self, frame):
        if self.num_saved < self.num_to_save:
            if self.frame_dims is None:
                self.frame_dims = [frame.rows(), frame.cols()]
            np_data = np.copy(frame.data(False))
            self.frames_np_data.append(np_data)
            self.num_saved += 1

    def dump(self):
        print('Acquired {} frames'.format(self.num_saved))
        if self.num_saved < self.num_to_save:
            print('Still acquiring')
            return
        out_folder = os.path.join('.', time.strftime('%Y-%m-%d-%H-%M-%S'))
        os.mkdir(out_folder)
        data_bgra = np.zeros(self.frame_dims + [4], np.uint8)
        for i, np_data in enumerate(self.frames_np_data):
            left, right = np_data[:np_data.size / 2], np_data[np_data.size / 2:]
            for j, current in enumerate([left, right]):
                current = np.reshape(current, self.frame_dims + [2])
                out_file = os.path.join(out_folder,
                                        'frame-{:03d}-{}.png'.format(i+1, j))

                cv2.cvtColor(src=current, code=cv2.COLOR_YUV2BGRA_UYVY, dst=data_bgra)
                cv2.imwrite(out_file, data_bgra)
        print('Saved {} frames in {}'.format(len(self.frames_np_data), out_folder))


if __name__ == '__main__':
    sfac = VideoSourceFactory.get_instance()
    bm = sfac.get_device(Device.DeckLinkSDI4K, ColourSpace.UYVY)
    frame = VideoFrame(ColourSpace.UYVY, False)

    saver = FrameSaver(10)

    bm.attach(saver)

    time.sleep(2)  # operate pipeline for 20 sec

    bm.detach(saver)

    saver.dump()
