.. _Network:

Capturing a network video stream
================================

For the relevant install option(s) and dependencies, please see `this page`_ to activate this feature.

.. _`this page`: https://github.com/gift-surg/GIFT-Grab/blob/master/doc/pypi.md#network-streams

First we get the `GIFT-Grab video source factory`_ singleton: ::

    from pygiftgrab import VideoSourceFactory
    fac = VideoSourceFactory.get_instance()

.. _`GIFT-Grab video source factory`: https://codedocs.xyz/gift-surg/GIFT-Grab/classgg_1_1_video_source_factory.html

Then we `request a connection`_ to our network source using the factory: ::

    from pygiftgrab import ColourSpace
    network_stream = fac.connect_network_source(
        source_mrl, ColourSpace.I420 )

.. _`request a connection`: https://codedocs.xyz/gift-surg/GIFT-Grab/classgg_1_1_video_source_factory.html#a0d39bea6386593ac962e268b7325ce92

The first argument above specifies the MRL_ of our network source.
The second argument specifies that we would like to get the video frames in the I420_ `colour space`_.

.. _MRL: https://wiki.videolan.org/Media_resource_locator/
.. _I420: https://wiki.videolan.org/YUV/#YUV_4:2:0_.28I420.2FJ420.2FYV12.29
.. _`colour space`: https://codedocs.xyz/gift-surg/GIFT-Grab/namespacegg.html#a4f52bacf224413c522da5fb3c89dde6b

For processing video frames obtained from the ``network_stream``, see the discussion at the end of the :ref:`Files` section.
