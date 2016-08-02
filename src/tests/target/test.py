from pytest import fail, yield_fixture
from pygiftgrab import Storage, Factory


def __storage2str(codec):
    if codec == Storage.File_H265:
        return 'H265'
    elif codec == Storage.File_XviD:
        return 'Xvid'


@yield_fixture(autouse=True)
def peri_test(codec):
    # This section runs before each test

    writer = Factory.writer(codec)
    if writer is None:
        raise RuntimeError('No ' + __storage2str(codec) + ' writer returned')
    else:
        # Run test
        yield

    # This section runs after each test


def test_frame_rate(codec):
    # TODO
    fail(msg='not implemented')


def test_resolution(codec):
    # TODO
    fail(msg='not implemented')


def test_num_frames(codec):
    # TODO
    fail(msg='not implemented')


def test_can_reuse_target(codec):
    # TODO
    fail(msg='not implemented')


def test_filetype_checked(codec):
    # TODO
    fail(msg='not implemented')
