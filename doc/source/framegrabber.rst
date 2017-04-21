.. _FrameGrabber:

Video acquisition using frame-grabber hardware
==============================================

For the relevant install option(s) and dependencies, please see the corresponding link from the list below to activate support for the corresponding frame-grabber hardware:

* `Epiphan DVI2PCIe Duo`_
* `Blackmagic DeckLink SDI 4K`_

.. _`Epiphan DVI2PCIe Duo`: https://github.com/gift-surg/GIFT-Grab/blob/master/doc/pypi.md#epiphan-dvi2pcie-duo
.. _`Blackmagic DeckLink SDI 4K`: https://github.com/gift-surg/GIFT-Grab/blob/master/doc/pypi.md#blackmagic-decklink-sdi-4k

In this example we are going to use an Epiphan DVI2PCIe Duo.
First we get the `GIFT-Grab video source factory`_ singleton: ::

    from pygiftgrab import VideoSourceFactory
    fac = VideoSourceFactory.get_instance()

.. _`GIFT-Grab video source factory`: https://codedocs.xyz/gift-surg/GIFT-Grab/classgg_1_1_video_source_factory.html

Then we `request the device`_ using the factory: ::

    epiphan = fac.get_device(
        Device.DVI2PCIeDuo_DVI, ColourSpace.I420 )

.. _`request the device`: https://codedocs.xyz/gift-surg/GIFT-Grab/classgg_1_1_video_source_factory.html#af46e23354df7483fab9860ce8c954d16

The first argument above specifies the `device type`_.
The second argument specifies that we would like to get the video frames in the I420_ `colour space`_.

.. _`device type`: https://codedocs.xyz/gift-surg/GIFT-Grab/namespacegg.html#a934637dd9e1d14db07268beeaa9b1fcb
.. _I420: https://wiki.videolan.org/YUV/#YUV_4:2:0_.28I420.2FJ420.2FYV12.29
.. _`colour space`: https://codedocs.xyz/gift-surg/GIFT-Grab/namespacegg.html#a4f52bacf224413c522da5fb3c89dde6b

For processing video frames obtained from the ``epiphan`` object, see the discussion at the end of the :ref:`Files` section.
