.. _FrameGrabber:

Video acquisition using frame-grabber hardware
==============================================

How to enable this feature
^^^^^^^^^^^^^^^^^^^^^^^^^^

Please see the links below for the **pre-requisites** and ``pip`` **install options** needed to enable support for the corresponding frame-grabber hardware:

* `Epiphan DVI2PCIe Duo`_
* `Blackmagic DeckLink SDI 4K`_

.. _`Epiphan DVI2PCIe Duo`: https://github.com/gift-surg/GIFT-Grab/blob/master/doc/pypi.md#epiphan-dvi2pcie-duo
.. _`Blackmagic DeckLink SDI 4K`: https://github.com/gift-surg/GIFT-Grab/blob/master/doc/pypi.md#blackmagic-decklink-sdi-4k

Example with explanation
^^^^^^^^^^^^^^^^^^^^^^^^

In this example we will see how we can capture the video stream from an external device using an Epiphan DVI2PCIe Duo.
First we obtain the `GIFT-Grab video source factory`_ singleton: ::

    from pygiftgrab import VideoSourceFactory
    sfac = VideoSourceFactory.get_instance()

.. _`GIFT-Grab video source factory`: https://codedocs.xyz/gift-surg/GIFT-Grab/classgg_1_1_video_source_factory.html

Then we `request the device`_ using the obtained factory: ::

    from pygiftgrab import Device, ColourSpace
    epiphan = sfac.get_device(
        Device.DVI2PCIeDuo_DVI, ColourSpace.I420 )

.. _`request the device`: https://codedocs.xyz/gift-surg/GIFT-Grab/classgg_1_1_video_source_factory.html#af46e23354df7483fab9860ce8c954d16

The first argument above specifies the `device type`_.
In this particular case ``Device.DVI2PCIeDuo_DVI`` indicates the DVI port of an Epiphan DVI2PCIe Duo.
The second argument specifies that we would like to get the video frames in the I420_ `colour space`_.

.. _`device type`: https://codedocs.xyz/gift-surg/GIFT-Grab/namespacegg.html#a934637dd9e1d14db07268beeaa9b1fcb
.. _I420: https://wiki.videolan.org/YUV/#YUV_4:2:0_.28I420.2FJ420.2FYV12.29
.. _`colour space`: https://codedocs.xyz/gift-surg/GIFT-Grab/namespacegg.html#a4f52bacf224413c522da5fb3c89dde6b

For processing video frames obtained from the ``epiphan`` object, please see the :ref:`ProcessingFrames` section.
Within all the source code pertaining to that discussion, you can simply replace all occurences of ``file_reader`` with the ``epiphan`` object we've created above.
The polymorphism achieved by returning a `GIFT-Grab video source`_ from the `GIFT-Grab video source factory`_ methods ``get_device()`` and ``create_file_reader()`` allows video consumers to be agnostic of the actual type of the `GIFT-Grab video source`_ object.

.. _`GIFT-Grab video source`: https://codedocs.xyz/gift-surg/GIFT-Grab/class_i_video_source.html
