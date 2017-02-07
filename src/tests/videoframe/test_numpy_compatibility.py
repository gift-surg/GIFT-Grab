from pytest import mark, yield_fixture, fail, raises
from pygiftgrab import VideoFrame, ColourSpace
import numpy as np


frame = None
cols = 1920
rows = 1080


@yield_fixture(autouse=True)
def peri_test(colour_space):
    global frame
    frame = VideoFrame(colour_space, cols, rows)
    assert frame is not None

    yield


@mark.numpy_compatibility
def test_data():
    global frame, cols, rows
    data_np = frame.data()
    assert data_np.dtype == np.uint8
    assert frame.data_length() == data_np.size


@mark.numpy_compatibility
def test_data_length():
    global frame, cols, rows
    data_np = frame.data()
    data_len = frame.data_length()

    with raises(IndexError) as e:
        data_np[data_len]

    try:
        data_np[data_len - 1]
    except IndexError as e:
        fail(e.message)


@mark.numpy_compatibility
def test_read_access(colour_space):
    global frame
    global cols, rows
    data_np = frame.data()

    # general indices sampled at the boundaries only
    # intermediate indices should be accessible,
    # provided that these are accessible
    indices = [col * rows + row
               for col in [0, cols - 1]
               for row in [0, rows - 1]]

    # currently performing no value checks
    for index in indices:
        try:
            if colour_space == ColourSpace.BGRA:
                data_np[index * 4 + 0]  # Blue channel
                data_np[index * 4 + 1]  # Green channel
                data_np[index * 4 + 2]  # Red channel
                data_np[index * 4 + 3]  # Alpha channel

            elif colour_space == ColourSpace.I420:
                # see https://wiki.videolan.org/YUV/#I420
                data_np[index]         # Y plane
                data_np[(int)( index * 1.25 )]  # U plane
                data_np[(int)( index * 1.5 )]   # V plane

            elif colour_space == ColourSpace.UYVY:
                # see http://www.fourcc.org/pixel-format/yuv-uyvy/
                data_np[index * 2]
                data_np[index * 2 + 1]

        except IndexError as e:
            fail(e.message)


@mark.numpy_compatibility
def test_write_access():
    global frame
    global cols, rows
    data_np = frame.data()
    data_np_ = frame.data()  # shadow pointer
    data_len = frame.data_length()
    for i in [0, data_len // 4, data_len // 2, 3 * data_len // 4]:
        rolled_value = roll_uint8(data_np[i])
        assert rolled_value != data_np[i]
        data_np[i] = rolled_value
        assert data_np_[i] == rolled_value


def roll_uint8(value):
    """Roll given one-byte value, i.e. increment it such that
    the range is preserved.
    >>> roll_uint8(-10)
    ValueError: Acceptable range: 0 - 255
    >>> roll_uint8(-1)
    ValueError: Acceptable range: 0 - 255
    >>> roll_uint8(0)
    1
    >>> roll_uint8(10)
    11
    >>> roll_uint8(254)
    255
    >>> roll_uint8(255)
    0
    >>> roll_uint8(256)
    ValueError: Acceptable range: 0 - 255
    """
    new_value = value + 1
    if 0 < new_value < 256:
        return new_value
    elif new_value == 256:
        return 0
    else:
        raise ValueError('Acceptable range: 0 - 255')
