from pytest import yield_fixture, fail, mark
from pygiftgrab import VideoSourceFactory, VideoFrame, ColourSpace, Device

source = None
frame = None
frame_with_colour_mismatch = None
width = 0
height = 0
sub_x = 0
sub_y = 0
sub_width = 0
sub_height = 0
sub2_x = 0
sub2_y = 0
sub2_width = 0
sub2_height = 0


@yield_fixture(scope='session')
def peri_test(colour_space):
    # This section runs before each test
    factory = VideoSourceFactory.get_instance()
    global source
    try:
        source = factory.get_device(Device.DeckLinkSDI4K, colour_space)
    except IOError as e:
        raise RuntimeError(
            'Could not connect to Blackmagic DeckLink SDI 4K,\n' +
            'The detailed error was:\n' +
            e.message
        )
    assert source is not None

    global frame, frame_with_colour_mismatch
    frame = VideoFrame(colour_space, False)
    if colour_space == ColourSpace.BGRA:
        mismatch_colour_space = ColourSpace.UYVY
    else:
        mismatch_colour_space = ColourSpace.BGRA
    frame_with_colour_mismatch = VideoFrame(mismatch_colour_space, False)

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

    global sub2_x, sub2_y, sub2_width, sub2_height
    sub2_width = width // 5
    sub2_height = height // 5
    sub2_x = width // 6
    sub2_y = height // 6
    # health checks
    assert sub2_x + sub2_width < width
    assert sub2_y + sub2_height < height

    # Run test
    yield


@mark.unit
@mark.usefixtures('peri_test')
def test_get_frame():
    assert source.get_frame(frame)
    assert frame.rows() > 0
    assert frame.cols() > 0

    assert not source.get_frame(frame_with_colour_mismatch)
