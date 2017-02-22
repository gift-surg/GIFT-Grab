#!/usr/bin/env python

from os.path import join, abspath
import os
from pkg_resources import resource_filename
from pytest import main, fail


epiphan_dvi2pcieduo_working_dir = abspath(
    resource_filename('giftgrab.tests',
                      join('epiphan', 'dvi2pcieduo')))
network_sources_working_dir = abspath(
    resource_filename('giftgrab.tests', join('network'))
)
epiphan_dvi2pcieduo_config_dir = abspath(
    resource_filename('giftgrab.tests',
                      join('epiphan', 'dvi2pcieduo', 'data')))
blackmagic_decklinksdi4k_working_dir = abspath(
    resource_filename('giftgrab.tests',
                      join('blackmagic', 'decklinksdi4k')))
videoframe_working_dir = abspath(
    resource_filename('giftgrab.tests', 'videoframe'))


def __run_target_tests(codec, colour_space):
    working_dir = abspath(resource_filename('giftgrab.tests', 'target'))
    ret = main(['--codec={}'.format(codec),
                '--colour-space={}'.format(colour_space),
                working_dir])
    if ret: exit(ret)


def test_hevc_bgra():
    __run_target_tests('HEVC', 'BGRA')


def test_hevc_i420():
    __run_target_tests('HEVC', 'I420')


def test_xvid_bgra():
    __run_target_tests('Xvid', 'BGRA')


def test_xvid_i420():
    __run_target_tests('Xvid', 'I420')


def test_vp9_bgra():
    __run_target_tests('VP9', 'BGRA')


def test_vp9_i420():
    __run_target_tests('VP9', 'I420')


def __run_file_tests(codec, colour_space):
    working_dir = abspath(resource_filename('giftgrab.tests', 'files'))
    data_dir = abspath(resource_filename('giftgrab.tests',
                       join('files', 'data')))
    if codec == 'HEVC':
        extension = '.mp4'
    elif codec == 'Xvid':
        extension = '.avi'
    elif codec == 'VP9':
        extension = '.webm'
    frame_count = 15
    frame_rate = 30
    filename = 'video_{}frames_{}fps{}'.format(
                    frame_count, frame_rate, extension)
    filepath = os.path.join(data_dir, filename)
    frame_width = 128
    frame_height = 64
    ret = main(['--colour-space={}'.format(colour_space),
                '--filepath={}'.format(filepath),
                '--frame-rate={}'.format(frame_rate),
                '--frame-count={}'.format(frame_count),
                '--frame-width={}'.format(frame_width),
                '--frame-height={}'.format(frame_height),
                working_dir, '-m', 'observer_pattern'])
    if ret: exit(ret)


def test_file_hevc_bgra():
    __run_file_tests('HEVC', 'BGRA')


def test_file_hevc_i420():
    __run_file_tests('HEVC', 'I420')


def test_file_hevc_uyvy():
    __run_file_tests('HEVC', 'UYVY')


def test_file_xvid_bgra():
    __run_file_tests('Xvid', 'BGRA')


def test_file_xvid_i420():
    __run_file_tests('Xvid', 'I420')


def test_file_xvid_uyvy():
    __run_file_tests('Xvid', 'UYVY')


def test_file_vp9_bgra():
    __run_file_tests('VP9', 'BGRA')


def test_file_vp9_i420():
    __run_file_tests('VP9', 'I420')


def test_file_vp9_uyvy():
    __run_file_tests('VP9', 'UYVY')


def __run_epiphan_tests(colour_space):
    for port in ['SDI', 'DVI']:
        ret = main(['--colour-space=%s' % (colour_space),
                    '--port=%s' % (port),
                    epiphan_dvi2pcieduo_working_dir, '-m', 'unit'])
        if ret: exit(ret)

    # Disabled the Python module tests until issue #114 is resolved
    # ret = main(['--colour-space=%s' % (colour_space),
    #             '--config-dir=%s' % (epiphan_dvi2pcieduo_config_dir),
    #             epiphan_dvi2pcieduo_working_dir, '-m', 'epiphan_py_module'])
    # if ret: exit(ret)


def __run_blackmagic_tests(colour_space):
    ret = main(['--colour-space=%s' % (colour_space),
                blackmagic_decklinksdi4k_working_dir, '-m', 'unit'])
    if ret: exit(ret)

    ret = main(['--colour-space=%s' % (colour_space),
                '--frame-rate=27',
                '--observers=3',
                blackmagic_decklinksdi4k_working_dir, '-m', 'observer_pattern'])
    if ret: exit(ret)


def __run_network_tests(colour_space):
    # parse needed arguments from environment variables
    var_name = 'TESTING_NETWORK_SOURCE_ADDRESS'
    testing_network_source_address = os.environ.get(var_name)
    if testing_network_source_address is None:
        fail('%s %s %s' % ('Environment variable ',
                           var_name,
                           ' needs to be set for testing network source support')
        )
    var_name = 'TESTING_NETWORK_SOURCE_FRAME_RATE'
    testing_network_source_frame_rate_buffer = os.environ.get(var_name)
    if testing_network_source_frame_rate_buffer is None:
        fail('%s %s %s' % ('Environment variable ',
                           var_name,
                           ' needs to be set for testing network source support')
        )
    else:
        testing_network_source_frame_rate = int(testing_network_source_frame_rate_buffer)

    var_name = 'TESTING_NETWORK_SOURCE_DELAY'
    testing_network_source_delay = 0.0
    if os.environ.get(var_name):
        testing_network_source_delay_buffer = os.environ.get(var_name)
        try:
            testing_network_source_delay = float(testing_network_source_delay_buffer)
        except ValueError as e:
            fail('Could not parse %s value %s' % (var_name,
                                                  testing_network_source_delay_buffer)
            )

    # run unit tests
    ret = main(['--address=%s' % (testing_network_source_address),
                '--colour-space=%s' % (colour_space),
                '--init-delay=%f' % (testing_network_source_delay),
                network_sources_working_dir, '-m', 'unit'])
    if ret: exit(ret)

    # run observer pattern tests
    observers = 3
    ret = main(['--address=%s' % (testing_network_source_address),
                '--colour-space=%s' % (colour_space),
                '--frame-rate=%i' % (testing_network_source_frame_rate),
                '--observers=%i' % (observers),
                '--init-delay=%f' % (testing_network_source_delay),
                network_sources_working_dir, '-m', 'observer_pattern'])
    if ret: exit(ret)


def __run_numpy_tests(colour_space):
    ret = main(['--colour-space=%s' % (colour_space),
                videoframe_working_dir, '-m', 'numpy_compatibility'])
    if ret: exit(ret)


def test_epiphan_dvi2pcieduo_bgra():
    __run_epiphan_tests('BGRA')


def test_epiphan_dvi2pcieduo_i420():
    __run_epiphan_tests('I420')


def test_blackmagic_decklinksdi4k_uyvy():
    __run_blackmagic_tests('UYVY')


def test_network_sources_bgra():
    __run_network_tests('BGRA')


def test_network_sources_i420():
    __run_network_tests('I420')


def test_numpy_bgra():
    __run_numpy_tests('BGRA')


def test_numpy_i420():
    __run_numpy_tests('I420')
