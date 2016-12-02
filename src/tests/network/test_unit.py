from pytest import yield_fixture, fail, mark
from pygiftgrab import VideoSourceFactory, VideoFrame, ColourSpace


@yield_fixture(scope='session')
def peri_test(address, colour_space):
    # TODO
    yield


@mark.unit
@mark.usefixtures('peri_test')
def test_get_frame():
    # TODO
    assert False


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
