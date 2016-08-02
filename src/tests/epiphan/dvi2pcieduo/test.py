from pytest import fail, yield_fixture
from pygiftgrab import Factory, VideoFrame_BGRA

source = None
frame = None
width = 0
height = 0
sub_x = 0
sub_y = 0
sub_width = 0
sub_height = 0

@yield_fixture(autouse=True)
def peri_test(port):
    # This section runs before each test
    global source
    try:
        source = Factory.connect(port)
    except IOError as e:
        raise RuntimeError('Could not connect to Epiphan DVI2PCIe Duo,\n' +
                           'The detailed error was:\n' +
                           e.message)
    assert source is not None

    global frame
    frame = VideoFrame_BGRA(False)

    global height, width
    assert source.get_frame(frame)
    width = frame.cols()
    height = frame.rows()

    global sub_x, sub_y, sub_width, sub_height
    sub_width = width // 3
    sub_height = height // 3
    sub_x = width // 2
    sub_y = height // 2
    # health checks
    assert sub_x + sub_width < width
    assert sub_y + sub_height < height

    # Run test
    yield

    # This section runs after each test
    try:
        Factory.disconnect(port)
    except IOError as e:
        raise RuntimeError('Could not disconnect from Epiphan DVI2PCIe Duo,\n' +
                           'The detailed error was:\n' +
                           e.message)
    assert source is None


def test_get_frame():
    assert source.get_frame(frame)
    assert frame.rows() > 0
    assert frame.cols() > 0


def test_sub_frame(port):
    source.set_sub_frame(sub_x, sub_y,
                         sub_width, sub_height)
    assert source.get_frame(frame)
    assert frame.cols() == sub_width
    assert frame.rows() == sub_height


def test_full_frame(port):
    source.set_sub_frame(sub_x, sub_y,
                         sub_width, sub_height)
    source.get_full_frame()
    assert source.get_frame(frame)
    assert frame.cols() == width
    assert frame.rows() == height

