from __future__ import print_function
from setuptools import setup, Extension
from distutils.errors import LibError
from setuptools.command.install import install
from distutils.command.build_ext import build_ext
from distutils.command.install_lib import install_lib
from os import mkdir, chdir, listdir, getcwd, link, rename
from os.path import join, abspath, dirname
from subprocess import check_output
from sys import stderr


def pip_root():
    """Get pip root directory, in different ways,
    depending on whether running from within a
    virtualenv.

    @return
    """

    import sys
    venv = hasattr(sys, 'real_prefix')

    if not venv:
        # see https://github.com/pypa/virtualenv/issues/355
        from site import getsitepackages
        site_packages = getsitepackages()[0]
    else:
        from sys import exec_prefix, version_info
        site_packages = 'python%d.%d' % (version_info.major,
                                         version_info.minor)
        site_packages = join('lib', site_packages)
        site_packages = join(site_packages, 'site-packages')
        site_packages = join(exec_prefix, site_packages)

    return site_packages


def cmake_install_prefix():
    """Generate ``CMAKE_INSTALL_PREFIX`` for installing
    GiftGrab under the PyPI root.

    @return
    """
    return join(pip_root(), 'giftgrab')


class GiftGrabPyExtension(Extension):

    """This class is used for adding GiftGrab Python
    library (built inside install command class below)
    to the PyPI install chain.

    """
    def __init__(self):
        self.name = 'pygiftgrab'
        self.sources = ['']


class GiftGrabBuildExtCommand(build_ext):

    """This class is used for adding GiftGrab
    components (build inside the GiftGrab install
    command class) to the PyPI install chain.

    """

    def run(self):
        pass


