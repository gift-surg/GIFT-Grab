#!/usr/bin/env python


def parse_config(file_path):
    """Attempt to parse YAML recording configuration file.

    @param file_path
    @return an `EpiphanThread` parametrised in `file_path`
    if parsing succeeds, ``None`` otherwise
    """
    print 'parse_config'


def write_config(epiphan_thread, file_path):
    """Serialise `epiphan_thread`'s configuration to YAML file.

    @param epiphan_thread
    @param file_path
    @return ``True`` if writing succeeds, ``False`` otherwise
    """
    print 'write_config'
