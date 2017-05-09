from pytest import yield_fixture, raises
from subprocess import check_call
from os import devnull, remove, listdir
from string import ascii_uppercase
from random import choice
from time import strftime
from math import ceil
from pygiftgrab import Codec, VideoTargetFactory, VideoFrame, ColourSpace
from giftgrab.utils import inspection


# for easily removing created files when done
tmp_file_prefix = 'tmp_GiftGrab_test'
factory = None
target = None
frame = None
frame_with_colour_mismatch = None
file_name = None
frame_rate = 59.94


def __file_ext(codec):
    if codec == Codec.HEVC:
        return 'mp4'
    elif codec == Codec.Xvid:
        return 'avi'
    elif codec == Codec.VP9:
        return 'webm'


def __codec2str(codec):
    if codec == Codec.HEVC:
        return 'HEVC'
    elif codec == Codec.Xvid:
        return 'Xvid'
    elif codec == Codec.VP9:
        return 'VP9'


@yield_fixture(autouse=True)
def peri_test(codec, colour_space):
    # This section runs before each test

    global frame
    frame = VideoFrame(colour_space, 400, 640)

    global frame_with_colour_mismatch
    if colour_space == ColourSpace.BGRA:
        colour_space_with_mismatch = ColourSpace.I420
    else:
        colour_space_with_mismatch = ColourSpace.BGRA
    frame_with_colour_mismatch = VideoFrame(colour_space_with_mismatch,
                                            400, 640)

    global factory
    factory = VideoTargetFactory.get_instance()
    assert factory is not None

    global file_name
    file_name = '{}_{}-{}.{}'.format(
        tmp_file_prefix,
        strftime('%Y-%m-%d-%H-%M-%S'),
        ''.join(choice(ascii_uppercase) for _ in range(5)),
        __file_ext(codec)
    )

    global target, frame_rate
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

    global tmp_file_prefix
    for f in listdir('.'):
        if str(f).startswith(tmp_file_prefix):
            remove(f)


def test_append_with_colour_mismatch(codec):
    if codec != Codec.Xvid:
        return

    with raises(RuntimeError):
        target.append(frame_with_colour_mismatch)


def test_frame_rate():
    global target, file_name, frame_rate
    for i in range(10):
        target.append(frame)
    del target
    _frame_rate = inspection.frame_rate(file_name)
    # TODO: remove after issue #193
    if file_name[ -4: ] == '.avi':
        assert abs(_frame_rate - frame_rate) < 0.001
    else:
        assert _frame_rate == frame_rate


def test_resolution(colour_space):
    rows = 1080
    cols = 1920
    frame1920x1080 = VideoFrame(colour_space, cols, rows)
    global target, file_name
    for i in range(10):
        target.append(frame1920x1080)
    del target
    assert inspection.resolution(file_name) == (cols, rows)


def test_num_frames():
    global target, file_name, frame, frame_rate
    exp_duration = 3  # sec
    num_frames = exp_duration * int( ceil(frame_rate) )
    for i in range(num_frames):
        target.append(frame)
    del target
    assert inspection.num_frames( file_name ) == num_frames


def test_filetype_checked(codec):
    invalid_file_ext = 'rrr'
    assert not invalid_file_ext == __file_ext(codec)
    global factory, file_name
    target = None
    with raises(RuntimeError):
        target = factory.create_file_writer(
            codec,
            '{}.{}'.format(file_name, invalid_file_ext),
            frame_rate
        )
    assert target is None
