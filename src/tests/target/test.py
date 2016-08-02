from pytest import fail, yield_fixture
from subprocess import check_call
from os import devnull, remove, listdir
from pygiftgrab import Storage, Factory, VideoFrame_BGRA
import inspection


# for easily removing created files when done
tmp_file_prefix = 'tmp_GiftGrab_test_'
target = None
frame = None


def __file_ext(codec):
    if codec == Storage.File_H265:
        return 'mp4'
    elif codec == Storage.File_XviD:
        return 'avi'


def __storage2str(codec):
    if codec == Storage.File_H265:
        return 'H265'
    elif codec == Storage.File_XviD:
        return 'Xvid'


@yield_fixture(autouse=True)
def peri_test(codec):
    # This section runs before each test

    global target, frame
    frame = VideoFrame_BGRA(640, 400)
    target = Factory.writer(codec)
    if target is None:
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
    frame_rate = 60
    file_name = '%sframe_rate_%f.%s'\
                % (tmp_file_prefix, frame_rate, __file_ext(codec))
    target.init(file_name, frame_rate)
    for i in range(10):
        target.append(frame)
    target.finalise()
    assert inspection.frame_rate(file_name) == frame_rate


def test_resolution(codec):
    rows = 1080
    cols = 1920
    frame1920x1080 = VideoFrame_BGRA(rows, cols)
    file_name = '%sresolution_%f.%s'\
                % (tmp_file_prefix, 60, __file_ext(codec))
    target.init(file_name, 60)
    for i in range(10):
        target.append(frame1920x1080)
    target.finalise()
    assert inspection.resolution(file_name) == (cols, rows)


def test_num_frames(codec):
    # TODO
    fail(msg='not implemented')


def test_can_reuse_target(codec):
    # TODO
    fail(msg='not implemented')


def test_filetype_checked(codec):
    # TODO
    fail(msg='not implemented')
