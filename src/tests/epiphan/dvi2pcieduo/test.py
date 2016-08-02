from pytest import fail, yield_fixture
from pygiftgrab import Factory

source = None


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


def test_get_frame(port):
    # TODO
    fail(msg='not implemented')


def test_sub_frame(port):
    # TODO
    fail(msg='not implemented')


def test_full_frame(port):
    # TODO
    fail(msg='not implemented')


def test_frame_dimensions(port):
    # TODO
    fail(msg='not implemented')
