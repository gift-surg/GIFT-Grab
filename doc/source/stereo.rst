.. _Stereo:

Capturing stereo video
======================

GIFT-Grab allows for capturing stereo video streams from frame grabbers supporting this
feature.
The example below demonstrates how stereo frames can be acquired and saved to individual
image files.
Running this example requires GIFT-Grab built/installed with support for
`Blackmagic DeckLink 4K Extreme 12G`_ and NumPy_.
The example uses OpenCV_ to save video frames to disk.

.. _`Blackmagic DeckLink 4K Extreme 12G`: https://github.com/gift-surg/GIFT-Grab/blob/master/doc/pypi.md#blackmagic-decklink-4k-extreme-12g
.. _NumPy: https://github.com/gift-surg/GIFT-Grab/blob/master/doc/pypi.md#numpy
.. _OpenCV: http://www.opencv.org/

The full source code of the example is below.
Please follow the comments and the flow of code.
This example is also available on GitHub_:

.. literalinclude:: ../../src/tests/blackmagic/stereo_capture.py
    :language: python

.. _GitHub: https://github.com/gift-surg/GIFT-Grab/blob/master/src/tests/blackmagic/stereo_capture.py
