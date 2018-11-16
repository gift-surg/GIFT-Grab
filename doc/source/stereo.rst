.. _Stereo:

Capturing stereo video
======================

GIFT-Grab allows for capturing stereo video streams from frame grabbers supporting this
feature.
The example below demonstrates how stereo frames can be acquired and saved to individual
image files.
Running this example requires a `Blackmagic DeckLink 4K Extreme 12G`_ and OpenCV_.

.. _`Blackmagic DeckLink 4K Extreme 12G`: https://www.blackmagicdesign.com/products/decklink/models
.. _OpenCV: http://www.opencv.org/

The full source code of the example is below.
Please follow the comments and the flow of code.
This example is also available on GitHub_:

.. literalinclude:: ../../src/tests/blackmagic/stereo_capture.py
    :language: python

.. _GitHub: https://github.com/gift-surg/GIFT-Grab/blob/master/src/tests/blackmagic/stereo_capture.py
