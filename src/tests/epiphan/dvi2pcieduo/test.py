from pytest import fail, yield_fixture
from pygiftgrab import Factory, VideoFrame_BGRA

source = None
frame = None


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
    assert source.get_frame(frame)
    rows = frame.rows()
    cols = frame.cols()

    _width = cols // 3
    _height = rows // 3
    _x = cols // 2
    _y = rows // 2
    # health checks
    assert _x + _width < cols
    assert _y + _height < rows
    source.set_sub_frame(_x, _y, _width, _height)

    assert source.get_frame(frame)
    assert frame.cols() == _width
    assert frame.rows() == _height


def test_full_frame(port):
    # TODO
    fail(msg='not implemented')


def test_frame_dimensions(port):
    # TODO
    fail(msg='not implemented')
