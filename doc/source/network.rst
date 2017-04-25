.. _Network:

Capturing a network video stream
================================

How to enable this feature
^^^^^^^^^^^^^^^^^^^^^^^^^^

Please see `this page`_ for the **pre-requisites** and ``pip`` **install options** needed to enable this feature.

.. _`this page`: https://github.com/gift-surg/GIFT-Grab/blob/master/doc/pypi.md#network-streams

Example with explanation
^^^^^^^^^^^^^^^^^^^^^^^^

To start capturing a network video stream, we first obtain the `GIFT-Grab video source factory`_ singleton: ::

    from pygiftgrab import VideoSourceFactory
    sfac = VideoSourceFactory.get_instance()

.. _`GIFT-Grab video source factory`: https://codedocs.xyz/gift-surg/GIFT-Grab/classgg_1_1_video_source_factory.html

Then we `request a connection`_ to our network source using the obtained factory: ::

    from pygiftgrab import ColourSpace
    source_mrl = 'rtsp://192.168.127.191:555/stream.sdp'
    network_stream = sfac.connect_network_source(
        source_mrl, ColourSpace.BGRA )

.. _`request a connection`: https://codedocs.xyz/gift-surg/GIFT-Grab/classgg_1_1_video_source_factory.html#a0d39bea6386593ac962e268b7325ce92

The first argument above specifies the Media Resource Locator (MRL_) of our network source (the provided link explains the format of this string).
If you're using a device capable of streaming to the network such as an Epiphan Pearl, you can find out its MRL_ by looking it up in the user's manual and/or on the admin interface of your device.
The second argument specifies that we would like to get the video frames in the BGRA `colour space`_.

.. _MRL: https://wiki.videolan.org/Media_resource_locator/
.. _`colour space`: https://codedocs.xyz/gift-surg/GIFT-Grab/namespacegg.html#a4f52bacf224413c522da5fb3c89dde6b

For processing video frames obtained from the ``network_stream`` object, please see the :ref:`ProcessingFrames` section.
Within all the source code pertaining to that discussion, you can simply replace all occurences of ``file_reader`` with the ``network_stream`` object we've created above.
The polymorphism achieved by returning a `GIFT-Grab video source`_ from the `GIFT-Grab video source factory`_ methods ``connect_network_source()`` and ``create_file_reader()`` allows video consumers to be agnostic of the actual type of the `GIFT-Grab video source`_ object.

.. _`GIFT-Grab video source`: https://codedocs.xyz/gift-surg/GIFT-Grab/class_i_video_source.html
