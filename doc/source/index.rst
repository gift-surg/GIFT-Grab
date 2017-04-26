GIFT-Grab
=========

This site only contains Python examples for getting started with GIFT-Grab.
The full API documentation is available on CodeDocs_.
For further details, please also see `GIFT-Grab on GitHub`_.

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

Useful links
^^^^^^^^^^^^

* :ref:`search`
