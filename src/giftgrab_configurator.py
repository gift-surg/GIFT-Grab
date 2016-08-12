from site import getsitepackages
from sys import version_info
from os.path import join
from sysconfig import get_platform


class Features:

    """This class represents GiftGrab features
    as can be configured on building from source.

    """

    def __init__(self):
        """Switch all features off.

        """
        self.epiphan_dvi2pcie_duo = False
        self.i420 = False
        self.xvid = False
        self.h265 = False
        self.nvenc = False


    def __repr__(self):
        """Return a string representation.

        @return
        """
        s = ''
        if self.epiphan_dvi2pcie_duo: s += 'Epiphan DVI2PCIe Duo, '
        if self.i420: s += 'I420, '
        if self.xvid: s += 'Xvid, '
        if self.h265: s += 'H265, '
        if self.nvenc: s += 'NVENC'
        return s


class Builder:
    def __init__(self, features):
        """Initialise build parameters based on
        passed `features`.

        @param features arguments as passed by user
        when invoking ``pip install``
        """
        # self.epiphan_dvi2pcie_duo = '--epiphan-dvi2pcie-duo' in features
        # self.xvid = '--xvid' in features
        # self.h265 = '--h265' in features
        # self.nvenc = '--nvenc' in features

        # TODO
        # TODO: USE_EPIPHAN_DVI2PCIE_DUO

        # TODO: USE_XVID

        # TODO: USE_H265

        # TODO: USE_NVENC

        # TODO: BUILD_TESTS
        self.name = 'giftgrab'
        self.features = features


    def lib_name(self):
        """Get name of library file.

        """
        return 'lib%s' % (self.name)


    def lib_sources(self):
        """Determine needed source files for building
        C++ library.

        """
        # TODO
        return ['api/videoframe.cpp',
                'api/maskframe.cpp',
                'api/factory.cpp',
                'api/except.cpp']


    def lib_include_dirs(self):
        """Determine needed include dirs for building
        C++ library.

        """
        # TODO
        return ['api', 'utils']


    def lib_compile_args(self):
        """Get compiler arguments for building C++
        library.

        """
        # TODO
        return ['-std=c++11', '-O3', '-DNDEBUG', '-fPIC']


    def lib_link_args(self):
        """Get linker args for building C++ library.

        """
        # TODO
        return ['-fPIC', '-std=c++11', '-O3', '-DNDEBUG', '-shared']
        # TODO: all flags, e.g. -Wl


    def lib_libraries(self):
        """Determine libraries to link against for
        building C++ library.

        """
        # TODO
        pass


    def lib_library_dirs(self):
        """Determine locations of libraries to link
        against for building C++ library.

        """
        # TODO
        pass


    def lib_runtime_library_dirs(self):
        """Determine runtime library path for C++
        library.

        """
        # TODO
        pass

    def py_name(self):
        """Get name for Python library file.

        """
        return 'py%s' % (self.name)


    def py_sources(self):
        """Determine needed source files for building
        Python library.

        """
        # TODO
        return ['python/wrapper.cpp']


    def py_include_dirs(self):
        """Determine needed include dirs for building
        Python library.

        """
        # TODO
        py_dirs = ['/usr/include/python%d.%d' % (version_info.major, version_info.minor)]
        lib_dirs = self.lib_include_dirs()
        return lib_dirs + py_dirs
                



    def py_compile_args(self):
        """Get compiler arguments for building Python
        library.

        """
        # TODO
        return self.lib_compile_args()


    def py_link_args(self):
        """Get linker args for building Python library.

        """
        # TODO
        return self.lib_link_args()


    def py_libraries(self):
        """Determine libraries to link against for
        building Python library.

        """
        # TODO
        return ['boost_python', self.name]


    def py_library_dirs(self):
        """Determine locations of libraries to link
        against for building Python library.

        """
        # TODO
        lib_dir = 'lib.%s-%d.%d' % (get_platform(),
                                    version_info.major,
                                    version_info.minor)
        return [join('build', lib_dir)]


    def py_runtime_library_dirs(self):
        """Determine runtime library path for Python
        library.

        """
        # TODO
        return getsitepackages()