class GiftGrabInstallCommand(install):

    """This class builds GiftGrab components based on
    requested features from user.

    """

    user_options = install.user_options + \
                  [('epiphan-dvi2pcie-duo', None, None),
                   ('i420', None, None),
                   ('xvid', None, None),
                   ('hevc', None, None),
                   ('x265', None, None),
                   ('enable-gpl', None, None),
                   ('vp9', None, None),
                   ('nvenc', None, None),
                   ('cmake-install-prefix=', None, None)]


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
        if self.vp9:
            str_rep += ' VP9,'
        if self.hevc:
            str_rep += ' HEVC'
            if self.nvenc:
                str_rep += ' (NVENC)'
            elif self.x265 and self.enable_gpl:
                str_rep += ' (x265)'
        if not str_rep: str_rep = 'no features'
        return str_rep.rstrip(',')


    def initialize_options(self):
        install.initialize_options(self)
        self.epiphan_dvi2pcie_duo = None
        self.i420 = None
        self.xvid = None
        self.hevc = None
        self.x265 = None
        self.enable_gpl = None
        self.vp9 = None
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
        print('!!!!! ERROR !!!!!\n%s\n\n' % (err_msg), file=stderr)


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

        # check PythonLibs
        cmd = ['cmake', join(join(self.here, 'cmake'), 'pythonlibs')]
        err_msg = '%s\n%s' % (
            'PythonLibs do not seem to be installed on your system.',
            'PythonLibs are needed for the GiftGrab Python API.'
        )
        self.__check_command(cmd, err_msg)

        # check Boost.Python
        cmd = ['cmake', join(join(self.here, 'cmake'), 'boost.python')]
        err_msg = '%s\n%s' % (
            'Boost.Python does not seem to be installed on your system.',
            'Boost.Python is needed for the GiftGrab Python API.'
        )
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

        # check libVLC
        if self.epiphan_dvi2pcie_duo and self.i420:
            cmd = ['cmake', join(join(self.here, 'cmake'), 'libvlc')]
            err_msg = '%s\n%s' % (
                'libVLC does not seem to be installed on your system.',
                'libVLC is needed for I420 colour space support.'
            )
            self.__check_command(cmd, err_msg)

        # check FFmpeg
        if self.hevc or self.vp9:
            cmd = ['cmake', join(join(self.here, 'cmake'), 'ffmpeg')]
            err_msg = '%s\n%s' % (
                'FFmpeg does not seem to be installed on your system.',
                'FFmpeg is needed for HEVC or VP9 support.'
            )
            self.__check_command(cmd, err_msg)

            # built correctly ?
            cmd = ['ffmpeg', '-buildconf']
            err_msg = '%s\n%s' % (
                'Failed to obtain your FFmpeg build configuration.',
                'This command failed: %s %s' % tuple(cmd)
            )
            output_buffer = self.__check_command(cmd, err_msg)

            if self.hevc:
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

                if self.nvenc:
                    opt = '--enable-nvenc'
                    if opt not in output_buffer:
                        err_summary = 'Your FFmpeg does not seem to support NVENC.'
                        err_msg = '%s\n%s%s' % (
                            err_summary,
                            'Please install FFmpeg with NVENC support ',
                            '(%s).' % (opt)
                        )
                        self.__print_err_msg(err_msg)
                        raise LibError(err_summary)
                elif self.x265:
                    if not self.enable_gpl:
                        err_msg = 'You must enable GPL for x265 support'
                        self.__print_err_msg(err_msg)
                        raise LibError(err_msg)
                    else:
                        opt = '--enable-libx265'
                        if opt not in output_buffer:
                            err_summary = 'Your FFmpeg does not seem to support x265.'
                            err_msg = '%s\n%s%s' % (
                                err_summary,
                                'Please install FFmpeg with x265 support ',
                                '(%s).' % (opt)
                            )
                            self.__print_err_msg(err_msg)
                            raise LibError(err_summary)
                else:
                    opt = '--enable-libkvazaar'
                    if opt not in output_buffer:
                        err_summary = 'Your FFmpeg does not seem to support kvazaar.'
                        err_msg = '%s\n%s%s' % (
                            err_summary,
                            'Please install FFmpeg with kvazaar support ',
                            '(%s).' % (opt)
                        )
                        self.__print_err_msg(err_msg)
                        raise LibError(err_summary)

            if self.vp9:
                opt = '--enable-muxer=webm'
                if opt not in output_buffer:
                    err_summary = 'Your FFmpeg does not seem to support WebM.'
                    err_msg = '%s\n%s%s' % (
                        err_summary,
                        'Please install FFmpeg with WebM support ',
                        '(%s).' % (opt)
                    )
                    self.__print_err_msg(err_msg)
                    raise LibError(err_summary)

                opt = '--enable-libvpx'
                if opt not in output_buffer:
                    err_summary = 'Your FFmpeg does not seem to support libvpx.'
                    err_msg = '%s\n%s%s' % (
                        err_summary,
                        'Please install FFmpeg with libvpx support ',
                        '(%s).' % (opt)
                    )
                    self.__print_err_msg(err_msg)
                    raise LibError(err_summary)

        # moment of truth
        cmake_args = []
        cmake_args.append('-DBUILD_PYTHON=ON')
        cmake_args.append('-DBUILD_TESTS=ON')
        if self.epiphan_dvi2pcie_duo:
            cmake_args.append('-DUSE_EPIPHAN_DVI2PCIE_DUO=ON')
        if self.xvid:
            cmake_args.append('-DUSE_XVID=ON')
        if self.hevc:
            cmake_args.append('-DUSE_HEVC=ON')
            if self.nvenc:
                cmake_args.append('-DUSE_NVENC=ON')
            elif self.x265:
                cmake_args.append('-DENABLE_GPL=ON')
                cmake_args.append('-DUSE_X265=ON')
        if self.vp9:
            cmake_args.append('-DUSE_VP9=ON')
        if self.i420:
            cmake_args.append('-DUSE_I420=ON')
        cmake_args.append('-DCMAKE_INSTALL_PREFIX=%s' % (
                          cmake_install_prefix()))
        cmd = ['cmake', self.here]
        cmd[1:1] = cmake_args
        err_msg = '%s\n%s' % (
            'There was an error (details below) ',
            'when trying to build GiftGrab.'
        )
        self.__check_command(cmd, err_msg)
        cmd = ['make', '-j', 'install']
        output_buffer = self.__check_command(cmd, err_msg, False)
        print(output_buffer)

        # TODO: the following is a massive hack
        # to create a hard link from Python base to
        # installed GiftGrab Python lib. Consider
        # replacing with something better
        cmd = ['cmake', '-L', self.here]
        err_msg = '%s\n%s %s' % (
            'Could not obtain CMake install prefix path.',
            'This step is necessary to make',
            'GiftGrab Python API accessible.'
        )
        output_buffer = self.__check_command(cmd, err_msg, False)
        pylib_path = join(join(join(cmake_install_prefix(), 'lib'),
                               'giftgrab'),
                          'pygiftgrab.so')
        pylib_link = join(pip_root(), 'pygiftgrab.so')
        print('\n+++++ INFO +++++\n%s%s\n\n' % (
               'Installing GiftGrab with support for: ',
               str(self)
               )
        )
        # will create temp link first, and then move to actual one
        # to prevent "already exists" error
        tmp_pylib_link = pylib_link + '.tmp'
        link(pylib_path, tmp_pylib_link)
        rename(tmp_pylib_link, pylib_link)
        print('+++++ INFO +++++\n%s %s %s %s\n\n' % (
              'Created symlink',
              pylib_link,
              'to',
              pylib_path)
        )

        # everything fine so far:
        chdir(self.here)
        install.run(self)

