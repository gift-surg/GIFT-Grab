from pygiftgrab import Codec, ColourSpace


def pytest_addoption(parser):
    parser.addoption('--codec', action='store',
                     help='Codec (HEVC, Xvid, or VP9)')
    parser.addoption('--colour-space', action='store',
                     help='Colour space specification (BGRA, I420, or UYVY)')


def pytest_generate_tests(metafunc):
    if 'codec' in metafunc.fixturenames:
        codec_str = str(metafunc.config.option.codec)
        case_insensitive = codec_str.lower()
        if case_insensitive == 'hevc':
            codec = Codec.HEVC
        elif case_insensitive == 'xvid':
            codec = Codec.Xvid
        elif case_insensitive == 'vp9':
            codec = Codec.VP9
        else:
            raise RuntimeError('Could not recognise codec ' +
                               codec_str)
        metafunc.parametrize('codec', [codec])

    if 'colour_space' in metafunc.fixturenames:
        colour_space_str = str(metafunc.config.option.colour_space)
        case_insensitive = colour_space_str.lower()
        if case_insensitive == 'bgra':
            colour_space = ColourSpace.BGRA
        elif case_insensitive == 'i420':
            colour_space = ColourSpace.I420
        elif case_insensitive == 'uyvy':
            colour_space = ColourSpace.UYVY
        else:
            raise RuntimeError('Could not recognise colour space ' +
                               colour_space_str)
        metafunc.parametrize('colour_space', [colour_space])
