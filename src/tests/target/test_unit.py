from pytest import fail, yield_fixture, raises
from subprocess import check_call
from os import devnull, remove, listdir
from string import ascii_uppercase
from random import choice
from time import strftime
from pygiftgrab import Storage, VideoTargetFactory, VideoFrame, ColourSpace
from giftgrab.utils import inspection


# for easily removing created files when done
tmp_file_prefix = 'tmp_GiftGrab_test_'
factory = None
target = None
frame = None
frame_with_colour_mismatch = None
file_name = None
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
def peri_test(codec, colour_space):
    # This section runs before each test

    global factory, tmp_file_prefix, file_name, target
    global frame, frame_with_colour_mismatch, frame_rate
    frame = VideoFrame(colour_space, 400, 640)
    if colour_space == ColourSpace.BGRA:
        colour_space_with_mismatch = ColourSpace.I420
    else:
        colour_space_with_mismatch = ColourSpace.BGRA
    frame_with_colour_mismatch = VideoFrame(colour_space_with_mismatch,
                                            400, 640)
    factory = VideoTargetFactory.get_instance()
    assert factory is not None

    file_name = '{}_{}-{}.{}'.format(
        tmp_file_prefix,
        strftime('%Y-%m-%d-%H-%M-%S'),
        ''.join(choice(ascii_uppercase) for _ in range(5)),
        __file_ext(codec)
    )
    target = None
    target = factory.create_file_writer(codec,
                                        file_name,
                                        frame_rate)
    assert target is not None

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

    # for f in listdir('.'):
    #     if str(f).startswith(tmp_file_prefix):
    #         remove(f)


def test_append_with_colour_mismatch(codec, colour_space):
    if codec != Storage.File_XviD:
        return

    with raises(RuntimeError):
        target.append(frame_with_colour_mismatch)
    target.finalise()


def test_frame_rate(codec):
    global target, file_name
    for i in range(10):
        target.append(frame)
    target.finalise()
    assert inspection.frame_rate(file_name) == frame_rate


def test_resolution(codec, colour_space):
    rows = 1080
    cols = 1920
    frame1920x1080 = VideoFrame(colour_space, cols, rows)
    global target, file_name
    for i in range(10):
        target.append(frame1920x1080)
    target.finalise()
    assert inspection.resolution(file_name) == (cols, rows)


def test_num_frames(codec):
    global target, file_name
    num_frames = 3 * frame_rate
    for i in range(num_frames):
        target.append(frame)
    target.finalise()
    assert inspection.duration(file_name) * frame_rate == num_frames


def test_filetype_checked(codec):
    file_ext = 'rrr'
    assert not file_ext == __file_ext(codec)
    file_name = '%sfiletype_checked.%s'\
                % (tmp_file_prefix, file_ext)
    target = None
    global factory
    with raises(RuntimeError):
        target = factory.create_file_writer(codec,
                                            file_name,
                                            frame_rate)
    assert target is None
