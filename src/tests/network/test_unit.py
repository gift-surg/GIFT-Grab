from pytest import yield_fixture, mark
from pygiftgrab import VideoSourceFactory, VideoFrame, ColourSpace


source = None
frame = None
frame_with_colour_mismatch = None


@yield_fixture(scope='session')
def peri_test(address, colour_space):
    factory = VideoSourceFactory.get_instance()
    global source
    source = factory.connect_network_source(address, colour_space)
    assert source is not None

    global frame, frame_with_colour_mismatch
    frame = VideoFrame(colour_space, False)
    if colour_space == ColourSpace.BGRA:
        mismatch_colour_space = ColourSpace.I420
    else:
        mismatch_colour_space = ColourSpace.BGRA
    frame_with_colour_mismatch = VideoFrame(mismatch_colour_space, False)

    yield


@mark.unit
@mark.usefixtures('peri_test')
def test_get_frame():
    global source, frame
    assert source.get_frame(frame)
    assert frame.rows() > 0
    assert frame.cols() > 0

    assert not source.get_frame(frame_with_colour_mismatch)


@mark.unit
@mark.usefixtures('peri_test')
def test_sub_frame():
    global source, frame
    source.get_frame(frame)
    width = frame.cols()
    height = frame.rows()

    # a first sub-frame
    sub_width = width // 3
    sub_height = height // 3
    sub_x = width // 2
    sub_y = height // 2
    # health checks
    assert sub_x + sub_width < width
    assert sub_y + sub_height < height

    source.set_sub_frame(sub_x, sub_y,
                         sub_width, sub_height)
    assert source.get_frame(frame)
    assert frame.cols() == sub_width
    assert frame.rows() == sub_height

    # a second sub-frame
    sub2_width = width // 5
    sub2_height = height // 5
    sub2_x = width // 6
    sub2_y = height // 6
    # health checks
    assert sub2_x + sub2_width < width
    assert sub2_y + sub2_height < height

    source.set_sub_frame(sub2_x, sub2_y,
                         sub2_width, sub2_height)
    assert source.get_frame(frame)
    assert frame.cols() == sub2_width
    assert frame.rows() == sub2_height


@mark.unit
@mark.usefixtures('peri_test')
def test_full_frame():
    # TODO
    assert False
