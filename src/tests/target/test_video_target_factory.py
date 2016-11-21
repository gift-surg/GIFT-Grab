from pytest import mark, raises
from pygiftgrab import VideoTargetFactory, Storage, VideoFrame, ColourSpace
import os.path
import os.remove


@mark.video_target_factory
def test_get_instance():
    factory_1 = VideoTargetFactory.get_instance()
    factory_2 = VideoTargetFactory.get_instance()
    assert factory_1 is factory_2


@mark.video_target_factory
def test_create_file_writer(codec, colour_space):
    factory = VideoTargetFactory.get_instance()

    filename = '/fill/me.mp4'  # TODO
    assert not os.path.isfile(filename)

    writer = None
    writer = factory.create_file_writer(codec, filename)
    assert writer is not None

    frame = VideoFrame(colour_space, 1920, 1080)
    writer.append(frame)
    del writer

    assert os.path.isfile(filename)
    os.remove(filename)
