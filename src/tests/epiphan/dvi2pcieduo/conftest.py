from pytest import fixture
from pygiftgrab import Device, ColourSpace


def pytest_addoption(parser):
    parser.addoption('--colour-space', action='store',
                     help='Colour space specification (BGRA or I420)')
    parser.addoption('--port', action='store',
                     help='Epiphan DVI2PCIe Duo port specification (SDI or DVI)')
    parser.addoption('--config-dir', action='store',
                     help='Epiphan DVI2PCIe Duo config files directory')


@fixture(scope='session')
def colour_space(request):
    colour_space = request.config.getoption('--colour-space')
    if colour_space == 'BGRA':
        return ColourSpace.BGRA
    elif colour_space == 'I420':
        return ColourSpace.I420
    else:
        raise RuntimeError('Could not recognise colour space ' +
                           colour_space)

@fixture(scope='session')
def port(request):
    port = request.config.getoption('--port')
    if port == 'DVI':
        return Device.DVI2PCIeDuo_DVI
    elif port == 'SDI':
        return Device.DVI2PCIeDuo_SDI
    else:
        raise RuntimeError('Could not recognise Epiphan DVI2PCIe Duo port ' +
                           port)

@fixture(scope='session')
def config_dir(request):
    return request.config.getoption('--config-dir')

