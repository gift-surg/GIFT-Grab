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


def test_network_sources_bgra():
    __run_network_tests('BGRA')


def test_network_sources_i420():
    __run_network_tests('I420')
