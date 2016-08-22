#!/usr/bin/env python

from os.path import join, abspath
from pkg_resources import resource_filename
from pytest import main


def test_giftgrab():
    working_dir = abspath(resource_filename('giftgrab.tests', 'target'))
    main(['--codec=H265', working_dir])

    working_dir = abspath(resource_filename('giftgrab.tests', 'target'))
    main(['--codec=XviD', working_dir])

    working_dir = abspath(
            resource_filename('giftgrab.tests',
                              join('epiphan', 'dvi2pcieduo')))
    config_dir = abspath(
            resource_filename('giftgrab',
                              join('data', 'epiphan')))
    for port in ['SDI', 'DVI']:
        for colour_space in ['BGR24', 'I420']:
            main(['--colour-space=%s' % (colour_space),
                  '--port=%s' % (port),
                  '--config-dir=%s' % (config_dir),
                  working_dir])
