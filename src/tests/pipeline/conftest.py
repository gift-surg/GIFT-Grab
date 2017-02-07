from pytest import fixture


def pytest_addoption(parser):
    parser.addoption('--filepath', action='store', type=str, required=True,
                     help='Video file to use')


@fixture(scope='session')
def filepath(request):
    filepath = request.config.getoption('--filepath')
    if not filepath:
        raise RuntimeError('Filepath cannot be empty')
    else:
        return filepath
