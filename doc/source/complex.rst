.. _Complex:

Multi-threaded complex processing pipelines
===========================================

The example below shows how GIFT-Grab can be used for running complex pipelines with multiple
intermediate processing nodes and threads.
The intermediate processing nodes in this example are built on the same principles as in the
:ref:`SciPy` example.
Running the example requires an `HEVC-encoded MP4 file`_, an `NVENC-capable GPU`_, and `NumPy support`_.

.. _`HEVC-encoded MP4 file`: https://github.com/gift-surg/GIFT-Grab/blob/master/doc/build.md#reading-video-files
.. _`NVENC-capable GPU`: https://github.com/gift-surg/GIFT-Grab/blob/master/doc/build.md#hevc
.. _`NumPy support`: https://github.com/gift-surg/GIFT-Grab/blob/master/doc/build.md#python-api

The full source code of the example is below.
Please follow the comments and the flow of code.
This example is also available on GitHub_:

.. literalinclude:: ../../src/tests/pipeline/complex_pipeline.py
   :language: python

.. _GitHub: https://github.com/gift-surg/GIFT-Grab/blob/master/src/tests/pipeline/complex_pipeline.py
