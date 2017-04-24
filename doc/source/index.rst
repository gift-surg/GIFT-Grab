GIFT-Grab
=========

This site only contains Python examples for getting started with GIFT-Grab.
The full API documentation is available on CodeDocs_.
For further details, please also see `GIFT-Grab on GitHub`_.

.. _CodeDocs: https://codedocs.xyz/gift-surg/GIFT-Grab/
.. _`GIFT-Grab on GitHub`: https://github.com/gift-surg/GIFT-Grab


Python examples
^^^^^^^^^^^^^^^

**Important:** to run each example below, GIFT-Grab must have been installed with the corresponding feature enabled.
All features are disabled by default and can be enabled via `pip install options`_.
In summary this is done by appending the corresponding install options **when calling** ``pip``, e.g.: ::

   pip install gift-grab --install-option="--feature-1" --install-option="--feature-2"

.. _`pip install options`: https://pip.pypa.io/en/stable/reference/pip_install/#cmdoption-install-option

The relevant install options for each example below are outlined on the respective page.
Multiple install options can be specified, provided that these are not mutually exclusive.
Mutually exclusive features are noted in the documentation on GitHub.

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
