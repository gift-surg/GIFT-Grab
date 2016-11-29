from pytest import mark, yield_fixture, fail
from pygiftgrab import VideoFrame


frame = None
cols = 1920
rows = 1080


@yield_fixture(autouse=True)
def peri_test(colour_space):
    global frame
    frame = VideoFrame(colour_space, cols, rows)
    assert frame is not None


@mark.numpy_compatibility
def test_data(colour_space):
    global frame, cols, rows
    data_numpy = frame.data()
    assert data_numpy.data_type == uchar
    assert data_numpy.size(0) == cols
    assert data_numpy.size(1) == rows


@mark.numpy_compatibility
def test_data_length(colour_space):
    global frame
    global cols, rows
    data_numpy = frame.data()
    assert frame.data_length() == data_numpy.total_size()
    try:
        data_numpy(0, 0)
        data_numpy(0, rows - 1)
        data_numpy(cols - 1, 0)
        data_numpy(cols - 1, rows - 1)
    except IndexError as e:
        fail(e.message)
