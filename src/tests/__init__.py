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


def test_hevc():
    working_dir = abspath(resource_filename('giftgrab.tests', 'target'))
    ret = main(['--codec=HEVC', working_dir])
    if ret: exit(ret)


def test_xvid():
    working_dir = abspath(resource_filename('giftgrab.tests', 'target'))
    ret = main(['--codec=XviD', working_dir])
    if ret: exit(ret)


def test_vp9():
    working_dir = abspath(resource_filename('giftgrab.tests', 'target'))
    ret = main(['--codec=VP9', working_dir])
    if ret: exit(ret)


def test_epiphan_dvi2pcieduo_bgr24():
    for port in ['SDI', 'DVI']:
        ret = main(['--colour-space=%s' % ('BGR24'),
                    '--port=%s' % (port),
                    '--config-dir=%s' % (epiphan_dvi2pcieduo_config_dir),
                    epiphan_dvi2pcieduo_working_dir])
        if ret: exit(ret)


def test_epiphan_dvi2pcieduo_i420():
    for port in ['SDI', 'DVI']:
        ret = main(['--colour-space=%s' % ('I420'),
                    '--config-dir=%s' % (epiphan_dvi2pcieduo_config_dir),
                    epiphan_dvi2pcieduo_working_dir, '-m', 'real_time'])
        if ret: exit(ret)

    for port in ['SDI', 'DVI']:
        ret = main(['--colour-space=%s' % ('I420'),
                    '--port=%s' % (port),
                    epiphan_dvi2pcieduo_working_dir, '-m', 'unit'])
        if ret: exit(ret)
