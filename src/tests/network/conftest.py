from pytest import fixture
from pygiftgrab import ColourSpace


def pytest_addoption(parser):
    parser.addoption('--address', action='store',
                     help='Network source address')
    parser.addoption('--init-delay', action='store', type=float,
                     help='Network source initialisation delay (sec)')
    parser.addoption('--colour-space', action='store',
                     help='Colour space specification (BGRA or I420)')
    parser.addoption('--frame-rate', action='store', type=int,
                     help='Frame-rate specification for test')
    parser.addoption('--observers', action='store', type=int,
                     help='Number of observers to use when testing frame rate')


@fixture(scope='session')
def address(request):
    return request.config.getoption('--address')


@fixture(scope='session')
def init_delay(request):
    return request.config.getoption('--init-delay')


@fixture(scope='session')
def colour_space(request):
    colour_space = request.config.getoption('--colour-space')
    case_insensitive = colour_space.lower()
    if case_insensitive == 'bgra':
        return ColourSpace.BGRA
    elif case_insensitive == 'i420':
        return ColourSpace.I420
    else:
        raise RuntimeError('Could not recognise colour space ' +
                           colour_space)


@fixture(scope='session')
def frame_rate(request):
    return request.config.getoption('--frame-rate')


@fixture(scope='session')
def observers(request):
    return request.config.getoption('--observers')
