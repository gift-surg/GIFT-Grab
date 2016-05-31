from epiphan import BGR24, I420


def pytest_addoption(parser):
    parser.addoption("--bgr24", action="store_true",
                     help="Run tests for BGR24 colour space (up to 30 fps)")
    parser.addoption("--i420", action="store_true",
                     help="Run tests for I420 colour space (up to 60 fps)")


def pytest_generate_tests(metafunc):
    if 'colour_space' in metafunc.fixturenames:
        if metafunc.config.option.bgr24:
            colour_space = BGR24
        elif metafunc.config.option.i420:
            colour_space = I420
        else:
            colour_space = BGR24  # default
        metafunc.parametrize('colour_space', colour_space)
