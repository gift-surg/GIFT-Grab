.. _Encoding:

Encoding video streams in real time
===================================

How to enable this feature
^^^^^^^^^^^^^^^^^^^^^^^^^^

Please see `this page`_ for the **pre-requisites** and ``pip`` **install options** needed to enable this feature.

.. _`this page`: https://github.com/gift-surg/GIFT-Grab/blob/master/doc/pypi.md#writing-video-files

Exemplary source code with explanation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This section shows how GIFT-Grab can be used for encoding video frames and saving them to a file.
To this end, we first obtain the `GIFT-Grab video target factory`_ singleton: ::

    from pygiftgrab import VideoTargetFactory
    tfac = VideoTargetFactory.get_instance()

.. _`GIFT-Grab video target factory`: https://codedocs.xyz/gift-surg/GIFT-Grab/classgg_1_1_video_target_factory.html#details

Then we `create a file writer`_ using the factory: ::

    from pygiftgrab import Codec
    frame_rate = 30.0
    file_writer = tfac.create_file_writer(
        Codec.HEVC, '/tmp/myoutput.mp4', frame_rate )

.. _`create a file writer`: https://codedocs.xyz/gift-surg/GIFT-Grab/classgg_1_1_video_target_factory.html#adacfc7db06b4a0fa924b269a9a22af08

The first argument in the call to ``create_file_writer()`` specifies the `codec to use for encoding`_.
The third argument is the frame rate of the resulting ``/tmp/myoutput.mp4`` file.
The ``frame_rate`` could be assigned based on the frame rate of the `video source`_ that feeds frames to our ``file_writer``.
For instance, if we were using the ``file_reader`` created in the :ref:`Files` section, we could `query its frame rate`_: ::

    frame_rate = file_reader.get_frame_rate()

.. _`query its frame rate`: https://codedocs.xyz/gift-surg/GIFT-Grab/class_i_video_source.html#a44096924cbcb7af81285a2ab6dd38f7f
.. _`codec to use for encoding`: https://codedocs.xyz/gift-surg/GIFT-Grab/namespacegg.html#a0d9545742253419d218d627ead459d3b

Once we `attach`_ this ``file_writer`` to a `video source`_, e.g. our ``frame_reader``, it starts receiving `video frames`_, each of which it subsequently encodes using HEVC_ and writes to ``/tmp/myoutput.mp4``: ::

    file_reader.attach( file_writer )

.. _attach: https://codedocs.xyz/gift-surg/GIFT-Grab/classgg_1_1_i_observable.html#a38b52081b221dc476aa9c2ba32774a2d
.. _`video source`: https://codedocs.xyz/gift-surg/GIFT-Grab/class_i_video_source.html
.. _`video frames`: https://codedocs.xyz/gift-surg/GIFT-Grab/classgg_1_1_video_frame.html
.. _HEVC: https://github.com/gift-surg/GIFT-Grab/blob/master/doc/pypi.md#hevc

This pipeline keeps operating until we `detach`_ our ``file_writer`` from our ``file_reader``: ::

    file_reader.detach( file_writer )

.. _detach: https://codedocs.xyz/gift-surg/GIFT-Grab/classgg_1_1_i_observable.html#ada3f3062b7cd3fd5845dbef9d604ff5b

The :ref:`SciPy` section shows a more complex pipeline that implements a custom image processing capability.

Full source code
^^^^^^^^^^^^^^^^

::

    #!/usr/bin/env python2

    from pygiftgrab import VideoTargetFactory
    from pygiftgrab import Codec
    from time import sleep
    from pygiftgrab import VideoSourceFactory
    from pygiftgrab import ColourSpace


    if __name__ == '__main__':
        sfac = VideoSourceFactory.get_instance()
        file_reader = sfac.create_file_reader(
            '/tmp/myinput.mp4', ColourSpace.I420 )

        tfac = VideoTargetFactory.get_instance()
        frame_rate = file_reader.get_frame_rate()
        file_writer = tfac.create_file_writer(
            Codec.HEVC, '/tmp/myoutput.mp4', frame_rate )

        file_reader.attach( file_writer )

        sleep( 20 )

        file_reader.detach( file_writer )
