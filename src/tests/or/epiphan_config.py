#!/usr/bin/env python

from yaml import load, dump, YAMLError
import pygiftgrab


def parse_config(file_path):
    """Attempt to parse YAML recording configuration file.

    @param file_path
    @return ``port, frame_rate, file_path, timeout_limit``
    of an `Recorder` if parsing succeeds, ``None``
    otherwise
    """
    with open(file_path, 'r') as stream:
        try:
            data = load(stream)
        except YAMLError as e:
            print 'Parsing file ' + file_path +\
                  ' failed with: ' + e.message
            return None
        else:
            file_path = data['file_path']
            frame_rate = data['frame_rate']
            timeout_limit = data['timeout_limit']
            port = None
            if data['port'] == 'SDI':
                port = pygiftgrab.Device.DVI2PCIeDuo_SDI
            elif data['port'] == 'DVI':
                port = pygiftgrab.Device.DVI2PCIeDuo_DVI
            else:
                print 'Port ' + data['port'] + ' not recognised.'
                return None

            return port, frame_rate, file_path, timeout_limit


def write_config(epiphan_thread, file_path):
    """Serialise `epiphan_thread`'s configuration to YAML file.

    @param epiphan_thread
    @param file_path
    @return ``True`` if writing succeeds, ``False`` otherwise
    """
    with open(file_path, 'w') as stream:
        try:
            if epiphan_thread.port == pygiftgrab.Device.DVI2PCIeDuo_SDI:
                port = 'SDI'
            elif epiphan_thread.port == pygiftgrab.Device.DVI2PCIeDuo_DVI:
                port = 'DVI'
            else:
                print 'Port ' + str(epiphan_thread.port) + ' unknown'
                return False
            data = dict(port=port,
                        frame_rate=epiphan_thread.frame_rate,
                        file_path=epiphan_thread.file_path,
                        timeout_limit=epiphan_thread.timeout_limit)
            stream.write(dump(data, default_flow_style=False))
            stream.close()
        except YAMLError as e:
            print 'Dumping thread configuration to file ' +\
                  file_path + ' failed with: ' + e.message
            return False
        else:
            return True
