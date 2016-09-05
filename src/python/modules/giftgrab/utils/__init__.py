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

    @param codec one of the storage formats defined
    in pygiftgrab
    """
    if codec == gg.Storage.File_XviD:
        return '.avi'
    elif codec == gg.Storage.File_H265:
        return '.mp4'
    elif codec == gg.Storage.File_VP9:
        return '.webm'
    else:
        raise ValueError('Codec unknown')


def __record_epiphan_dvi2pcieduo(port,
                                 frame,
                                 num_frames,
                                 codec):
    """Record specified nr. of frames from specified Epiphan
    DVI2PCIeDuo port to specified file using specified codec.

    Note that the frame-rate of the file is set to 30, but
    this value may not reflect reality, due to the encoding
    time involved.

    @param port as defined in pygiftgrab
    @param frame a BGRA or I420 frame to use (i.e. determines
    the colour space)
    @param num_frames
    @param file_path does not include the file extension,
    which is automatically inferred using `codec`
    @param codec as defined in pygiftgrab
    """
    epiphan = gg.Factory.connect(port)
    writer = gg.Factory.writer(codec)
    frame_rate = 30
    writer.init(__unique_file_path() + __extension(codec), frame_rate)
    for _ in range(num_frames):
        epiphan.get_frame(frame)
        writer.append(frame)
    writer.finalise()
    gg.Factory.disconnect(port)


num_frames = 30


def record_epiphan_dvi2pcieduo_sdi_bgr24_xvid():
    port = gg.Device.DVI2PCIeDuo_SDI
    frame = gg.VideoFrame_BGRA(True)
    codec = gg.Storage.File_XviD
    __record_epiphan_dvi2pcieduo(port,
                                 frame,
                                 num_frames,
                                 codec)


def record_epiphan_dvi2pcieduo_sdi_bgr24_h265():
    port = gg.Device.DVI2PCIeDuo_SDI
    frame = gg.VideoFrame_BGRA(True)
    codec = gg.Storage.File_H265
    __record_epiphan_dvi2pcieduo(port,
                                 frame,
                                 num_frames,
                                 codec)


def record_epiphan_dvi2pcieduo_sdi_bgr24_vp9():
    port = gg.Device.DVI2PCIeDuo_SDI
    frame = gg.VideoFrame_BGRA(True)
    codec = gg.Storage.File_VP9
    __record_epiphan_dvi2pcieduo(port,
                                 frame,
                                 num_frames,
                                 codec)


def record_epiphan_dvi2pcieduo_sdi_i420_xvid():
    port = gg.Device.DVI2PCIeDuo_SDI
    frame = gg.VideoFrame_I420(True)
    codec = gg.Storage.File_XviD
    __record_epiphan_dvi2pcieduo(port,
                                 frame,
                                 num_frames,
                                 codec)


def record_epiphan_dvi2pcieduo_sdi_i420_h265():
    port = gg.Device.DVI2PCIeDuo_SDI
    frame = gg.VideoFrame_I420(True)
    codec = gg.Storage.File_H265
    __record_epiphan_dvi2pcieduo(port,
                                 frame,
                                 num_frames,
                                 codec)


def record_epiphan_dvi2pcieduo_sdi_i420_vp9():
    port = gg.Device.DVI2PCIeDuo_SDI
    frame = gg.VideoFrame_I420(True)
    codec = gg.Storage.File_VP9
    __record_epiphan_dvi2pcieduo(port,
                                 frame,
                                 num_frames,
                                 codec)


def record_epiphan_dvi2pcieduo_dvi_bgr24_xvid():
    port = gg.Device.DVI2PCIeDuo_DVI
    frame = gg.VideoFrame_BGRA(True)
    codec = gg.Storage.File_XviD
    __record_epiphan_dvi2pcieduo(port,
                                 frame,
                                 num_frames,
                                 codec)


def record_epiphan_dvi2pcieduo_dvi_bgr24_h265():
    port = gg.Device.DVI2PCIeDuo_DVI
    frame = gg.VideoFrame_BGRA(True)
    codec = gg.Storage.File_H265
    __record_epiphan_dvi2pcieduo(port,
                                 frame,
                                 num_frames,
                                 codec)


def record_epiphan_dvi2pcieduo_dvi_bgr24_vp9():
    port = gg.Device.DVI2PCIeDuo_DVI
    frame = gg.VideoFrame_BGRA(True)
    codec = gg.Storage.File_VP9
    __record_epiphan_dvi2pcieduo(port,
                                 frame,
                                 num_frames,
                                 codec)


def record_epiphan_dvi2pcieduo_dvi_i420_xvid():
    port = gg.Device.DVI2PCIeDuo_DVI
    frame = gg.VideoFrame_I420(True)
    codec = gg.Storage.File_XviD
    __record_epiphan_dvi2pcieduo(port,
                                 frame,
                                 num_frames,
                                 codec)


def record_epiphan_dvi2pcieduo_dvi_i420_h265():
    port = gg.Device.DVI2PCIeDuo_DVI
    frame = gg.VideoFrame_I420(True)
    codec = gg.Storage.File_H265
    __record_epiphan_dvi2pcieduo(port,
                                 frame,
                                 num_frames,
                                 codec)


def record_epiphan_dvi2pcieduo_dvi_i420_vp9():
    port = gg.Device.DVI2PCIeDuo_DVI
    frame = gg.VideoFrame_I420(True)
    codec = gg.Storage.File_VP9
    __record_epiphan_dvi2pcieduo(port,
                                 frame,
                                 num_frames,
                                 codec)

