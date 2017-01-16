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
def test_access(colour_space):
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
