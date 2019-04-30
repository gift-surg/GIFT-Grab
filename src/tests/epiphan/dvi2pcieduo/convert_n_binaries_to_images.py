#!/usr/bin/env python3

import numpy as np
from imageio import imwrite


"""
Helper script accompanying the save_n_frames_to_binary.cpp app.
It simply reads in the binary output of that app and saves each
data file as a PNG image.
"""


def four_uint8_to_uint32(four_bytes):
    """Convert 4 unsigned bytes to an unsigned integer."""
    return np.frombuffer(four_bytes, dtype='<u4')[0]


if __name__ == '__main__':
    for i in range(3):
        data_read = np.fromfile(f'snapshot-{i}.bin', dtype=np.uint8)
        width = four_uint8_to_uint32(data_read[:4])
        height = four_uint8_to_uint32(data_read[4:8])
        data_np = data_read[8:].reshape((height, width, 4))
        # BGRA to RGBA
        blue = np.copy(data_np[:, :, 0])
        data_np[:, :, 0] = data_np[:, :, 2]
        data_np[:, :, 2] = blue
        imwrite(f'snapshot-{i}.png', data_np)
