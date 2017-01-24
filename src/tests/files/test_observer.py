from pytest import fail, mark, yield_fixture
from pygiftgrab import VideoSourceFactory


factory = None
video_duration = 0  # inferred, in sec


@yield_fixture(scope='session')
def peri_test(codec, colour_space, filepath,
              frame_rate, frame_count,
              frame_width, frame_height):
	factory = VideoSourceFactory.get_instance()
	video_duration = frame_count / frame_rate
	yield


@mark.observer_pattern
def test_valid_filepath_returns_raii_reader(
	filepath, codec, colour_space,
	frame_rate, frame_count,
	frame_width, frame_height
):
	fail('tests not implemented yet')


@mark.observer_pattern
def test_reader_releases_file_on_destruction(
	filepath, codec, colour_space,
	frame_rate, frame_count,
	frame_width, frame_height
):
	fail('tests not implemented yet')


@mark.observer_pattern
def test_invalid_filepath_throws_exception(
	codec, colour_space
):
	fail('tests not implemented yet')
