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
  * The following exceptions map to `RuntimeError`:
     * `BasicException`
     * `VideoSourceError`
     * `VideoTargetError`
     * `ObserverError`
  * The following exceptions map to `IOError`:
     * `DeviceAlreadyConnected`
     * `DeviceNotFound`
     * `DeviceOffline`