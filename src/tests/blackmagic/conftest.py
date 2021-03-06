from pytest import fixture
from pygiftgrab import (ColourSpace, Device)


def pytest_addoption(parser):
    parser.addoption('--device', action='store',
                     help='Device to test (decklinksdi4k, or decklink4kextreme12g)')
    parser.addoption('--colour-space', action='store',
                     help='Colour space specification (BGRA, I420, or UYVY)')
    parser.addoption('--frame-rate', action='store', type=int,
                     help='Frame-rate specification for test')
    parser.addoption('--observers', action='store', type=int,
                     help='Number of observers to use when testing frame rate')


@fixture(scope='session')
def device(request):
    device = request.config.getoption('--device')
    case_insensitive = device.lower()
    if case_insensitive == 'decklinksdi4k':
        return Device.DeckLinkSDI4K
    elif case_insensitive == 'decklink4kextreme12g':
        return Device.DeckLink4KExtreme12G
    else:
        raise RuntimeError('Could not recognise device {}'.format(device))


@fixture(scope='session')
def frame_rate(request):
    return request.config.getoption('--frame-rate')


@fixture(scope='session')
def observers(request):
    return request.config.getoption('--observers')


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

