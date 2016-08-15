from setuptools import setup, Extension
from setuptools.command.install import install
from setuptools.command.build_ext import build_ext
from os import environ, mkdir, chdir, getcwd, path, listdir
from build_config import BuildOptions, Features

from subprocess import check_output

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

    """This class customises the GiftGrab C++ and Python library
    based on the user-input feature list.

    """

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

    """This class parses the user-input feature requests.

    """

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



        # TODO: error if c++ doesn't exist

        # check whether CMake installed
        try:
            output_buffer = check_output(['cmake'])
        except:
            print('CMake does not seem to be installed on your system.\n\n')
            print('CMake is needed to build GiftGrab.')
            raise

        build_dir = '_buildinglskdjf'
        here = path.abspath(path.dirname(__file__))
        mkdir(build_dir)
        chdir(build_dir)

        # check OpenCV
        if (features.epiphan_dvi2pcie_duo and not features.i420) \
           or features.xvid:
            try:
                output_buffer = check_output(['cmake', path.join(here, 'cmake/opencv')])
                # raise OSError(output_buffer)
            except:
                print('OpenCV does not seem to be installed on your system.')
                print('OpenCV is needed for Xvid and Epiphan DVI2PCIe Duo (without EpiphanSDK) support.\n\n')
                raise

        install.run(self)


# TODO: pip python dependencies (e.g. py.test)

# TODO: resources (e.g. sources, headers, include dirs,
# etc. based on selected features)



# print '>>>>> %s' % (sys.argv)


# TODO: BUILD_TESTS


setup(
    name='giftgrab',
    version='16.08.15rc1',
    cmdclass={
        'install': GiftGrabInstallCommand,
    },
)
