from pytest import fail, yield_fixture


@yield_fixture(autouse=True)
def peri_test():
    # This section runs before each test

    # TODO
    print('TODO: check factory returns target')

    # Run test
    yield

    # This section runs after each test


def test_frame_rate(codec):
    # TODO
    fail(msg='not implemented')


def test_resolution(codec):
    # TODO
    fail(msg='not implemented')


def test_num_frames(codec):
    # TODO
    fail(msg='not implemented')


def test_can_reuse_target(codec):
    # TODO
    fail(msg='not implemented')


def test_filetype_checked(codec):
    # TODO
    fail(msg='not implemented')
