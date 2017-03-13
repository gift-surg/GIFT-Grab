# What is available in Python

Most of the C++ public API has been exposed to Python.
In particular the following classes are available and are of interest to Python API users:
* `VideoSourceFactory`
* `VideoTargetFactory`
* `IObservable` except for `notify()`
* `IObserver`
* `VideoFrame` except for `manages_own_data()`


# What is NOT available or different in Python

* Copy constructors are not available.
* `VideoFrame::data()` is exposed only when installing GIFT-Grab with NumPy support. This function can be used to access video data as NumPy arrays.
* C++ exceptions are mapped to Python built-in exceptions.
  * The following GIFT-Grab exceptions map to Python [`RuntimeError`](https://docs.python.org/2/library/exceptions.html#exceptions.RuntimeError):
     * `BasicException`
     * `VideoSourceError`
     * `VideoTargetError`
     * `ObserverError`
  * The following GIFT-Grab exceptions map to Python [`IOError`](https://docs.python.org/2/library/exceptions.html#exceptions.IOError):
     * `DeviceAlreadyConnected`
     * `DeviceNotFound`
     * `DeviceOffline`
