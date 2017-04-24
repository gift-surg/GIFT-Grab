.. _Files:

Reading video files
===================

**Important:** Please see `this page`_ for the **pre-requisites** and ``pip`` **install options** needed to enable this feature.

.. _`this page`: https://github.com/gift-surg/GIFT-Grab/blob/master/doc/pypi.md#reading-video-files

To read a video file, we first obtain the `GIFT-Grab video source factory`_ singleton: ::

    from pygiftgrab import VideoSourceFactory
    fac = VideoSourceFactory.get_instance()

.. _`GIFT-Grab video source factory`: https://codedocs.xyz/gift-surg/GIFT-Grab/classgg_1_1_video_source_factory.html#details

Then we `create a file reader`_ using the obtained factory: ::

    from pygiftgrab import ColourSpace
    file_reader = fac.create_file_reader( '/tmp/myinput.mp4', ColourSpace.I420 )

.. _`create a file reader`: https://codedocs.xyz/gift-surg/GIFT-Grab/classgg_1_1_video_source_factory.html#a7de4df06b2b210085cfe3f3027e36dd7

The second argument above specifies that we would like to get the video frames in the I420_ `colour space`_.
The ``file_reader`` object automatically starts reading the frames in ``'/tmp/myinput.mp4'`` one by one, at the file's frame rate.

To process the frames, we need to `attach`_ an `observer`_ to our ``file_reader``, which is already an `observable`_ object.
You can see how to do this in one of the following examples:

* :ref:`Encoding` shows how to save video frames to a file.
* :ref:`SciPy` shows how to implement a custom video frame processing capability in your application.

.. _I420: https://wiki.videolan.org/YUV/#YUV_4:2:0_.28I420.2FJ420.2FYV12.29
.. _`colour space`: https://codedocs.xyz/gift-surg/GIFT-Grab/namespacegg.html#a4f52bacf224413c522da5fb3c89dde6b
.. _`observer`: https://codedocs.xyz/gift-surg/GIFT-Grab/classgg_1_1_i_observer.html
.. _`observable`: https://codedocs.xyz/gift-surg/GIFT-Grab/classgg_1_1_i_observer.html
.. _`attach`: https://codedocs.xyz/gift-surg/GIFT-Grab/classgg_1_1_i_observable.html#a38b52081b221dc476aa9c2ba32774a2d
