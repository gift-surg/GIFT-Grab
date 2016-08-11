from setuptools import setup, Extension

lib_sources = ['api/videoframe.cpp',
               'api/maskframe.cpp',
               'api/factory.cpp',
               'api/except.cpp']
lib_include_dirs = ['api', 'utils']
libgiftgrab = Extension('libgiftgrab',
                        sources=lib_sources,
                        include_dirs=lib_include_dirs,
                        extra_compile_args=['-std=c++11', '-O3', '-DNDEBUG', '-fPIC'],
                        extra_link_flags=['-fPIC', '-std=c++11', '-O3', '-DNDEBUG', '-shared'])

setup(
    name='giftgrab',
    version='16.08.11rc1',
    ext_modules=[libgiftgrab]
    )