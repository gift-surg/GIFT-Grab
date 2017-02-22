from pytest import fail, mark, yield_fixture, raises
try:
    # in case of PyPI installation, this will work:
    from giftgrab.tests.utils import FileChecker
except ImportError:
    # in case of installation from source, this will work:
    from utils import FileChecker
from time import sleep
from pygiftgrab import VideoSourceFactory


factory = None
video_duration = 0  # inferred, in sec
quarter_video_duration = 0  # inferred, in sec


@yield_fixture(scope='session')
def peri_test(colour_space, filepath,
              frame_rate, frame_count,
              frame_width, frame_height):
    global factory
    factory = VideoSourceFactory.get_instance()
    global video_duration, quarter_video_duration
    video_duration = frame_count / frame_rate
    quarter_video_duration = video_duration / 4
    yield


@mark.observer_pattern
@mark.usefixtures('peri_test')
def test_valid_filepath_returns_raii_reader(
    filepath, colour_space,
    frame_rate, frame_count,
    frame_width, frame_height
):
    source = None
    global factory
    global video_duration, quarter_video_duration
    source = factory.create_file_reader(
        filepath, colour_space
    )
    assert source is not None

    file_checker = FileChecker(source)
    file_checker.attach()
    sleep(video_duration)
    file_checker.detach()
    assert file_checker.assert_colour(colour_space)
    assert file_checker.assert_frame_rate(frame_rate)
    assert file_checker.assert_frame_dimensions(
        frame_width, frame_height)
    assert file_checker.assert_data()
    assert file_checker.assert_frame_data_lengths(
        colour_space, frame_width, frame_height)


@mark.observer_pattern
@mark.usefixtures('peri_test')
def test_reader_releases_file_on_destruction(
    filepath, colour_space,
    frame_rate, frame_count,
    frame_width, frame_height
):
    source = None
    global factory
    global video_duration, quarter_video_duration
    source = factory.create_file_reader(
        filepath, colour_space
    )
    assert source is not None
    file_checker_1 = FileChecker(source)
    file_checker_1.attach()
    sleep(quarter_video_duration)
    file_checker_1.detach()
    assert file_checker_1.assert_data()
    del file_checker_1
    del source

    source = None
    source = factory.create_file_reader(
        filepath, colour_space
    )
    file_checker_2 = FileChecker(source)
    file_checker_2.attach()
    sleep(video_duration)
    file_checker_2.detach()
    assert file_checker_2.assert_colour(colour_space)
    assert file_checker_2.assert_frame_rate(frame_rate)
    assert file_checker_2.assert_frame_dimensions(
        frame_width, frame_height)
    assert file_checker_2.assert_data()
    assert file_checker_2.assert_frame_data_lengths(
        colour_space, frame_width, frame_height)


@mark.observer_pattern
@mark.usefixtures('peri_test')
def test_invalid_filepath_throws_exception(colour_space):
    source = None
    global factory
    with raises(RuntimeError):
        source = factory.create_file_reader(
            '/this/path/should/never/exist.video',
            colour_space
        )
    assert source is None


@mark.observer_pattern
@mark.usefixtures('peri_test')
def test_set_sub_frame(
    filepath, colour_space, frame_width, frame_height
):
    global factory
    source = None
    source = factory.create_file_reader(
        filepath, colour_space
    )
    sub_x = frame_width // 4
    sub_y = frame_height // 4
    sub_width = frame_width // 2
    sub_height = frame_height // 2
    assert sub_x > 0 and sub_x + sub_width < frame_width
    assert sub_y > 0 and sub_y + sub_height < frame_height
    source.set_sub_frame(sub_x, sub_y,
                         sub_width, sub_height)
    file_checker = FileChecker(source)
    file_checker.attach()
    global video_duration
    sleep(video_duration)
    file_checker.detach()
    assert file_checker.assert_frame_dimensions(
        sub_width, sub_height)


@mark.observer_pattern
@mark.usefixtures('peri_test')
def test_get_full_frame(
    filepath, colour_space, frame_width, frame_height
):
    global factory
    source = None
    source = factory.create_file_reader(
        filepath, colour_space
    )
    sub_x = frame_width // 4
    sub_y = frame_height // 4
    sub_width = frame_width // 2
    sub_height = frame_height // 2
    assert sub_x > 0 and sub_x + sub_width < frame_width
    assert sub_y > 0 and sub_y + sub_height < frame_height
    source.set_sub_frame(sub_x, sub_y,
                         sub_width, sub_height)
    global quarter_video_duration
    sleep(quarter_video_duration)
    source.get_full_frame()
    file_checker = FileChecker(source)
    file_checker.attach()
    global video_duration
    sleep(video_duration)
    file_checker.detach()
    assert file_checker.assert_frame_dimensions(
        frame_width, frame_height)
