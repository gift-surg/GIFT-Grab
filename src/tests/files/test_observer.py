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
def peri_test(codec, colour_space, filepath,
              frame_rate, frame_count,
              frame_width, frame_height):
	factory = VideoSourceFactory.get_instance()
	video_duration = frame_count / frame_rate
	quarter_video_duration = video_duration / 4
	yield


@mark.observer_pattern
def test_valid_filepath_returns_raii_reader(
	filepath, codec, colour_space,
	frame_rate, frame_count,
	frame_width, frame_height
):
	source = None
	source = factory.create_file_reader(
		filepath, codec, colour_space
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
		colour, frame_width, frame_height)


@mark.observer_pattern
def test_reader_releases_file_on_destruction(
	filepath, codec, colour_space,
	frame_rate, frame_count,
	frame_width, frame_height
):
	source = None
	source = factory.create_file_reader(
		filepath, codec, colour_space
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
		filepath, codec, colour_space
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
		colour, frame_width, frame_height)


@mark.observer_pattern
def test_invalid_filepath_throws_exception(
	codec, colour_space
):
	source = None
	with raises(RuntimeError):
		source = factory.create_file_reader(
            '/this/path/should/never/exist.video',
            codec, colour_space
        )
	assert source is None
