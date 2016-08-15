PROJECT(GiftGrab)
CMAKE_MINIMUM_REQUIRED(VERSION 3.2.0)

# Build types and default build type
SET_PROPERTY(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    MESSAGE(STATUS "Setting build type to 'Release' as none was specified.")
    SET(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()

ADD_SUBDIRECTORY(cmake/platform)

# C++11
INCLUDE(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX11)
    LIST(APPEND CMAKE_CXX_FLAGS "-std=c++11")
else()
    CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
    if(COMPILER_SUPPORTS_CXX0X)
        LIST(APPEND CMAKE_CXX_FLAGS "-std=c++0x")
    else()
        MESSAGE(FATAL_ERROR
            "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support.")
    endif()
endif()

# Configurable options, depending on desired functionality
OPTION(USE_EPIPHAN_DVI2PCIE_DUO
    "Capture video with Epiphan DVI2PCIe Duo" OFF)
OPTION(USE_H265
    "Video encoding with H.265 (requires FFmpeg, pkg-config and x265)" OFF)
OPTION(USE_XVID
    "Video encoding with Xvid (requires OpenCV)" OFF)
OPTION(USE_NVENC
    "Hardware-accelerated video encoding with H.265 (requires FFmpeg, pkg-config and NVENC)" OFF)
OPTION(BUILD_DOC
    "Source code documentation (requires Doxygen)" OFF)
OPTION(BUILD_TESTS
    "Tests (requires pytest)" OFF)
OPTION(BUILD_PYTHON
    "GiftGrab Python API (requires Python and Boost.Python)" OFF)
OPTION(USE_I420
    "Capture video in I420 colour space (requires EpiphanSDK)" OFF)
OPTION(GENERATE_PERFORMANCE_OUTPUT
    "Performance benchmark reports (requires Boost.Timer and Boost.System)" OFF)
MARK_AS_ADVANCED(GENERATE_PERFORMANCE_OUTPUT)

# Determine whether OpenCV needed
if((USE_EPIPHAN_DVI2PCIE_DUO AND NOT USE_I420) OR
    USE_XVID)
    SET(USE_OPENCV ON CACHE BOOL "OFF ON" FORCE)
endif()

# Determine whether FFmpeg needed
if(USE_H265 OR USE_NVENC)
    SET(USE_FFMPEG ON CACHE BOOL "OFF ON" FORCE)
endif()

# Library name
SET(NAME giftgrab)

# Library include dirs
INCLUDE_DIRECTORIES(api)
INCLUDE_DIRECTORIES(utils)

# Variable for header and source files
SET(HEADERS
    api/ivideosource.h
    api/ivideotarget.h
    api/videoframe.h
    api/maskframe.h
    api/factory.h
    api/except.h
)
SET(SOURCES
    api/videoframe.cpp
    api/maskframe.cpp
    api/factory.cpp
    api/except.cpp
)

# OpenCV components
if(USE_OPENCV)
    ADD_SUBDIRECTORY(cmake/opencv)
    INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})
    LIST(APPEND LIBS ${OpenCV_LIBS})
    ADD_DEFINITIONS(-DUSE_OPENCV)

    INCLUDE_DIRECTORIES(opencv)
    LIST(APPEND HEADERS opencv/opencv_video_source.h)
    LIST(APPEND HEADERS opencv/opencv_video_target.h)
    LIST(APPEND SOURCES opencv/opencv_video_source.cpp)
    LIST(APPEND SOURCES opencv/opencv_video_target.cpp)
endif(USE_OPENCV)

# FFmpeg components
if(USE_FFMPEG)
    ADD_SUBDIRECTORY(cmake/ffmpeg)
    INCLUDE_DIRECTORIES(${FFmpeg_INCLUDE_DIRS})
    LIST(APPEND LIBS ${FFmpeg_LIBS})
    LINK_DIRECTORIES(${FFmpeg_LIBRARY_DIRS})
    ADD_DEFINITIONS(-DUSE_FFMPEG)
    if(USE_NVENC)
        ADD_DEFINITIONS(-DUSE_NVENC)
    endif(USE_NVENC)

    INCLUDE_DIRECTORIES(ffmpeg)
    LIST(APPEND HEADERS ffmpeg/ffmpeg_video_target.h)
    LIST(APPEND SOURCES ffmpeg/ffmpeg_video_target.cpp)
