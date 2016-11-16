from pytest import fixture
from pygiftgrab import Device, ColourSpace


def pytest_addoption(parser):
    parser.addoption('--colour-space', action='store',
                     help='Colour space specification (BGRA or I420)')
    parser.addoption('--port', action='store',
                     help='Epiphan DVI2PCIe Duo port specification (SDI or DVI)')
    parser.addoption('--config-dir', action='store',
                     help='Epiphan DVI2PCIe Duo config files directory')
    parser.addoption('--frame-rate', action='store', type=int,
                     help='Frame-rate specification for test')
    parser.addoption('--observers', action='store', type=int,
                     help='Number of observers to use when testing frame rate')


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
    else:
        raise RuntimeError('Could not recognise colour space ' +
                           colour_space)

@fixture(scope='session')
def port(request):
    port = request.config.getoption('--port')
    case_insensitive = port.lower()
    if case_insensitive == 'dvi':
        return Device.DVI2PCIeDuo_DVI
    elif case_insensitive == 'sdi':
        return Device.DVI2PCIeDuo_SDI
    else:
        raise RuntimeError('Could not recognise Epiphan DVI2PCIe Duo port ' +
                           port)

@fixture(scope='session')
def config_dir(request):
    return request.config.getoption('--config-dir')
