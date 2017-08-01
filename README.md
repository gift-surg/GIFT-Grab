# GIFT-Grab

An open-source C++ and Python API for acquiring, processing and encoding video streams in real time.

GIFT-Grab supports several frame-grabber cards, standard-compliant network streams and video files.

The Python API is compatible with [NumPy][numpy] and [SciPy][scipy].
Please note that currently **only Python 2** is supported.

[scipy]: https://www.scipy.org/
[numpy]: http://www.numpy.org/


# Examples

[Python examples with explanation][rtd]

[rtd]: http://gift-grab.rtfd.io
[pip-install-ops]: https://pip.pypa.io/en/stable/reference/pip_install/#cmdoption-install-option


# API documentation

* [C++ API documentation](https://codedocs.xyz/gift-surg/GIFT-Grab/)
* Python API documentation: please use the C++ API documentation
  * Most Python API elements are equivalent to their C++ counterparts.
  * Differences are noted in the C++ docstrings (e.g. [`data()` method of `VideoFrame`][vf-data]).

[vf-data]: https://codedocs.xyz/gift-surg/GIFT-Grab/classgg_1_1_video_frame.html#a458e15b00b5b2d39855db76215c44055


# Installation

* [Build GIFT-Grab from source][build-from-source] (C++ and Python)
* [Install GIFT-Grab using `pip`][pip] (only Python)

[build-from-source]: doc/build.md
[pip]: doc/pypi.md


# Features

**Hint:** the "HOWTO"s in the table are quick links to the relevant installation documentation. **IF DOING `pip install gift-grab`**, please make sure you use the appropriate [`pip --install-option`s][pip-install-ops] for enabling the desired features (details [here](doc/pypi.md)).

| **Feature** | | [**C++ API**](doc/build.md) | [**Python API**](doc/pypi.md) |
| :--- | :--- | :---: | :---: |
| **Frame-grabbers** | | | |
| | [Epiphan DVI2PCIe Duo][e-dd] | [HOWTO](doc/build.md#epiphan-dvi2pcie-duo) | [HOWTO](doc/pypi.md#epiphan-dvi2pcie-duo) |
| | [Blackmagic DeckLink SDI 4K][bm-dls4] | [HOWTO](doc/build.md#blackmagic-decklink-sdi-4k) |  [HOWTO](doc/pypi.md#blackmagic-decklink-sdi-4k) |
| **Network streams** | | [HOWTO](doc/build.md#network-streams) | [HOWTO](doc/pypi.md#network-streams) |
| **Video files** | | | |
| | Reading | [HOWTO](doc/build.md#reading-video-files) | [HOWTO](doc/pypi.md#reading-video-files) |
| | Writing [Xvid][xvid]-encoded [AVI][avi] files | [HOWTO](doc/build.md#xvid) | [HOWTO](doc/pypi.md#xvid) |
| | Writing [HEVC (H.265)][hevc]-encoded [MP4][mp4] files | [HOWTO](doc/build.md#hevc) | [HOWTO](doc/pypi.md#hevc) |
| | Writing [VP9][vp9]-encoded [WebM][webm] files | [HOWTO](doc/build.md#vp9) | [HOWTO](doc/pypi.md#vp9) |
| **[NumPy][numpy] / [SciPy][scipy] compatibility** | | [HOWTO](doc/build.md#python-api) | [HOWTO](doc/pypi.md#numpy) |


# Testing

* [Testing GIFT-Grab built from source](doc/build.md#testing-gift-grab) 
* [Testing GIFT-Grab installed with `pip`](doc/pypi.md#testing-gift-grab)


[e-dd]: http://www.epiphan.com/products/dvi2pcie-duo/
[bm-dls4]: https://www.blackmagicdesign.com/products/decklink/models
[xvid]: https://www.xvid.com/
[avi]: https://msdn.microsoft.com/en-us/library/windows/desktop/dd318189(v=vs.85).aspx
[hevc]: http://www.itu.int/ITU-T/recommendations/rec.aspx?rec=11885
[mp4]: http://www.iso.org/iso/catalogue_detail.htm?csnumber=38538
[vp9]:http://www.webmproject.org/vp9/
[webm]: https://www.webmproject.org/users/


# Support and contact

Please see the [contribution guide][cg] for bug reports, feature requests, contact information, etc.

[cg]: CONTRIBUTING.md

# Licensing and copyright

Copyright (c) 2015-7, [University College London][ucl].

GIFT-Grab is available as free open-source software under the BSD-3-Clause licence.
Please see the LICENSE file for details.

Other licences may apply for the GIFT-Grab dependencies.
Please see the [dependency installation guidelines][gg-tips] for the implications of using them with regards to licensing.

[gg-tips]: doc/tips.md


# Funding

GIFT-Grab was developed as part of the [GIFT-Surg][giftsurg] project at the [Translational Imaging Group][tig] in the [Centre for Medical Image Computing][cmic] at [University College London (UCL)][ucl].

This work was supported through an Innovative Engineering for Health award by the [Wellcome Trust][wellcometrust] [WT101957], the [Engineering and Physical Sciences Research Council (EPSRC)][epsrc] [NS/A000027/1] and a [National Institute for Health Research][nihr] Biomedical Research Centre [UCLH][uclh]/UCL High Impact Initiative.


[tig]: http://cmictig.cs.ucl.ac.uk
[giftsurg]: http://www.gift-surg.ac.uk
[cmic]: http://cmic.cs.ucl.ac.uk
[ucl]: http://www.ucl.ac.uk
[nihr]: http://www.nihr.ac.uk/research
[uclh]: http://www.uclh.nhs.uk
[epsrc]: http://www.epsrc.ac.uk
[wellcometrust]: http://www.wellcome.ac.uk
