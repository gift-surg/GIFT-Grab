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


def __record_epiphan_dvi2pcieduo(port,
                                 colour_space,
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
    source_factory = gg.VideoSourceFactory.get_instance()
    epiphan = source_factory.get_device(port, colour_space)
    target_factory = gg.VideoTargetFactory.get_instance()
    frame_rate = 30
    filename = __unique_file_path() + __extension(codec)
    writer = target_factory.create_file_writer(codec,
                                               filename,
                                               frame_rate)
    for _ in range(num_frames):
        epiphan.get_frame(frame)
        writer.append(frame)


num_frames = 30


def record_epiphan_dvi2pcieduo_sdi_bgra_xvid():
    port = gg.Device.DVI2PCIeDuo_SDI
    colour_space = gg.ColourSpace.BGRA
    frame = gg.VideoFrame(colour_space, True)
    codec = gg.Codec.Xvid
    __record_epiphan_dvi2pcieduo(port,
                                 colour_space,
                                 frame,
                                 num_frames,
                                 codec)


def record_epiphan_dvi2pcieduo_sdi_bgra_hevc():
    port = gg.Device.DVI2PCIeDuo_SDI
    colour_space = gg.ColourSpace.BGRA
    frame = gg.VideoFrame(colour_space, True)
    codec = gg.Codec.HEVC
    __record_epiphan_dvi2pcieduo(port,
                                 colour_space,
                                 frame,
                                 num_frames,
                                 codec)


def record_epiphan_dvi2pcieduo_sdi_bgra_vp9():
    port = gg.Device.DVI2PCIeDuo_SDI
    colour_space = gg.ColourSpace.BGRA
    frame = gg.VideoFrame(colour_space, True)
    codec = gg.Codec.VP9
    __record_epiphan_dvi2pcieduo(port,
                                 colour_space,
                                 frame,
                                 num_frames,
                                 codec)


def record_epiphan_dvi2pcieduo_sdi_i420_xvid():
    port = gg.Device.DVI2PCIeDuo_SDI
    colour_space = gg.ColourSpace.I420
    frame = gg.VideoFrame(colour_space, True)
    codec = gg.Codec.Xvid
    __record_epiphan_dvi2pcieduo(port,
                                 colour_space,
                                 frame,
                                 num_frames,
                                 codec)


def record_epiphan_dvi2pcieduo_sdi_i420_hevc():
    port = gg.Device.DVI2PCIeDuo_SDI
    colour_space = gg.ColourSpace.I420
    frame = gg.VideoFrame(colour_space, True)
    codec = gg.Codec.HEVC
    __record_epiphan_dvi2pcieduo(port,
                                 colour_space,
                                 frame,
                                 num_frames,
                                 codec)


def record_epiphan_dvi2pcieduo_sdi_i420_vp9():
    port = gg.Device.DVI2PCIeDuo_SDI
    colour_space = gg.ColourSpace.I420
    frame = gg.VideoFrame(colour_space, True)
    codec = gg.Codec.VP9
    __record_epiphan_dvi2pcieduo(port,
                                 colour_space,
                                 frame,
                                 num_frames,
                                 codec)


def record_epiphan_dvi2pcieduo_dvi_bgra_xvid():
    port = gg.Device.DVI2PCIeDuo_DVI
    colour_space = gg.ColourSpace.BGRA
    frame = gg.VideoFrame(colour_space, True)
    codec = gg.Codec.Xvid
    __record_epiphan_dvi2pcieduo(port,
                                 colour_space,
                                 frame,
                                 num_frames,
                                 codec)


def record_epiphan_dvi2pcieduo_dvi_bgra_hevc():
    port = gg.Device.DVI2PCIeDuo_DVI
    colour_space = gg.ColourSpace.BGRA
    frame = gg.VideoFrame(colour_space, True)
    codec = gg.Codec.HEVC
    __record_epiphan_dvi2pcieduo(port,
                                 colour_space,
                                 frame,
                                 num_frames,
                                 codec)


def record_epiphan_dvi2pcieduo_dvi_bgra_vp9():
    port = gg.Device.DVI2PCIeDuo_DVI
    colour_space = gg.ColourSpace.BGRA
    frame = gg.VideoFrame(colour_space, True)
    codec = gg.Codec.VP9
    __record_epiphan_dvi2pcieduo(port,
                                 colour_space,
                                 frame,
                                 num_frames,
                                 codec)


def record_epiphan_dvi2pcieduo_dvi_i420_xvid():
    port = gg.Device.DVI2PCIeDuo_DVI
    colour_space = gg.ColourSpace.I420
    frame = gg.VideoFrame(colour_space, True)
    codec = gg.Codec.Xvid
    __record_epiphan_dvi2pcieduo(port,
                                 colour_space,
                                 frame,
                                 num_frames,
                                 codec)


def record_epiphan_dvi2pcieduo_dvi_i420_hevc():
    port = gg.Device.DVI2PCIeDuo_DVI
    colour_space = gg.ColourSpace.I420
    frame = gg.VideoFrame(colour_space, True)
    codec = gg.Codec.HEVC
    __record_epiphan_dvi2pcieduo(port,
                                 colour_space,
                                 frame,
                                 num_frames,
                                 codec)


def record_epiphan_dvi2pcieduo_dvi_i420_vp9():
    port = gg.Device.DVI2PCIeDuo_DVI
    colour_space = gg.ColourSpace.I420
    frame = gg.VideoFrame(colour_space, True)
    codec = gg.Codec.VP9
    __record_epiphan_dvi2pcieduo(port,
                                 colour_space,
                                 frame,
                                 num_frames,
                                 codec)

