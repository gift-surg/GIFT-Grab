# GIFT-Grab

GIFT-Grab is an open-source C++ and Python API for acquiring, processing and encoding video streams in real time.

GIFT-Grab supports several frame-grabber cards, standard-compliant network streams and video files.

The Python API is compatible with [NumPy][numpy] and [SciPy][scipy].
Please note that currently **only Python 2** is supported.

[scipy]: https://www.scipy.org/
[numpy]: http://www.numpy.org/


## Features

* Capturing video streams using frame-grabber hardware, the following cards are supported:
   - [Epiphan DVI2PCIe Duo][e-dd]
   - [Blackmagic DeckLink SDI 4K][bm-dls4]
* Capturing standard-compliant network video streams
* Reading (decoding) video files
* Writing (encoding) video files (including real-time encoding), the following formats are supported:
   - [Xvid][xvid]-encoded [AVI][avi]
   - [HEVC (H.265)][hevc]-encoded [MP4][mp4]
   - [VP9][vp9]-encoded [WebM][webm]
* Exposing video data as NumPy arrays for processing with NumPy-compatible Python libraries


## Getting started

* Installation
   - [Required tools and libraries][gg-reqs]
   - [Building GIFT-Grab from source][build-from-source] (C++ and Python)
   - [Installing GIFT-Grab using `pip`][pip] (only Python)
* [Python examples with explanation][rtd]
* [Building a minimal GIFT-Grab with NumPy compatibility][gg-min]
* API documentation
   - [C++ API documentation](https://codedocs.xyz/gift-surg/GIFT-Grab/)
   - Python API documentation: please use the C++ API documentation
      * Most Python API elements are equivalent to their C++ counterparts.
      * Differences are noted in the C++ docstrings (e.g. [`data()` method of `VideoFrame`][vf-data]).
* [Known issues and limitations][gg-issues]


[gg-reqs]: doc/requirements.md
[gg-issues]: doc/issues.md
[gg-min]: doc/minimal.md
[rtd]: http://gift-grab.rtfd.io
[pip-install-ops]: https://pip.pypa.io/en/stable/reference/pip_install/#cmdoption-install-option
[vf-data]: https://codedocs.xyz/gift-surg/GIFT-Grab/classgg_1_1_video_frame.html#a458e15b00b5b2d39855db76215c44055
[build-from-source]: doc/build.md
[pip]: doc/pypi.md


[e-dd]: http://www.epiphan.com/products/dvi2pcie-duo/
[bm-dls4]: https://www.blackmagicdesign.com/products/decklink/models
[xvid]: https://www.xvid.com/
[avi]: https://msdn.microsoft.com/en-us/library/windows/desktop/dd318189(v=vs.85).aspx
[hevc]: http://www.itu.int/ITU-T/recommendations/rec.aspx?rec=11885
[mp4]: http://www.iso.org/iso/catalogue_detail.htm?csnumber=38538
[vp9]:http://www.webmproject.org/vp9/
[webm]: https://www.webmproject.org/users/


## Support and contributing

Please see the [contribution guide][cg] for bug reports, feature requests, and if you would like to contribute to GIFT-Grab.

[cg]: CONTRIBUTING.md

## Licensing and copyright

Copyright (c) 2015-7, [University College London][ucl]

GIFT-Grab is available as free open-source software under the BSD-3-Clause licence.
Please see the LICENSE file for details.

Other licences may apply for the GIFT-Grab dependencies.
Please see the [dependency installation guidelines][gg-tips] for the implications of using them with regards to licensing.

[gg-tips]: doc/tips.md


## Acknowledgements

GIFT-Grab was developed as part of the [GIFT-Surg][giftsurg] project at the [Translational Imaging Group][tig] in the [Centre for Medical Image Computing][cmic] at [University College London][ucl] (UCL).

This work was supported through an Innovative Engineering for Health award by the [Wellcome Trust][wellcometrust] [WT101957], the [Engineering and Physical Sciences Research Council][epsrc] [NS/A000027/1] and a [National Institute for Health Research][nihr] Biomedical Research Centre [UCLH][uclh] / UCL High Impact Initiative.


[tig]: http://cmictig.cs.ucl.ac.uk
[giftsurg]: http://www.gift-surg.ac.uk
[cmic]: http://cmic.cs.ucl.ac.uk
[ucl]: http://www.ucl.ac.uk
[nihr]: http://www.nihr.ac.uk/research
[uclh]: http://www.uclh.nhs.uk
[epsrc]: http://www.epsrc.ac.uk
[wellcometrust]: http://www.wellcome.ac.uk
