from pygiftgrab import Device
from epiphan import BGR24, I420


def pytest_addoption(parser):
    parser.addoption('--colour-space', action='store',
                     help='Colour space specification (BGR24 or I420)')
    parser.addoption('--port', action='store',
                     help='Epiphan DVI2PCIe Duo port specification (SDI or DVI)')


def pytest_generate_tests(metafunc):
    if 'colour_space' in metafunc.fixturenames:
        if metafunc.config.option.colour_space == 'BGR24':
            colour_space = BGR24
        elif metafunc.config.option.colour_space == 'I420':
            colour_space = I420
        else:
            raise RuntimeError('Could not recognise colour space ' +
                               metafunc.config.option.colour_space)
        metafunc.parametrize('colour_space', [colour_space])

    if 'port' in metafunc.fixturenames:
        if metafunc.config.option.port == 'SDI':
            port = Device.DVI2PCIeDuo_DVI
        elif metafunc.config.option.port == 'DVI':
            port = Device.DVI2PCIeDuo_SDI
        else:
            raise RuntimeError('Could not recognise Epiphan DVI2PCIe Duo port ' +
                               metafunc.config.option.port)
        metafunc.parametrize('port', [port])
