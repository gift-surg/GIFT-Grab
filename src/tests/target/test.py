from pytest import fail, yield_fixture


@yield_fixture(autouse=True)
def peri_test():
    # This section runs before each test

    # TODO
    print('TODO: check factory returns target for H265')

    # Run test
    yield

    # This section runs after each test


def test_frame_rate():
    # TODO
    fail(msg='not implemented')


def test_resolution():
    # TODO
    fail(msg='not implemented')


def test_num_frames():
    # TODO
    fail(msg='not implemented')


def test_can_reuse_target():
    # TODO
    fail(msg='not implemented')


def test_filetype_checked():
    # TODO
    fail(msg='not implemented')
