from setuptools import setup, Extension
from distutils.errors import LibError
from setuptools.command.install import install
from setuptools.command.build_ext import build_ext
from os import environ, mkdir, chdir, getcwd, path, listdir, rmdir
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
        ('nvenc', None, None),
    ]

    def initialize_options(self):
        install.initialize_options(self)
        self.epiphan_dvi2pcie_duo = None
        self.i420 = None
        self.xvid = None
        self.h265 = None
        self.nvenc = None

    def finalize_options(self):
        install.finalize_options(self)

    def run(self):
        self.epiphan_dvi2pcie_duo = self.epiphan_dvi2pcie_duo
        self.i420 = self.i420
        self.xvid = self.xvid
        self.h265 = self.h265
        self.nvenc = self.nvenc

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

        try:
            output_buffer = check_output(['cmake', path.join(here, 'cmake/cpp11')])
        except:
            print('Your compiler does not seem to support C++11.')
            print('A C++11 supporting compiler is needed to build GiftGrab.\n\n')
            raise

        chdir(here)
        build_dir += '-1'
        mkdir(build_dir)
        chdir(build_dir)

        # check platform supported
        try:
            output_buffer = check_output(['cmake', path.join(here, 'cmake/platform')])
        except:
            print('Your platform does not seem to be supported.\n\n')
            raise

        chdir(here)
        build_dir += '0'
        mkdir(build_dir)
        chdir(build_dir)

        # check OpenCV
        if (self.epiphan_dvi2pcie_duo and not self.i420) \
           or self.xvid:
            try:
                output_buffer = check_output(['cmake', path.join(here, 'cmake/opencv')])
                # raise OSError(output_buffer)
            except:
                print('OpenCV does not seem to be installed on your system.')
                print('OpenCV is needed for Xvid and Epiphan DVI2PCIe Duo (without EpiphanSDK) support.\n\n')
                raise

        chdir(here)
        build_dir += '1'
        mkdir(build_dir)
        chdir(build_dir)


        # check FFmpeg
        if self.h265 or self.nvenc:
            try:
                output_buffer = check_output(['cmake', path.join(here, 'cmake/ffmpeg')])
            except:
                print('FFmpeg does not seem to be installed on your system.')
                print('FFmpeg is needed for H265 support.')
                raise

            try:
                ffmpeg_buildconf = ['ffmpeg', '-buildconf']
                output_buffer = check_output(ffmpeg_buildconf)
            except:
                print('Failed to obtain your FFmpeg build configuration.')
                print('This command failed: %s %s\n\n' % tuple(ffmpeg_buildconf))
                raise

            if '--enable-muxer=mp4' not in output_buffer:
                print('Your FFmpeg does not seem to support MP4.')
                print('Please install FFmpeg with MP4 support (--enable-muxer=mp4).\n\n')
                raise LibError('FFmpeg not built with MP4 support')

            if self.nvenc and '--enable-nvenc' not in output_buffer:
                print('Your FFmpeg does not seem to support NVENC.')
                print('Please install FFmpeg with NVENC support (--enable-nvenc).\n\n')
                raise LibError('FFmpeg not built with NVENC support')

            if self.h265 and not self.nvenc and \
               '--enable-libx265' not in output_buffer:
                print('Your FFmpeg does not seem to support H265.')
                print('Please install FFmpeg with x265 support (--enable-libx265).\n\n')
                raise LibError('FFmpeg not built with x265 support')

        chdir(here)
        build_dir += '2'
        mkdir(build_dir)
        chdir(build_dir)

        # check EpiphanSDK
        if self.i420:
            try:
                output_buffer = check_output(['cmake', path.join(here, 'cmake/epiphansdk')])
            except:
                print('EpiphanSDK does not seem to be installed on your system.')
                print('EpiphanSDK is needed for I420 colour space support.')
                raise

        chdir(here)
        build_dir += 'REAL'
        mkdir(build_dir)
        chdir(build_dir)

        # moment of truth
        cmake_args = []
        cmake_args.append('-DBUILD_PYTHON=ON')
        cmake_args.append('-DBUILD_TESTS=ON')
        if self.epiphan_dvi2pcie_duo:
            cmake_args.append('-DUSE_EPIPHAN_DVI2PCIE_DUO=ON')
        if self.xvid:
            cmake_args.append('-DUSE_XVID=ON')
        if self.h265:
            cmake_args.append('-DUSE_H265=ON')
            if self.nvenc:
                cmake_args.append('-DUSE_NVENC=ON')
        cmake_cmds = ['cmake', here]
        cmake_cmds[1:1] = cmake_args
        try:
            output_buffer = check_output(cmake_cmds)
            output_buffer = check_output(['make', '-j'])
        except:
            print('There was an error (following below) when trying to build GiftGrab.\n\n')
            raise
        # raise RuntimeError(listdir('.'))

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
