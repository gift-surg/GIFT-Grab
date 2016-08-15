from setuptools import setup, Extension
from setuptools.command.install import install
from setuptools.command.build_ext import build_ext
from os import environ
from build_config import BuildOptions, Features
import sys

# TODO: error if c++ doesn't exist
environ["CC"] = "c++"
environ["CXX"] = "c++"

features = Features()
build_opts = BuildOptions(features)

libgiftgrab = Extension(
    name=build_opts.lib_name(),
    sources=build_opts.lib_sources()
    )

pygiftgrab = Extension(
    name=build_opts.py_name(),
    sources=build_opts.py_sources()
    )

class GiftGrabBuildExtCommand(build_ext):
    def run(self):
        global libgiftgrab, pygiftgrab, build_opts

        libgiftgrab.include_dirs = build_opts.lib_include_dirs()
        libgiftgrab.extra_compile_args = build_opts.lib_compile_args()
        libgiftgrab.extra_link_args = build_opts.lib_link_args()

        pygiftgrab.include_dirs = build_opts.py_include_dirs()
        pygiftgrab.extra_compile_args = build_opts.py_compile_args()
        pygiftgrab.extra_link_args = build_opts.py_link_args()
        pygiftgrab.libraries = build_opts.py_libraries()
        pygiftgrab.library_dirs = build_opts.py_library_dirs()
        pygiftgrab.runtime_library_dirs = build_opts.py_runtime_library_dirs()

        build_ext.run(self)

class GiftGrabInstallCommand(install):
    user_options = install.user_options + [
        ('epiphan-dvi2pcie-duo', None, None),
        ('i420', None, None),
        ('xvid', None, None),
        ('h265', None, None),
    ]

    def initialize_options(self):
        install.initialize_options(self)
        self.epiphan_dvi2pcie_duo = None
        self.i420 = None
        self.xvid = None
        self.h265 = None

    def finalize_options(self):
        install.finalize_options(self)

    def run(self):
        global features
        features.epiphan_dvi2pcie_duo = self.epiphan_dvi2pcie_duo
        features.i420 = self.i420
        features.xvid = self.xvid
        features.h265 = self.h265
        print '<<<<< IN features %s' % (str(features))
        install.run(self)


# TODO: pip python dependencies (e.g. py.test)

# TODO: resources (e.g. sources, headers, include dirs,
# etc. based on selected features)



# print '>>>>> %s' % (sys.argv)


# TODO: BUILD_TESTS


setup(
    name='giftgrab',
    version='16.08.15rc1',
    ext_modules=[libgiftgrab, pygiftgrab],
    cmdclass={
        'install': GiftGrabInstallCommand,
        'build_ext': GiftGrabBuildExtCommand,
    },
    )
