from setuptools import setup
from distutils.errors import LibError
from setuptools.command.install import install
from os import mkdir, chdir, path, listdir
from subprocess import check_output

class GiftGrabInstallCommand(install):

    """This class builds GiftGrab components based on
    requested features from user.

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
