#!/usr/bin/env python

import config
from time import sleep

if __name__ == '__main__':
    from_file_path = 'from.yml'
    to_file_path = 'to.yml'
    et = config.parse_config(file_path=from_file_path)
    # et.start()
    # sleep(5)
    # et.stop()
    if et:
        config.write_config(epiphan_thread=et,
                            file_path=to_file_path)
