from setuptools import setup, Extension
from setuptools.command.install import install
from setuptools.command.build_ext import build_ext
from os import environ
from giftgrab_configurator import Builder, Features
import sys

# TODO: error if c++ doesn't exist
environ["CC"] = "c++"
environ["CXX"] = "c++"

features = Features()
builder = Builder(features)

libgiftgrab = Extension(
    name=builder.lib_name(),
    sources=builder.lib_sources()
    )

pygiftgrab = Extension(
    name=builder.py_name(),
    sources=builder.py_sources()
    )

class GiftGrabBuildExtCommand(build_ext):
    def run(self):
        global libgiftgrab, pygiftgrab, builder
        for e in self.extensions:
            if e is libgiftgrab:
                e.include_dirs=builder.lib_include_dirs()
                e.extra_compile_args=builder.lib_compile_args()
                e.extra_link_args=builder.lib_link_args()
            elif e is pygiftgrab:
                e.include_dirs=builder.py_include_dirs()
                e.extra_compile_args=builder.py_compile_args()
                e.extra_link_args=builder.py_link_args()
                e.libraries=builder.py_libraries()
                e.library_dirs=builder.py_library_dirs()
                e.runtime_library_dirs=builder.py_runtime_library_dirs()
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
    version='16.08.11rc1',
    ext_modules=[libgiftgrab, pygiftgrab],
    cmdclass={
        'install': GiftGrabInstallCommand,
        'build_ext': GiftGrabBuildExtCommand,
    },
    )
