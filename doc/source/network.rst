.. _Network:

Capturing a network video stream
================================

**Important:** Please see `this page`_ for the **pre-requisites** and ``pip`` **install options** needed to enable this feature.

.. _`this page`: https://github.com/gift-surg/GIFT-Grab/blob/master/doc/pypi.md#network-streams

To start capturing a network video stream, we first obtain the `GIFT-Grab video source factory`_ singleton: ::

    from pygiftgrab import VideoSourceFactory
    sfac = VideoSourceFactory.get_instance()

.. _`GIFT-Grab video source factory`: https://codedocs.xyz/gift-surg/GIFT-Grab/classgg_1_1_video_source_factory.html

Then we `request a connection`_ to our network source using the obtained factory: ::

    from pygiftgrab import ColourSpace
    source_mrl = 'rtsp://192.168.127.191:555/stream.sdp'
    network_stream = sfac.connect_network_source(
        source_mrl, ColourSpace.I420 )

.. _`request a connection`: https://codedocs.xyz/gift-surg/GIFT-Grab/classgg_1_1_video_source_factory.html#a0d39bea6386593ac962e268b7325ce92

The first argument above specifies the Media Resource Locator (MRL_) of our network source (the provided link explains the format of this string).
To find out what your device's MRL_ is, you will need to look it up in the user's manual of your device.
The second argument specifies that we would like to get the video frames in the I420_ `colour space`_.

.. _MRL: https://wiki.videolan.org/Media_resource_locator/
.. _I420: https://wiki.videolan.org/YUV/#YUV_4:2:0_.28I420.2FJ420.2FYV12.29
.. _`colour space`: https://codedocs.xyz/gift-surg/GIFT-Grab/namespacegg.html#a4f52bacf224413c522da5fb3c89dde6b

For processing video frames obtained from the ``network_stream`` object, please see the discussion at the end of the :ref:`Files` section, which talks about attaching an observer object to the video source there, i.e. the ``file_reader``.
Within all the code pertaining to that discussion, you can simply replace all occurences of ``file_reader`` with the ``network_stream`` object we've created above.
The polymorphism achieved by returning a `GIFT-Grab video source`_ from the `GIFT-Grab video source factory`_ methods ``connect_network_source()`` and ``create_file_reader()`` which allows video consumers to be agnostic of the actual type of the `GIFT-Grab video source`_ object.

.. _`GIFT-Grab video source`: https://codedocs.xyz/gift-surg/GIFT-Grab/class_i_video_source.html
