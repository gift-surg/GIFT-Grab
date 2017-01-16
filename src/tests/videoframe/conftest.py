from pytest import fixture
from pygiftgrab import ColourSpace


def pytest_addoption(parser):
    parser.addoption('--colour-space', action='store',
                     help='Colour space specification (BGRA or I420)')


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
