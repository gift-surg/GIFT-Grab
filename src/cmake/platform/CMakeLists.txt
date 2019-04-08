CMAKE_MINIMUM_REQUIRED(VERSION 3.2.0)

# Unsupported platforms
if(APPLE)
    MESSAGE(FATAL_ERROR "This library does not support Mac OS X yet.")
endif(APPLE)
if(WIN32)
    MESSAGE(WARNING "Windows support is experimental.")
endif(WIN32)
