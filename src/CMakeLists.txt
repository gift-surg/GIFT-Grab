PROJECT(GiftGrab)
CMAKE_MINIMUM_REQUIRED(VERSION 3.2.0)

# Build types and default build type
SET_PROPERTY(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    MESSAGE(STATUS "Setting build type to 'Release' as none was specified.")
    SET(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()

# Unsupported platforms
if(APPLE)
    MESSAGE(FATAL_ERROR "This library does not support Mac OS X yet.")
endif(APPLE)
if(WIN32)
    MESSAGE(FATAL_ERROR "This library does not support Windows yet.")
endif(WIN32)

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
OPTION(USE_BLACKMAGIC "Support for streaming video using BlackMagic framegrabbers" OFF)
OPTION(USE_FFMPEG "Support for saving video to files using FFmpeg" OFF)
OPTION(USE_EPIPHAN_DVI2PCIE_DUO "Support for Epiphan DVI2PCIe Duo" OFF)
OPTION(USE_H265 "Support for the H265 codec (HEVC)" OFF)
OPTION(USE_XVID "Support for the Xvid codec" OFF)
# OpenCV
FIND_PACKAGE(OpenCV REQUIRED)
if(OpenCV_FOUND)
    INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})
else(OpenCV_FOUND)
    MESSAGE(FATAL_ERROR "OpenCV could not be found. This library requires OpenCV.")
endif(OpenCV_FOUND)

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
SET(LIBS ${OpenCV_LIBS})

# Video files

# BlackMagicSDK
if(USE_BLACKMAGIC)
    FIND_PACKAGE(BlackMagicSDK CONFIG REQUIRED)
    if(BlackMagicSDK_FOUND)
        INCLUDE_DIRECTORIES(${BlackMagicSDK_INCLUDE_DIR})
        INCLUDE_DIRECTORIES(blackmagic)
        ADD_DEFINITIONS(-DUSE_BLACKMAGIC)
        LIST(APPEND HEADERS blackmagic/blackmagic_frame_grabber.h)
        LIST(APPEND SOURCES blackmagic/blackmagic_frame_grabber.cpp)
        LIST(APPEND LIBS ${BlackMagicSDK_LIBS})
    else(BlackMagicSDK_FOUND)
        SET(USE_BLACKMAGIC OFF CACHE BOOL "ON OFF" FORCE)
        MESSAGE(FATAL_ERROR "Blackmagic SDK could not be found.")
    endif(BlackMagicSDK_FOUND)
endif(USE_BLACKMAGIC)

# we do not officially support libVLC yet!
SET(USE_LIBVLC OFF CACHE BOOL "ON OFF" FORCE)

# OpenCV framegrabber
if(USE_OPENCV)
    INCLUDE_DIRECTORIES(opencv)
    LIST(APPEND HEADERS opencv/opencv_video_source.h)
    LIST(APPEND HEADERS opencv/opencv_video_target.h)
    LIST(APPEND SOURCES opencv/opencv_video_source.cpp)
    LIST(APPEND SOURCES opencv/opencv_video_target.cpp)
endif(USE_OPENCV)

# FFmpeg video encoder
if(USE_FFMPEG)
    ADD_DEFINITIONS(-DUSE_FFMPEG)
    OPTION(FFMPEG_HWACCEL
        "NVIDIA GPU-based hardware-accelerated video encoding" OFF)
    if(FFMPEG_HWACCEL)
        ADD_DEFINITIONS(-DFFMPEG_HWACCEL)
    endif(FFMPEG_HWACCEL)
    INCLUDE_DIRECTORIES(${FFmpeg_DIR}/include)
    INCLUDE_DIRECTORIES(ffmpeg)
    LIST(APPEND HEADERS ffmpeg/ffmpeg_video_target.h)
    LIST(APPEND SOURCES ffmpeg/ffmpeg_video_target.cpp)
    LIST(APPEND LIBS
        ${FFmpeg_DIR}/lib/libavformat.so
        ${FFmpeg_DIR}/lib/libavcodec.so
        ${FFmpeg_DIR}/lib/libavutil.so
        ${FFmpeg_DIR}/lib/libswscale.so
    )
endif(USE_FFMPEG)

# libVLC
if(USE_LIBVLC)
    LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
    FIND_PACKAGE(LIBVLC REQUIRED)
    if(LIBVLC_FOUND)
        ADD_DEFINITIONS(-DUSE_LIBVLC)
        INCLUDE_DIRECTORIES(vlc)
        LIST(APPEND HEADERS vlc/vlc_video_source.h)
        LIST(APPEND SOURCES vlc/vlc_video_source.cpp)
        LIST(APPEND LIBS ${LIBVLC_LIBRARY})
    else(LIBVLC_FOUND)
        SET(USE_LIBVLC OFF CACHE BOOL "ON OFF" FORCE)
        MESSAGE(FATAL_ERROR "libVLC could not be found.")
    endif(LIBVLC_FOUND)
endif(USE_LIBVLC)

# Support for I420 (YUV420p) colour space
OPTION(USE_COLOUR_SPACE_I420
    "Support for grabbing frames in I420 colour space instead of BGR24" OFF)
