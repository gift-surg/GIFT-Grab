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
    testing_network_source_frame_rate = os.environ.get(var_name)
    if testing_network_source_frame_rate is None:
        fail('%s %s %s' % ('Environment variable ',
                           var_name,
                           ' needs to be set for testing network source support')
        )
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
                '--frame-rate=%f' % (testing_network_source_frame_rate),
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


def test_network_sources_bgra():
    __run_network_tests('BGRA')


def test_network_sources_i420():
    __run_network_tests('I420')


def test_numpy_bgra():
    __run_numpy_tests('BGRA')


def test_numpy_i420():
    __run_numpy_tests('I420')
