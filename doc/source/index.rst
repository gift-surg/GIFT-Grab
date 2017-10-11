GIFT-Grab
=========

GIFT-Grab is an open-source C++ and Python API for acquiring, processing and encoding video streams in real time.
GIFT-Grab supports several frame-grabber cards, standard-compliant network streams and video files.
The Python API is compatible with NumPy_ and SciPy_.
Please note that currently **only Python 2** is supported.

.. _SciPy: https://www.scipy.org/
.. _NumPy: http://www.numpy.org/

This site only contains Python examples for getting started with GIFT-Grab.
The full API documentation is available on CodeDocs_.
For further details including known issues, please also see `GIFT-Grab on GitHub`_.

.. _CodeDocs: https://codedocs.xyz/gift-surg/GIFT-Grab/
.. _`GIFT-Grab on GitHub`: https://github.com/gift-surg/GIFT-Grab


Important installation note
^^^^^^^^^^^^^^^^^^^^^^^^^^^

To be able to run each example below, GIFT-Grab must have been installed with the corresponding feature(s) enabled.
All features are disabled by default and can be enabled via `pip install options`_.
In summary this is done by appending the corresponding install options **when calling** ``pip``, e.g.: ::

   pip install gift-grab --install-option="--feature-1" --install-option="--feature-2"

.. _`pip install options`: https://pip.pypa.io/en/stable/reference/pip_install/#cmdoption-install-option

The relevant install options for each example below are outlined on the respective example page.
Multiple install options can be specified, provided that these are not mutually exclusive.
Mutually exclusive features are noted in the documentation on GitHub.

The examples were tested after installing GIFT-Grab with the install options directly following this paragraph.
If you would like to use this single command, please make sure all dependencies listed on the `GIFT-Grab Python API installation instructions`_ have been installed.
Alternatively you can install a "smaller" GIFT-Grab by using the install option(s) needed for the example(s) you'd like to run. ::

   pip install gift-grab --install-option="--files" --install-option="--epiphan-dvi2pcie-duo" --install-option="--enable-nonfree" --install-option="--epiphansdk" --install-option="--network-sources" --install-option="--numpy" --install-option="--hevc" --install-option="--enable-nonfree" --install-option="--nvenc"

.. _`GIFT-Grab Python API installation instructions`: https://github.com/gift-surg/GIFT-Grab/blob/master/doc/pypi.md#install-gift-grab-from-the-python-packaging-index

Examples
^^^^^^^^

.. toctree::
   :maxdepth: 2

   files
   framegrabber
   network
   scipy
   encoding

Citing GIFT-Grab
^^^^^^^^^^^^^^^^

If you use GIFT-Grab in your work, please cite `Shakir et al. (2017)`_:

  Shakir DI, García-Peraza-Herrera LC, Daga P, Doel T, Clarkson MJ, Ourselin S, and Vercauteren T.
  `GIFT-Grab: Real-time C++ and Python multi-channel video capture, processing and encoding API.`_
  Journal of Open Research Software. 2017;5(1):27. DOI: `http://doi.org/10.5334/jors.169`_

BibTeX entry:

.. code-block:: bibtex

  @article{giftgrab17,
    author = {Dzhoshkun Ismail Shakir and Luis Carlos Garc\'{i}a-Peraza-Herrera and Pankaj Daga and Tom Doel and Matthew J. Clarkson and S\'{e}bastien Ourselin and Tom Vercauteren},
    title = {{GIFT-Grab: Real-time C++ and Python multi-channel video capture, processing and encoding API}},
    journal = {{Journal of Open Research Software}},
    year = {2017},
    number = {1},
    pages = {27},
    month = {10},
    day = {9},
    volume = {5},
    url = {http://doi.org/10.5334/jors.169},
    doi = {http://doi.org/10.5334/jors.169},
  }

.. _`Shakir et al. (2017)`: http://doi.org/10.5334/jors.169
.. _`GIFT-Grab: Real-time C++ and Python multi-channel video capture, processing and encoding API.`: http://doi.org/10.5334/jors.169
.. _`http://doi.org/10.5334/jors.169`: http://doi.org/10.5334/jors.169

Useful links
^^^^^^^^^^^^

* :ref:`search`
