#!/usr/bin/env python

from os.path import join, abspath
from pkg_resources import resource_filename
from pytest import main


epiphan_dvi2pcieduo_working_dir = abspath(
    resource_filename('giftgrab.tests',
                      join('epiphan', 'dvi2pcieduo')))
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


def __run_numpy_tests(colour_space):
    ret = main(['--colour-space=%s' % (colour_space),
                videoframe_working_dir, '-m', 'numpy_compatibility'])
    if ret: exit(ret)


def test_epiphan_dvi2pcieduo_bgra():
    __run_epiphan_tests('BGRA')


def test_epiphan_dvi2pcieduo_i420():
    __run_epiphan_tests('I420')


def test_numpy_bgra():
    __run_numpy_tests('BGRA')


def test_numpy_i420():
    __run_numpy_tests('I420')