class GiftGrabInstallLibCommand(install_lib):

    """This class adds GiftGrab installation directory
    to list of installed files, for deletion upon
    ``pip uninstall``.

    """

    def get_outputs(self):
        return [cmake_install_prefix()] + install_lib.get_outputs(self)


summary = 'GIFT-Grab provides a user-friendly Python and C++ API for the ' +\
          'real-time acquisition and encoding of multi-channel video ' +\
          'streams by means of frame-grabber hardware.'
description = 'GIFT-Grab was developed as part of the GIFT-Surg project ' +\
              '(http://www.gift-surg.ac.uk) at the Translational Imaging ' +\
              'Group in the Centre for Medical Image Computing at ' +\
              'University College London (UCL). See the GIFT-Grab ' +\
              'home-page on GitHub for features and manual.' +\
              '\n\n' +\
              'Copyright (c) 2016, University College London. This work ' +\
              'was supported through an Innovative Engineering for Health ' +\
              'award by the Wellcome Trust [WT101957], the Engineering ' +\
              'and Physical Sciences Research Council (EPSRC) and a ' +\
              'National Institute for Health Research Biomedical Research ' +\
              'Centre UCLH/UCL High Impact Initiative.'
description = '%s\n\n%s' % (summary, description)
console_scripts = []
for port in ['sdi', 'dvi']:
    for colour_space in ['bgr24', 'i420']:
        for codec in ['xvid', 'hevc', 'vp9']:
             record_script = 'edd-{}-{}-{}'.format(
                 port, colour_space, codec)
             record_script += '=giftgrab.utils:record_epiphan_dvi2pcieduo_{}_{}_{}'.format(
                 port, colour_space, codec)
             console_scripts.append(record_script)
for colour_space in ['bgra', 'i420']:
    for codec in ['xvid', 'hevc', 'vp9']:
        target_script = 'test-giftgrab-{}-{}'.format(
            codec, colour_space)
        target_script += '=giftgrab.tests:test_{}_{}'.format(
            codec, colour_space)
        console_scripts.append(target_script)
console_scripts = console_scripts +\
                  ['test-giftgrab-epiphan-dvi2pcieduo-bgr24=giftgrab.tests:test_epiphan_dvi2pcieduo_bgr24',
                   'test-giftgrab-epiphan-dvi2pcieduo-i420=giftgrab.tests:test_epiphan_dvi2pcieduo_i420']
setup(
    name='gift-grab',
    version='16.08.24rc1',
    description=summary,
    long_description=description,
    url='https://github.com/gift-surg/GIFT-Grab',
    author='Dzhoshkun Ismail Shakir',
    author_email='d.shakir@ucl.ac.uk',
    license='BSD-3-Clause',
    classifiers=[
        'Development Status :: 4 - Beta',

        'Intended Audience :: Developers',
        'Intended Audience :: End Users/Desktop',
        'Intended Audience :: Healthcare Industry',
        'Intended Audience :: Information Technology',
        'Intended Audience :: Science/Research',

        'License :: OSI Approved :: BSD License',

        'Operating System :: POSIX :: Linux',

        'Programming Language :: C++',
        'Programming Language :: Python',

        'Topic :: Multimedia :: Video',
        'Topic :: Multimedia :: Video :: Capture',
        'Topic :: Multimedia :: Video :: Conversion',
        'Topic :: Scientific/Engineering',
        'Topic :: Software Development :: Libraries :: Python Modules',
        'Topic :: Utilities'
    ],
    keywords='frame-grabber, framegrabber, frame grabber,'
             'multi-channel video capture, multi-channel video encoding,'
             'hardware-accelerated video encoding,'
             'GPU-accelerated video encoding,'
             'real-time video encoding, codec, Xvid, H.265, HEVC, VP9,'
             'Epiphan DVI2PCIe Duo, medical imaging,'
             'FFmpeg, OpenCV, kvazaar, x265, NVENC'
             'GIFT-Surg',

    install_requires=['pytest', 'PyYAML'],
    packages=['giftgrab', 'giftgrab.tests', 'giftgrab.utils'],
    package_dir={'giftgrab': join('python', 'modules', 'giftgrab'),
                 'giftgrab.tests': 'tests',
                 'giftgrab.utils': join('python', 'modules', 'giftgrab', 'utils')},
    py_modules=['giftgrab.epiphan', 'giftgrab.utils.inspection'],
    ext_modules=[GiftGrabPyExtension()],
    cmdclass={
        'install': GiftGrabInstallCommand,
        'build_ext': GiftGrabBuildExtCommand,
        'install_lib': GiftGrabInstallLibCommand
    },
    entry_points={
        'console_scripts': console_scripts,
    },
    package_data={'giftgrab.tests': [join('target', '*.py'),
                                     join('epiphan', 'dvi2pcieduo', '*.py'),
                                     join('epiphan', 'dvi2pcieduo', 'data', '*.yml')]}
)
