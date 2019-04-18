#!/usr/bin/env python3

import numpy as np
import scipy.misc


"""
Helper script accompanying the save_n_frames_to_binary.cpp app.
It simply reads in the binary output of that app and saves each
data file as a PNG image.
"""

if __name__ == '__main__':
    for i in range(3):
        data_read = np.fromfile(f'snapshot-{i}.bin', dtype=np.uint8)
        width = np.fromstring(data_read[:4], dtype='>u4')
        height = np.fromstring(data_read[4:8], dtype='>u4')
        data_np = data_read[8:].reshape((data_read[1], data_read[0], 4))
        scipy.misc.imsave(f'snapshot-{i}.png', data_np)
