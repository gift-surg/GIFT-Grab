from setuptools import setup, Extension
from distutils.command.build import build
from sys import version_info, exec_prefix
from sysconfig import get_platform
from os.path import join

lib_sources = ['api/videoframe.cpp',
               'api/maskframe.cpp',
               'api/factory.cpp',
               'api/except.cpp']
lib_include_dirs = ['api', 'utils']
# TODO: BUILD_TESTS
# TODO: all flags, e.g. -Wl
extra_compile_args = ['-std=c++11', '-O3', '-DNDEBUG', '-fPIC']
extra_link_args = ['-fPIC', '-std=c++11', '-O3', '-DNDEBUG', '-shared']

py_sources = ['python/wrapper.cpp']
py_include_dirs = lib_include_dirs + ['/usr/include/python2.7']
libgiftgrab = Extension('libgiftgrab',
                        sources=lib_sources,
                        include_dirs=lib_include_dirs,
                        extra_compile_args=extra_compile_args,
                        extra_link_flags=extra_link_args
                        )
py_runtime_library_dirs = [join(
                              join(join(exec_prefix, 'local/lib'),
                                'python%d.%d' % (version_info.major, version_info.minor)
                                ), 'site-packages')]
py_library_dirs = [join('build',
                        'lib.%s-%d.%d' % (get_platform(), version_info.major, version_info.minor))]
print py_library_dirs
print py_runtime_library_dirs
pygiftgrab = Extension('pygiftgrab',
                       sources=py_sources,
                       include_dirs=py_include_dirs,
                       extra_compile_args=extra_compile_args,
                       extra_link_flags=extra_link_args,
                       libraries=['boost_python', libgiftgrab.name[3:]],
                       library_dirs=py_library_dirs,
                       runtime_library_dirs=py_runtime_library_dirs,
                       )
setup(
    name='giftgrab',
    version='16.08.11rc1',
    ext_modules=[libgiftgrab, pygiftgrab]
    )