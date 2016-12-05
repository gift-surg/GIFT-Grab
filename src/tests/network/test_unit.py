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
    # TODO
    assert False


@mark.unit
@mark.usefixtures('peri_test')
def test_full_frame():
    # TODO
    assert False
