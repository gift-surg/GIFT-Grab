from pygiftgrab import Storage


def pytest_addoption(parser):
    parser.addoption('--codec', action='store',
                     help='Codec (HEVC, Xvid, or VP9)')


def pytest_generate_tests(metafunc):
    if 'codec' in metafunc.fixturenames:
        codec_str = str(metafunc.config.option.codec)
        case_insensitive = codec_str.lower()
        if case_insensitive == 'hevc':
            codec = Storage.File_HEVC
        elif case_insensitive == 'xvid':
            codec = Storage.File_XviD
        elif case_insensitive == 'vp9':
            codec = Storage.File_VP9
        else:
            raise RuntimeError('Could not recognise codec ' +
                               codec_str)
        metafunc.parametrize('codec', [codec])
