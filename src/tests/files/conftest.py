from pytest import fixture
from pygiftgrab import Codec, ColourSpace


def pytest_addoption(parser):
    parser.addoption('--colour-space', action='store', type=str, required=True,
                     help='Colour space specification (BGRA or I420)')
    parser.addoption('--filepath', action='store', type=str, required=True,
                     help='Video file to use')
    parser.addoption('--frame-rate', action='store', type=float, required=True,
                     help='Frame rate of video file')
    parser.addoption('--frame-count', action='store', type=int, required=True,
                     help='No. of frames in video file')
    parser.addoption('--frame-width', action='store', type=int, required=True,
                     help='Width of frames in video file')
    parser.addoption('--frame-height', action='store', type=int, required=True,
                     help='Height of frames in video file')


@fixture(scope='session')
def colour_space(request):
    colour_space = request.config.getoption('--colour-space')
    case_insensitive = colour_space.lower()
    if case_insensitive == 'bgra':
        return ColourSpace.BGRA
    elif case_insensitive == 'i420':
        return ColourSpace.I420
    elif case_insensitive == 'uyvy':
        return ColourSpace.UYVY
    else:
        raise RuntimeError('Could not recognise colour space ' +
                           colour_space)


@fixture(scope='session')
def filepath(request):
    filepath = request.config.getoption('--filepath')
    if not filepath:
        raise RuntimeError('Filepath cannot be empty')
    else:
        return filepath


@fixture(scope='session')
def frame_rate(request):
    frame_rate = float(request.config.getoption('--frame-rate'))
    if frame_rate <= 0:
        raise RuntimeError('Frame rate must be positive')
    else:
        return frame_rate


@fixture(scope='session')
def frame_count(request):
    frame_count = int(request.config.getoption('--frame-count'))
    if frame_count <= 0:
        raise RuntimeError('Frame count must be positive')
    else:
        return frame_count


@fixture(scope='session')
def frame_width(request):
    frame_width = int(request.config.getoption('--frame-width'))
    if frame_width <= 0:
        raise RuntimeError('Frame width must be positive')
    else:
        return frame_width


@fixture(scope='session')
def frame_height(request):
    frame_height = int(request.config.getoption('--frame-height'))
    if frame_height <= 0:
        raise RuntimeError('Frame height must be positive')
    else:
        return frame_height