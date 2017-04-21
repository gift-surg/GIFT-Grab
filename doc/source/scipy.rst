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
To achieve this, it extends the ``IObservableObserver`` class exposed by GIFT-Grab. ::

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
