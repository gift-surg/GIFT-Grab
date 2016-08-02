from pytest import fail, yield_fixture
from subprocess import check_call
from os import devnull, remove, listdir
from pygiftgrab import Storage, Factory


# for easily removing created files when done
tmp_file_prefix = 'tmp_GiftGrab_test_'


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

    try:
        file_null = open(devnull, 'w')
        check_call(['ffprobe', '--help'],
                   stdout=file_null, stderr=file_null)
    except BaseException:
        raise RuntimeError(
            'FFmpeg does not appear to be installed on your system,\n'
            'GiftGrab tests need FFmpeg.')

    # Run test
    yield

    # This section runs after each test

    for f in listdir('.'):
        if str(f).startswith(tmp_file_prefix):
            remove(f)


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