if(USE_COLOUR_SPACE_I420)
    SET(USE_EPIPHANSDK ON)
    ADD_DEFINITIONS(-DUSE_COLOUR_SPACE_I420)
    LIST(APPEND HEADERS api/videoframe_i420.h)
    LIST(APPEND SOURCES api/videoframe_i420.cpp)
endif(USE_COLOUR_SPACE_I420)

# Epiphan SDK video source
if(USE_EPIPHANSDK)
    FIND_PACKAGE(EpiphanSDK CONFIG REQUIRED)
    INCLUDE_DIRECTORIES(${EpiphanSDK_INCLUDE_DIR})
    INCLUDE_DIRECTORIES(epiphansdk)
    LIST(APPEND LIBS ${EpiphanSDK_LIBS})
    LIST(APPEND HEADERS epiphansdk/epiphansdk_video_source.h)
    LIST(APPEND SOURCES epiphansdk/epiphansdk_video_source.cpp)
endif(USE_EPIPHANSDK)

# Performance benchmarking
OPTION(GENERATE_PERFORMANCE_OUTPUT
    "Support for performance (timer) reports via Boost.Timer" OFF)
MARK_AS_ADVANCED(GENERATE_PERFORMANCE_OUTPUT)
if(GENERATE_PERFORMANCE_OUTPUT)
    FIND_PACKAGE(Boost COMPONENTS timer system REQUIRED)
    ADD_DEFINITIONS(-DGENERATE_PERFORMANCE_OUTPUT)
    LIST(APPEND LIBS ${Boost_LIBRARIES})
endif(GENERATE_PERFORMANCE_OUTPUT)

# Compile and link
ADD_LIBRARY(${NAME} SHARED ${HEADERS} ${SOURCES})
TARGET_LINK_LIBRARIES(${NAME} ${LIBS})
LIST(APPEND INSTALLABLES ${NAME})

# Documentation
OPTION(BUILD_DOC "Build source code documentation" OFF)
if(BUILD_DOC)
    FIND_PACKAGE(Doxygen)
    if(DOXYGEN_FOUND)
        SET(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/../doc/Doxyfile.in)
        SET(DOXYFILE ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile)
        CONFIGURE_FILE(${DOXYFILE_IN}
            ${DOXYFILE} @ONLY)
        ADD_CUSTOM_TARGET(doc ALL
            ${DOXYGEN_EXECUTABLE} ${DOXYFILE}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc
            COMMENT "Generating API documentation with Doxygen" VERBATIM)
    endif(DOXYGEN_FOUND)
endif(BUILD_DOC)

# Tests
OPTION(BUILD_TESTS "Build tests" OFF)
if(BUILD_TESTS)
    SET(TEST_NAME save_frames)
    ADD_EXECUTABLE(${TEST_NAME} tests/save_frames.cpp)
    TARGET_LINK_LIBRARIES(${TEST_NAME} ${NAME} ${LIBS})

    SET(TEST_NAME dvi2pcieduo)
    ADD_EXECUTABLE(${TEST_NAME} tests/epiphan/dvi2pcieduo.cpp)
    TARGET_LINK_LIBRARIES(${TEST_NAME} ${NAME} ${LIBS})

    ENABLE_TESTING()

    if(USE_EPIPHAN_DVI2PCIE_DUO)
        SUBDIRS(tests/epiphan/dvi2pcieduo)
    endif(USE_EPIPHAN_DVI2PCIE_DUO)

    if(USE_H265)
        SUBDIRS(tests/h265)
    endif(USE_H265)

    if(USE_XVID)
        SUBDIRS(tests/xvid)
    endif(USE_XVID)
endif(BUILD_TESTS)

# Python wrappers
OPTION(BUILD_PYTHON "Build GiftGrab Python API" OFF)
if(BUILD_PYTHON)
    FIND_PACKAGE(PythonLibs 2.7 REQUIRED)
    INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_DIRS})
    FIND_PACKAGE(Boost COMPONENTS python REQUIRED)
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
    SET(NAME_PYTHON py${NAME})
    ADD_LIBRARY(${NAME_PYTHON} SHARED python/wrapper.cpp)
    TARGET_LINK_LIBRARIES(${NAME_PYTHON} ${Boost_LIBRARIES} ${NAME})
    FILE(COPY tests/save_frames.py DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    # no lib prefix
    SET_TARGET_PROPERTIES(${NAME_PYTHON} PROPERTIES PREFIX "")
    LIST(APPEND INSTALLABLES ${NAME_PYTHON})
endif(BUILD_PYTHON)

# Install
SET(GiftGrab_PATH_SUFFIX giftgrab)
INSTALL(
    FILES ${HEADERS} cmake/GiftGrabConfig.cmake
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include/${GiftGrab_PATH_SUFFIX}
    )
INSTALL(
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/python/modules/epiphan.py
    DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/${GiftGrab_PATH_SUFFIX}
    )
INSTALL(
    TARGETS ${INSTALLABLES}
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/${GiftGrab_PATH_SUFFIX}
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/${GiftGrab_PATH_SUFFIX}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/${GiftGrab_PATH_SUFFIX}
    )
