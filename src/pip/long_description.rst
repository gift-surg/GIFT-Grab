IMPORTANT INSTALLATION NOTE
===========================

``pip install gift-grab`` currently works only with ``--install-option``'s.
Please see `this guideline on the GIFT-Grab repository`_.

.. _`this guideline on the GIFT-Grab repository`: https://github.com/gift-surg/GIFT-Grab/blob/master/doc/pypi.md


GIFT-Grab
=========

GIFT-Grab is an open-source C++ and Python API for acquiring, processing and encoding video streams in real time.

GIFT-Grab supports several frame-grabber cards, standard-compliant network streams and video files.

The Python API is compatible with `NumPy`_ and `SciPy`_.
Please note that currently **only Python 2** is supported.

.. _`SciPy`: https://www.scipy.org/
.. _`NumPy`: http://www.numpy.org/


Features
--------

* Capturing video streams using frame-grabber hardware, the following cards are supported:
   - `Epiphan DVI2PCIe Duo`_
   - `Blackmagic DeckLink SDI 4K`_
* Capturing standard-compliant network video streams
* Reading (decoding) video files
* Writing (encoding) video files (including real-time encoding), the following formats are supported:
   - `Xvid`_-encoded `AVI`_
   - `HEVC (H.265)`_-encoded `MP4`_
   - `VP9`_-encoded `WebM`_
* Video data as NumPy arrays to facilitate processing with NumPy-compatible Python libraries

.. _`Epiphan DVI2PCIe Duo`: http://www.epiphan.com/products/dvi2pcie-duo/
.. _`Blackmagic DeckLink SDI 4K`: https://www.blackmagicdesign.com/products/decklink/models
.. _`Xvid`: https://www.xvid.com/
.. _`AVI`: https://msdn.microsoft.com/en-us/library/windows/desktop/dd318189(v=vs.85).aspx
.. _`HEVC (H.265)`: http://www.itu.int/ITU-T/recommendations/rec.aspx?rec=11885
.. _`MP4`: http://www.iso.org/iso/catalogue_detail.htm?csnumber=38538
.. _`VP9`: http://www.webmproject.org/vp9/
.. _`WebM`: https://www.webmproject.org/users/


Getting started
---------------

* Installation
   - `Required tools and libraries`_
   - `Building GIFT-Grab from source`_ (C++ and Python)
   - |InstallUsingPip| (only Python)
* `Python examples with explanation`_
* `Building a minimal GIFT-Grab with NumPy compatibility`_
* API documentation
   - `C++ API documentation`_
   - Python API documentation: please use the C++ API documentation
      * Most Python API elements are equivalent to their C++ counterparts.
      * Differences are noted in the C++ docstrings (e.g. |VideoFrame.data|).
* `Known issues and limitations`_

.. _`Required tools and libraries`: https://github.com/gift-surg/GIFT-Grab/blob/master/doc/requirements.md
.. _`Building GIFT-Grab from source`: https://github.com/gift-surg/GIFT-Grab/blob/master/doc/build.md
.. |VideoFrame.data| replace:: ``data()`` method of ``VideoFrame``
.. _VideoFrame.data: https://codedocs.xyz/gift-surg/GIFT-Grab/classgg_1_1_video_frame.html#a458e15b00b5b2d39855db76215c44055
.. |InstallUsingPip| replace:: Installing GIFT-Grab using ``pip``
.. _InstallUsingPip: https://github.com/gift-surg/GIFT-Grab/blob/master/doc/pypi.md
.. _`Python examples with explanation`: http://gift-grab.rtfd.io
.. _`C++ API documentation`: https://codedocs.xyz/gift-surg/GIFT-Grab/
.. _`Building a minimal GIFT-Grab with NumPy compatibility`: https://github.com/gift-surg/GIFT-Grab/blob/master/doc/minimal.md
.. _`Known issues and limitations`: https://github.com/gift-surg/GIFT-Grab/blob/master/doc/issues.md


Support and contributing
------------------------

Please see the `contribution guide`_ for bug reports, feature requests, and if you would like to contribute to GIFT-Grab.

.. _`contribution guide`: https://github.com/gift-surg/GIFT-Grab/blob/master/CONTRIBUTING.md


Licensing and copyright
-----------------------

Copyright (c) 2015-7, `University College London`_

GIFT-Grab is available as free open-source software under the BSD-3-Clause licence.
Please see the `LICENSE`_ file for details.

Other licences may apply for the GIFT-Grab dependencies.
Please see the `dependency installation guidelines`_ for the implications of using them with regards to licensing.

.. _`University College London`: http://www.ucl.ac.uk
.. _`LICENSE`: https://github.com/gift-surg/GIFT-Grab/blob/master/LICENSE
.. _`dependency installation guidelines`: https://github.com/gift-surg/GIFT-Grab/blob/master/doc/tips.md


Acknowledgements
----------------

GIFT-Grab was developed as part of the `GIFT-Surg`_ project at the `Translational Imaging Group`_ in the `Centre for Medical Image Computing`_ at `University College London`_ (UCL).

This work was supported through an Innovative Engineering for Health award by the `Wellcome Trust`_ [WT101957], the `Engineering and Physical Sciences Research Council`_ [NS/A000027/1] and a `National Institute for Health Research`_ Biomedical Research Centre `UCLH`_ / UCL High Impact Initiative.

.. _`Wellcome Trust`: http://www.wellcome.ac.uk
.. _`Translational Imaging Group`: http://cmictig.cs.ucl.ac.uk
.. _`GIFT-Surg`: http://www.gift-surg.ac.uk
.. _`Centre for Medical Image Computing`: http://cmic.cs.ucl.ac.uk
.. _`National Institute for Health Research`: http://www.nihr.ac.uk/
.. _`UCLH`: http://www.uclh.nhs.uk
.. _`Engineering and Physical Sciences Research Council`: http://www.epsrc.ac.uk
