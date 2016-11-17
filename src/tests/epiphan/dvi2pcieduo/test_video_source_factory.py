from pytest import mark, raises, fixture
from pygiftgrab import VideoSourceFactory, VideoFrame, ColourSpace, Device


@fixture(params=[0, 1], ids=[Device.DVI2PCIeDuo_DVI, Device.DVI2PCIeDuo_SDI])
def device(request):
    return request.param


@mark.video_source_factory
def test_get_instance_returns_factory():
    factory = None
    factory = VideoSourceFactory.get_instance()
    assert factory is not None


@mark.video_source_factory
def test_get_instance_returns_singleton():
    factory = None
    factory_ = None
    factory = VideoSourceFactory.get_instance()
    factory_ = VideoSourceFactory.get_instance()
    assert factory is factory_  # i.e. singleton


@mark.video_source_factory
def test_get_device_returns_connection(device, colour_space):
    factory = VideoSourceFactory.get_instance()

    source = None
    source = factory.get_device(device, colour_space)
    assert source is not None

    frame = VideoFrame(colour_space, False)
    assert frame.cols() == 0
    assert frame.rows() == 0
    assert source.get_frame(frame)
    assert frame.cols() > 0
    assert frame.rows() > 0


@mark.video_source_factory
def test_get_device_returns_singleton(device, colour_space):
    factory = VideoSourceFactory.get_instance()

    source = factory.get_device(device, colour_space)
    source_ = factory.get_device(device, colour_space)
    assert source is source_


@mark.video_source_factory
def test_get_device_does_not_create_duplicate(device, colour_space):
    factory = VideoSourceFactory.get_instance()
    source = factory.get_device(device, colour_space)

    source_duplicate = None
    if colour_space == ColourSpace.I420:
        other_colour_space = ColourSpace.BGRA
    else:
        other_colour_space = ColourSpace.I420
    with raises(IOError):
        source_duplicate = factory.get_device(device, other_colour_space)
    assert source_duplicate is None


@mark.video_source_factory
def test_factory_garbage_collects_devices(colour_space):
    source_dvi = None
    source_sdi = None

    if True:  # artificial scope: factory is deleted at the end
        factory = VideoSourceFactory.get_instance()

        source_dvi = factory.get_device(Device.DVI2PCIeDuo_DVI, colour_space)
        source_sdi = factory.get_device(Device.DVI2PCIeDuo_SDI, colour_space)
        assert source_dvi is not None
        assert source_sdi is not None

    assert source_dvi is None
    assert source_sdi is None
