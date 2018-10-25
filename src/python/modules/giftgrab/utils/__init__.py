#!/usr/bin/env python
import pygiftgrab as gg
from time import strftime


def __unique_file_path():
    """Generate a timestamped, unique file name.

    @return
    """
    return strftime('%Y-%m-%d-%H-%M-%S')


def __extension(codec):
    """Infer file extension from passed `codec`.

    @param codec one of the codecs supported
    in pygiftgrab
    """
    if codec == gg.Codec.Xvid:
        return '.avi'
    elif codec == gg.Codec.HEVC:
        return '.mp4'
    elif codec == gg.Codec.VP9:
        return '.webm'
    else:
        raise ValueError('Codec unknown')
