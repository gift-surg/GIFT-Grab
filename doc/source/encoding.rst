.. _Encoding:

Encoding video streams in real time
===================================

For the relevant install option(s) and dependencies, please see `this page`_ to activate this feature.

.. _`this page`: https://github.com/gift-surg/GIFT-Grab/blob/master/doc/pypi.md#writing-video-files

This section shows how GIFT-Grab can be used for encoding video frames and saving them to a file.
To this end, we first get the `GIFT-Grab video target factory`_ singleton: ::

    from pygiftgrab import VideoTargetFactory
    fac = VideoTargetFactory.get_instance()

.. _`GIFT-Grab video target factory`: https://codedocs.xyz/gift-surg/GIFT-Grab/classgg_1_1_video_target_factory.html

Then we `create a file writer`_ using the factory: ::

    from pygiftgrab import Codec
    file_writer = fac.create_file_writer(
        Codec.HEVC, '/tmp/myoutput.mp4', frame_rate )


.. _`create a file writer`: https://codedocs.xyz/gift-surg/GIFT-Grab/classgg_1_1_video_target_factory.html#adacfc7db06b4a0fa924b269a9a22af08

The first argument above specifies the `codec to use for encoding`_.
The third argument is the frame rate of the resulting ``/tmp/myoutput.mp4`` file.
This could be assigned based on the video source's frame rate.
For instance, if we were using the ``file_reader`` created in the :ref:`Files` section, we could `query its frame rate`_: ::

    frame_rate = file_reader.get_frame_rate()

.. _`query its frame rate`: https://codedocs.xyz/gift-surg/GIFT-Grab/class_i_video_source.html#a44096924cbcb7af81285a2ab6dd38f7f
.. _`codec to use for encoding`: https://codedocs.xyz/gift-surg/GIFT-Grab/namespacegg.html#a0d9545742253419d218d627ead459d3b

Once this ``file_writer`` object gets attached_ to a `video source`_, it starts receiving `video frames`_, each of which it subsequently encodes using HEVC_ and writes to ``/tmp/myoutput.mp4``: ::

    file_reader.attach( file_writer )

.. _attached: https://codedocs.xyz/gift-surg/GIFT-Grab/classgg_1_1_i_observable.html#a38b52081b221dc476aa9c2ba32774a2d
.. _`video source`: https://codedocs.xyz/gift-surg/GIFT-Grab/class_i_video_source.html
.. _`video frames`: https://codedocs.xyz/gift-surg/GIFT-Grab/classgg_1_1_video_frame.html
.. _HEVC: https://github.com/gift-surg/GIFT-Grab/blob/master/doc/pypi.md#hevc

This pipeline keeps operating until the ``file_writer`` is detached_ from the ``file_reader``: ::

    file_reader.detach( file_writer )

.. _detached: https://codedocs.xyz/gift-surg/GIFT-Grab/classgg_1_1_i_observable.html#ada3f3062b7cd3fd5845dbef9d604ff5b

For a more complex pipeline, see the :ref:`SciPy` section.
