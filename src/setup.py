from setuptools import setup
from site import getsitepackages
from distutils.errors import LibError
from setuptools.command.install import install
from os import mkdir, chdir, listdir, getcwd, symlink, rename
from os.path import join, abspath, dirname
from subprocess import check_output


class GiftGrabInstallCommand(install):

    """This class builds GiftGrab components based on
    requested features from user.

    """

    user_options = install.user_options + \
                  [('epiphan-dvi2pcie-duo', None, None),
                   ('i420', None, None),
                   ('xvid', None, None),
                   ('h265', None, None),
                   ('nvenc', None, None)]


    def __repr__(self):
        """A string representation of the configuration.

        @return
        """
        str_rep = ''
        if self.epiphan_dvi2pcie_duo:
            str_rep += 'Epiphan DVI2PCIe Duo,'
        if self.i420:
            str_rep += ' I420,'
        if self.xvid:
            str_rep += ' Xvid,'
        if self.h265:
            str_rep += ' H265'
            if self.nvenc:
                str_rep += ' (NVENC)'
        if not str_rep: str_rep = 'no features'
        return str_rep.rstrip(',')


    def initialize_options(self):
        install.initialize_options(self)
        self.epiphan_dvi2pcie_duo = None
        self.i420 = None
        self.xvid = None
        self.h265 = None
        self.nvenc = None


    def finalize_options(self):
        install.finalize_options(self)


    def __new_build_dir(self):
        """Create new build directory and go there,
        incrementing build directory index.

        """
        chdir(self.here)
        self.build_dir_index += 1
        build_dir = '%s_%d' % (self.build_dir_prefix, self.build_dir_index)
        mkdir(build_dir)
        chdir(build_dir)


    def __check_command(self, cmd, err_msg, new_dir=True):
        """Check whether `cmd` exits normally,
        displaying `err_msg` and propagating the
        raised exception if not.

        This function changes to a new build
        directory before the check.

        @param cmd
        @param err_msg
        @param new_dir
        @return output of `cmd`
        """
        if new_dir: self.__new_build_dir()
        try:
            print('+++++ Running: %s\n\n' % (' '.join(cmd)))
            output_buffer = check_output(cmd)
            return output_buffer
        except:
            self.__print_err_msg(err_msg)
            raise


    def __print_err_msg(self, err_msg):
        """Print error message in a nice and
        standard format.

        """
        print('!!!!! ERROR !!!!!\n%s\n\n' % (err_msg))


    def run(self):
        self.build_dir_prefix = '_giftgrab-build'
        self.build_dir_index = 0
        self.here = abspath(dirname(__file__))

        # check whether CMake installed
        err_msg = '%s\n%s' % (
            'CMake does not seem to be installed on your system.',
            'CMake is needed to build GiftGrab.'
        )
        self.__check_command(['cmake'], err_msg)

        # check C++11 support
        cmd = ['cmake', join(join(self.here, 'cmake'), 'cpp11')]
        err_msg = '%s\n%s' % (
            'Your compiler does not seem to support C++11.',
            'A C++11 supporting compiler is needed to build GiftGrab.'
        )
        self.__check_command(cmd, err_msg)

        # check platform supported
        cmd = ['cmake', join(join(self.here, 'cmake'), 'platform')]
        err_msg = 'Your platform does not seem to be supported.'
        self.__check_command(cmd, err_msg)

        # check OpenCV
        if (self.epiphan_dvi2pcie_duo and not self.i420) \
           or self.xvid:
            cmd = ['cmake', join(join(self.here, 'cmake'), 'opencv')]
            err_msg = '%s\n%s%s' % (
                'OpenCV does not seem to be installed on your system.',
                'OpenCV is needed for Xvid and Epiphan DVI2PCIe Duo ',
                '(without EpiphanSDK) support.'
            )
            self.__check_command(cmd, err_msg)

        # check FFmpeg
        if self.h265 or self.nvenc:
            cmd = ['cmake', join(join(self.here, 'cmake'), 'ffmpeg')]
            err_msg = '%s\n%s' % (
                'FFmpeg does not seem to be installed on your system.',
                'FFmpeg is needed for H265 support.'
            )
            self.__check_command(cmd, err_msg)

            # built correctly ?
            cmd = ['ffmpeg', '-buildconf']
            err_msg = '%s\n%s' % (
                'Failed to obtain your FFmpeg build configuration.',
                'This command failed: %s %s' % tuple(cmd)
            )
            output_buffer = self.__check_command(cmd, err_msg)

            opt = '--enable-muxer=mp4'
            if opt not in output_buffer:
                err_summary = 'Your FFmpeg does not seem to support MP4.'
                err_msg = '%s\n%s%s' % (
                    err_summary,
                    'Please install FFmpeg with MP4 support ',
                    '(%s).' % (opt)
                )
                self.__print_err_msg(err_msg)
                raise LibError(err_summary)

            opt = '--enable-nvenc'
            if self.nvenc and opt not in output_buffer:
                err_summary = 'Your FFmpeg does not seem to support NVENC.'
                err_msg = '%s\n%s%s' % (
                    err_summary,
                    'Please install FFmpeg with NVENC support ',
                    '(%s).' % (opt)
                )
                self.__print_err_msg(err_msg)
                raise LibError(err_summary)

            opt = '--enable-libx265'
            if self.h265 and not self.nvenc and \
               opt not in output_buffer:
                err_summary = 'Your FFmpeg does not seem to support x265.'
                err_msg = '%s\n%s%s' % (
                    err_summary,
                    'Please install FFmpeg with x265 support ',
                    '(%s).' % (opt)
                )
                self.__print_err_msg(err_msg)
                raise LibError(err_summary)

        # check EpiphanSDK
        if self.i420:
            cmd = ['cmake', join(join(self.here, 'cmake'), 'epiphansdk')]
            err_msg = '%s\n%s' % (
                'EpiphanSDK does not seem to be installed on your system.',
                'EpiphanSDK is needed for I420 colour space support.'
            )
            self.__check_command(cmd, err_msg)

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
        cmd = ['cmake', self.here]
        cmd[1:1] = cmake_args
        err_msg = '%s\n%s' % (
            'There was an error (details below) ',
            'when trying to build GiftGrab.'
        )
        self.__check_command(cmd, err_msg)
        cmd = ['make', '-j', 'install']
        self.__check_command(cmd, err_msg, False)

        # TODO: the following is a massive hack
        # to create a symlink from Python base to
        # installed GiftGrab Python lib. Consider
        # replacing with something better
        cmd = ['cmake', '-L', self.here]
        err_msg = '%s\n%s %s' % (
            'Could not obtain CMake install prefix path.',
            'This step is necessary to make',
            'GiftGrab Python API accessible.'
        )
        output_buffer = self.__check_command(cmd, err_msg, False)
        cmake_install_prefix = output_buffer.split(
            'CMAKE_INSTALL_PREFIX:PATH=')[1]
        cmake_install_prefix = cmake_install_prefix.split('\n')[0]
        pylib_path = join(join(join(cmake_install_prefix, 'lib'),
                               'giftgrab'),
                          'pygiftgrab.so')
        pylib_link = join(getsitepackages()[0], 'pygiftgrab.so')
        # will create temp link first, and then move to actual one
        # to prevent "already exists" error
        tmp_pylib_link = pylib_link + '.tmp'
        symlink(pylib_path, tmp_pylib_link)
        rename(tmp_pylib_link, pylib_link)

        # everything fine so far:
        print('\n+++++ INFO +++++\n%s%s\n\n' % (
               'Installing GiftGrab with support for: ',
               str(self)
               )
        )
        install.run(self)


# TODO: pip python dependencies (e.g. py.test)

# TODO: resources (e.g. sources, headers, include dirs,
# etc. based on selected features)

setup(
    name='giftgrab',
    version='16.08.15rc1',
    cmdclass={
        'install': GiftGrabInstallCommand,
    },
)
