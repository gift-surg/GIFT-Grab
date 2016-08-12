from setuptools import setup, Extension
from os import environ
from giftgrab_configurator import Builder


# TODO: pip python dependencies (e.g. py.test)

# TODO: resources (e.g. sources, headers, include dirs,
# etc. based on selected features)

# TODO: error if c++ doesn't exist
environ["CC"] = "c++"
environ["CXX"] = "c++"

# TODO: pass user options instead of None
builder = Builder(None)


# TODO: BUILD_TESTS

libgiftgrab = Extension(name=builder.lib_name(),
                        sources=builder.lib_sources(),
                        include_dirs=builder.lib_include_dirs(),
                        extra_compile_args=builder.lib_compile_args(),
                        extra_link_args=builder.lib_link_args()
                        )

pygiftgrab = Extension(name=builder.py_name(),
                       sources=builder.py_sources(),
                       include_dirs=builder.py_include_dirs(),
                       extra_compile_args=builder.py_compile_args(),
                       extra_link_args=builder.py_link_args(),
                       libraries=builder.py_libraries(),
                       library_dirs=builder.py_library_dirs(),
                       runtime_library_dirs=builder.py_runtime_library_dirs(),
                       )
setup(
    name='giftgrab',
    version='16.08.11rc1',
    ext_modules=[libgiftgrab, pygiftgrab]
    )