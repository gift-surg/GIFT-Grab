from pytest import fail, yield_fixture, raises
from subprocess import check_call
from os import devnull, remove, listdir
from pygiftgrab import Storage, Factory, VideoFrame_BGRA
from giftgrab.utils import inspection


# for easily removing created files when done
tmp_file_prefix = 'tmp_GiftGrab_test_'
target = None
frame = None
frame_rate = 60


def __file_ext(codec):
    if codec == Storage.File_HEVC:
        return 'mp4'
    elif codec == Storage.File_XviD:
        return 'avi'
    elif codec == Storage.File_VP9:
        return 'webm'


def __storage2str(codec):
    if codec == Storage.File_HEVC:
        return 'HEVC'
    elif codec == Storage.File_XviD:
        return 'Xvid'
    elif codec == Storage.File_VP9:
        return 'VP9'


@yield_fixture(autouse=True)
def peri_test(codec):
    # This section runs before each test

    global target, frame
    frame = VideoFrame_BGRA(400, 640)
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
    _frame_rate = 40
    file_name = '%sframe_rate.%s'\
                % (tmp_file_prefix, __file_ext(codec))
    target.init(file_name, _frame_rate)
    for i in range(10):
        target.append(frame)
    target.finalise()
    assert inspection.frame_rate(file_name) == _frame_rate


def test_resolution(codec):
    rows = 1080
    cols = 1920
    frame1920x1080 = VideoFrame_BGRA(rows, cols)
    file_name = '%sresolution.%s'\
                % (tmp_file_prefix, __file_ext(codec))
    target.init(file_name, frame_rate)
    for i in range(10):
        target.append(frame1920x1080)
    target.finalise()
    assert inspection.resolution(file_name) == (cols, rows)


def test_num_frames(codec):
    file_name = '%snum_frames.%s'\
                % (tmp_file_prefix, __file_ext(codec))
    target.init(file_name, frame_rate)
    num_frames = 3 * frame_rate
    for i in range(num_frames):
        target.append(frame)
    target.finalise()
    assert inspection.duration(file_name) * frame_rate == num_frames


def test_can_reuse_target(codec):
    for j in range(3):
        file_name = '%scan_reuse_target_%d.%s'\
                    % (tmp_file_prefix, j, __file_ext(codec))
        target.init(file_name, frame_rate)
        for i in range(frame_rate):
            target.append(frame)
        target.finalise()


def test_filetype_checked(codec):
    file_ext = 'rrr'
    assert not file_ext == __file_ext(codec)
    file_name = '%sfiletype_checked.%s'\
                % (tmp_file_prefix, file_ext)
    with raises(RuntimeError):
        target.init(file_name, frame_rate)
