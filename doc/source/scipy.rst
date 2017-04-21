.. _SciPy:

Processing video frames with SciPy / NumPy
==========================================

For the relevant install option(s) and dependencies, please see `this page`_ to activate this feature.

.. _`this page`: https://github.com/gift-surg/GIFT-Grab/blob/master/doc/pypi.md#numpy

In this section we define a ``GaussianSmootherBGRA`` class whose instances act as video processing nodes.

The ``GaussianSmootherBGRA`` class achieves this by being an observer_ and observable_ simultaneously.
In other words, an instance of this class can be attached to an observable_ video source object.
This causes that video source object to pass each new `video frame`_ to the ``GaussianSmootherBGRA`` instance.
The ``GaussianSmootherBGRA`` instance in return processes that `video frame`_ and passes it to any observer_ object attached to itself.
To this end, it extends the ``IObservableObserver`` class exposed by GIFT-Grab. ::

    from pygiftgrab import IObservableObserver
    import scipy.ndimage as ndimage

    class GaussianSmootherBGRA(IObservableObserver):

        def __init__(self):
            super(GaussianSmootherBGRA, self).__init__()

        def update(self, frame):
            data_np = frame.data(True)
            ndimage.gaussian_filter(input=data_np,
                                    sigma=(5, 15, 0),
                                    order=0, output=data_np)

.. _observer: https://codedocs.xyz/gift-surg/GIFT-Grab/classgg_1_1_i_observer.html#details
.. _observable: https://codedocs.xyz/gift-surg/GIFT-Grab/classgg_1_1_i_observable.html#details

The ``update()`` method above overrides the `corresponding method of GIFT-Grab's observer`_.
It simply obtains a reference to the actual data of the `video frame`_ in the form of a `NumPy array`_ by calling the `data method`_.
It then applies Gaussian filtering `in-place` on this NumPy array using the `corresponding method of SciPy`_.

.. _`corresponding method of GIFT-Grab's observer`: https://codedocs.xyz/gift-surg/GIFT-Grab/classgg_1_1_i_observer.html#a3402ba495e36d0d40db549b2057c6335
.. _`video frame`: https://codedocs.xyz/gift-surg/GIFT-Grab/classgg_1_1_video_frame.html
.. _`NumPy array`: https://docs.scipy.org/doc/numpy/user/quickstart.html
.. _`data method`: https://codedocs.xyz/gift-surg/GIFT-Grab/classgg_1_1_video_frame.html#a458e15b00b5b2d39855db76215c44055
.. _`corresponding method of SciPy`: https://docs.scipy.org/doc/scipy/reference/generated/scipy.ndimage.gaussian_filter.html#scipy.ndimage.gaussian_filter

As a concrete example, consider attaching a ``GaussianSmootherBGRA`` object to a video source.
We could for instance use the ``file_reader`` in the :ref:`Files` section.
There is a subtlety, though: we need to initialise this ``file_reader`` using the BGRA colour space instead of I420 as in the :ref:`Files` section: ::

    file_reader = fac.create_file_reader( '/tmp/myvideo.mp4', ColourSpace.BGRA )

Note that we use ``ColourSpace.BGRA`` instead of ``ColourSpace.I420``, which is the only difference to the corresponding line in the :ref:`Files` section.
This is because our ``GaussianSmootherBGRA`` class expects the video frames to be encoded in the BGRA colour space.
Then we can create our ``gauss`` object and attach it to the ``file_reader``: ::

    gauss = GaussianSmootherBGRA()
    file_reader.attach( gauss )

Now the ``file_reader`` has started feeding video frames to ``gauss``.
We can attach the ``file_writer`` in the :ref:`Encoding` section to ``gauss`` as well: ::

    gauss.attach( file_writer )

Now ``gauss`` has started feeding the Gaussian-smoothed video frames to the ``file_writer``.
In other words, each video frame read from our video file is Gaussian-smoothed and subsequently encoded to a new video file.
This pipeline keeps operating until we detach_ the observers from the observables: ::

    file_reader.detach( gauss )
    gauss.detach( file_writer )

.. _detach: https://codedocs.xyz/gift-surg/GIFT-Grab/classgg_1_1_i_observable.html#ada3f3062b7cd3fd5845dbef9d604ff5b
