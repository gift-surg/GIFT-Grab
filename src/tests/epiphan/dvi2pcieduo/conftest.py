from pygiftgrab import Device


def pytest_addoption(parser):
    parser.addoption('--port', action='store',
                     help='Epiphan DVI2PCIe Duo port specification (SDI or DVI)')


def pytest_generate_tests(metafunc):
    if 'port' in metafunc.fixturenames:
        if metafunc.config.option.port == 'SDI':
            port = Device.DVI2PCIeDuo_DVI
        elif metafunc.config.option.port == 'DVI':
            port = Device.DVI2PCIeDuo_SDI
        else:
            raise RuntimeError('Could not recognise Epiphan DVI2PCIe Duo port ' +
                               metafunc.config.option.port)
        metafunc.parametrize('port', [port])
