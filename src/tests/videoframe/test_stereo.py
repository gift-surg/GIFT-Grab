from pytest import (mark, yield_fixture, raises)
from pygiftgrab import (VideoFrame, ColourSpace)


stereo_frame, stereo_count = None, None
data_length = 0


@yield_fixture(autouse=True)
def peri_test(colour_space):
    global stereo_frame, stereo_count, data_length
    stereo_count = 2
    cols, rows = 1920, 1080
    stereo_frame = VideoFrame(colour_space, cols, rows, stereo_count)
    data_length = VideoFrame.required_data_length(colour_space)


@mark.stereo_frames
def test_default_index_is_0():
    assert data_length == stereo_frame.data_length()
    assert stereo_frame.data_length() == stereo_frame.data_length(0)


@mark.stereo_frames
def test_valid_index_returns_data_length():
    for stereo_index in range(stereo_count):
        assert stereo_frame.data_length(stereo_index) == data_length


@mark.stereo_frames
def test_invalid_index_raises():
    for stereo_index in range(stereo_count, 2 * stereo_count):
        with raises(IndexError):
            stereo_frame.data_length(stereo_index)


@mark.stereo_frames
def test_stereo_frame_constructor(colour_space):
    cols, rows = 1920, 1080
    frame = VideoFrame(colour_space, cols, rows)
    assert frame.stereo_count() == 1
    for _stereo_count in range(2, 5):
        frame = VideoFrame(colour_space, cols, rows, _stereo_count)
        assert frame.stereo_count() == _stereo_count
