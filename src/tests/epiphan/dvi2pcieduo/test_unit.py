from pytest import yield_fixture, fail, mark
from pygiftgrab import Factory, VideoFrame

source = None
frame = None
width = 0
height = 0
sub_x = 0
sub_y = 0
sub_width = 0
sub_height = 0


@yield_fixture(scope='session')
def peri_test(port, colour_space):
    # This section runs before each test
    global source
    try:
        source = Factory.connect(port, colour_space)
    except IOError as e:
        raise RuntimeError('Could not connect to Epiphan DVI2PCIe Duo,\n' +
                           'The detailed error was:\n' +
                           e.message)
    assert source is not None

    global frame
    frame = VideoFrame(False, colour_space)

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
    # TODO: commented out to allow
    # for test GiftGrab#71 resolved
    # assert source is None


@mark.unit
@mark.usefixtures('peri_test')
def test_get_frame():
    assert source.get_frame(frame)
    assert frame.rows() > 0
    assert frame.cols() > 0


@mark.unit
@mark.usefixtures('peri_test')
def test_sub_frame():
    source.set_sub_frame(sub_x, sub_y,
                         sub_width, sub_height)
    assert source.get_frame(frame)
    assert frame.cols() == sub_width
    assert frame.rows() == sub_height


@mark.unit
@mark.usefixtures('peri_test')
def test_full_frame():
    source.set_sub_frame(sub_x, sub_y,
                         sub_width, sub_height)
    source.get_full_frame()
    assert source.get_frame(frame)
    assert frame.cols() == width
    assert frame.rows() == height
