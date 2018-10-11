#!/usr/bin/env python
# -*- coding: utf-8 -*-

from pygiftgrab import (VideoSourceFactory,
                        ColourSpace,
                        IObservableObserver)

"""
This script provides a multi-threading reliability check.
The background is issue #16. It looks like in applications
where multiple Python threads are involved, occasionally
the acquisition of the Global Interpreter Lock leads to a
deadlocks, which crashes the whole application with a
non-specific segmentation fault.

In this script we run a number of multi-threaded GIFT-Grab
pipelines, which should serve as a validation that this
problem is fixed.
"""

if __name__ == '__main__':
    print('multithreading reliability check script')
