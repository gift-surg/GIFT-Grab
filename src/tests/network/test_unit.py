from pytest import yield_fixture, mark
from pygiftgrab import VideoSourceFactory, VideoFrame, ColourSpace


source = None
frame = None
frame_with_colour_mismatch = None
tmp_frame = None


def generate_sub_frame(x_factor, y_factor, width_factor, height_factor):
    # full frame dimensions
    width, height = get_frame_dimensions()

    # generate sub frame dimensions
    sub_width = width // width_factor
    sub_height = height // height_factor
    sub_x = width // x_factor
    sub_y = height // y_factor

    # do health check on sub frame dimensions
    assert sub_x + sub_width < width
    assert sub_y + sub_height < height

    return sub_x, sub_y, sub_width, sub_height


def get_frame_dimensions():
    global source
    assert source is not None
    global tmp_frame
    assert source.get_frame(tmp_frame)
    return tmp_frame.cols(), tmp_frame.rows()


@yield_fixture(scope='session')
def peri_test(address, colour_space):
    factory = VideoSourceFactory.get_instance()
    global source
    source = factory.connect_network_source(address, colour_space)
    assert source is not None

    global frame, frame_with_colour_mismatch, tmp_frame
    frame = VideoFrame(colour_space, False)
    tmp_frame = VideoFrame(colour_space, False)
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

    # a first sub-frame
    sub_x, sub_y, sub_width, sub_height = generate_sub_frame(2, 2, 3, 3)
    source.set_sub_frame(sub_x, sub_y, sub_width, sub_height)
    assert source.get_frame(frame)
    assert frame.cols() == sub_width
    assert frame.rows() == sub_height

    # a second sub-frame
    sub2_x, sub2_y, sub2_width, sub2_height = generate_sub_frame(6, 6, 5, 5)
    source.set_sub_frame(sub2_x, sub2_y, sub2_width, sub2_height)
    assert source.get_frame(frame)
    assert frame.cols() == sub2_width
    assert frame.rows() == sub2_height


@mark.unit
@mark.usefixtures('peri_test')
def test_full_frame():
    # TODO
    assert False