endif(USE_FFMPEG)

# Support for I420 (YUV420p) colour space
if(USE_I420)
    SET(USE_EPIPHANSDK ON CACHE BOOL "OFF ON" FORCE)
    ADD_DEFINITIONS(-DUSE_I420)

    LIST(APPEND HEADERS api/videoframe_i420.h)
    LIST(APPEND SOURCES api/videoframe_i420.cpp)
endif(USE_I420)

# Epiphan SDK video source
if(USE_EPIPHANSDK)
    ADD_SUBDIRECTORY(cmake/epiphansdk)
    INCLUDE_DIRECTORIES(${EpiphanSDK_INCLUDE_DIRS})
    LIST(APPEND LIBS ${EpiphanSDK_LIBS})

    INCLUDE_DIRECTORIES(epiphansdk)
    LIST(APPEND HEADERS epiphansdk/epiphansdk_video_source.h)
    LIST(APPEND SOURCES epiphansdk/epiphansdk_video_source.cpp)
endif(USE_EPIPHANSDK)

# Performance benchmarking
if(GENERATE_PERFORMANCE_OUTPUT)
    FIND_PACKAGE(Boost COMPONENTS timer system REQUIRED)
    LIST(APPEND LIBS ${Boost_LIBRARIES})
    ADD_DEFINITIONS(-DGENERATE_PERFORMANCE_OUTPUT)
endif(GENERATE_PERFORMANCE_OUTPUT)

# Compile and link
ADD_LIBRARY(${NAME} SHARED ${HEADERS} ${SOURCES})
TARGET_LINK_LIBRARIES(${NAME} ${LIBS})
LIST(APPEND INSTALLABLES ${NAME})

# Documentation
if(BUILD_DOC)
    FIND_PACKAGE(Doxygen REQUIRED)
    SET(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/../doc/Doxyfile.in)
    SET(DOXYFILE ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile)
    CONFIGURE_FILE(${DOXYFILE_IN} ${DOXYFILE} @ONLY)
    ADD_CUSTOM_TARGET(doc ALL
        ${DOXYGEN_EXECUTABLE} ${DOXYFILE}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc
        COMMENT "Generating API documentation with Doxygen" VERBATIM)
endif(BUILD_DOC)

# Tests
if(BUILD_TESTS)
    if(NOT BUILD_PYTHON)
        MESSAGE(FATAL_ERROR "Tests currently depend on Python support")
    endif()

    ENABLE_TESTING()

    SUBDIRS(tests)
endif(BUILD_TESTS)

# Python wrappers
if(BUILD_PYTHON)
    FIND_PACKAGE(PythonLibs 2.7 REQUIRED)
    INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_DIRS})
    FIND_PACKAGE(Boost COMPONENTS python REQUIRED)
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

    SET(NAME_PYTHON py${NAME})
    ADD_LIBRARY(${NAME_PYTHON} SHARED python/wrapper.cpp)
    TARGET_LINK_LIBRARIES(${NAME_PYTHON} ${Boost_LIBRARIES} ${NAME})
    # no lib prefix
    SET_TARGET_PROPERTIES(${NAME_PYTHON} PROPERTIES PREFIX "")
    LIST(APPEND INSTALLABLES ${NAME_PYTHON})
endif(BUILD_PYTHON)

# Install
SET(GiftGrab_PATH_SUFFIX giftgrab)
INSTALL(
    FILES ${HEADERS}
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include/${GiftGrab_PATH_SUFFIX}
)
INSTALL(
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/python/modules/epiphan.py
    cmake/GiftGrabConfig.cmake
    DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/${GiftGrab_PATH_SUFFIX}
)
INSTALL(
    TARGETS ${INSTALLABLES}
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/${GiftGrab_PATH_SUFFIX}
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/${GiftGrab_PATH_SUFFIX}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/${GiftGrab_PATH_SUFFIX}
)
