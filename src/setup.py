from setuptools import setup, Extension

pygiftgrab = Extension('pygiftgrab',
                       sources=['api/videoframe.cpp',
                        'api/maskframe.cpp',
                        'api/factory.cpp',
                        'api/except.cpp'],
                        include_dirs=['../src/api', '../src/utils'],
                        extra_compile_args=['-std=c++11', '-O3', '-DNDEBUG', '-fPIC'],
                        extra_link_flags=['-fPIC', '-std=c++11', '-O3', '-DNDEBUG', '-shared'])

setup(
    name='giftgrab',
    version='16.08.11rc1',
    ext_modules=[pygiftgrab]
    )