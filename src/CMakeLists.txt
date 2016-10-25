PROJECT(GiftGrab)
CMAKE_MINIMUM_REQUIRED(VERSION 3.2.0)

# Will be used when installing
SET(GiftGrab_PATH_SUFFIX giftgrab)

# RPATH stuff - to avoid losing linking information
SET(CMAKE_SKIP_BUILD_RPATH FALSE)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib/${GiftGrab_PATH_SUFFIX}")
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# Build types and default build type
SET_PROPERTY(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    MESSAGE(STATUS "Setting build type to 'Release' as none was specified.")
    SET(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()

ADD_SUBDIRECTORY(cmake/platform)
ADD_SUBDIRECTORY(cmake/cpp11)

# Configurable options, depending on desired functionality
OPTION(USE_EPIPHAN_DVI2PCIE_DUO
    "Capture video with Epiphan DVI2PCIe Duo" OFF)
OPTION(USE_HEVC
    "Video encoding with HEVC (requires FFmpeg, pkg-config and kvazaar [default] or x265 [turn USE_X265 on])" OFF)
OPTION(USE_X265
    "Use x265 instead of kvazaar for HEVC video encoding (requires ENABLE_GPL as well)" OFF)
OPTION(ENABLE_GPL
    "Enable GPL libraries" OFF)
OPTION(USE_XVID
    "Video encoding with Xvid (requires OpenCV)" OFF)
OPTION(USE_VP9
    "Video encoding with VP9 (requires FFmpeg, pkg-config and libvpx)" OFF)
OPTION(USE_NVENC
    "Hardware-accelerated video encoding with HEVC (requires FFmpeg, pkg-config and NVENC)" OFF)
OPTION(ENABLE_NONFREE
    "Enable non-free components" OFF)
OPTION(BUILD_DOC
    "Source code documentation (requires Doxygen)" OFF)
OPTION(BUILD_TESTS
    "Tests (requires pytest)" OFF)
OPTION(BUILD_PYTHON
    "GIFT-Grab Python API (requires Python and Boost.Python)" OFF)
OPTION(USE_BGRA
    "Capture video in BGRA colour space (requires OpenCV)" ON)
OPTION(USE_I420
    "Capture video in I420 colour space (requires libVLC)" ON)
OPTION(GENERATE_PERFORMANCE_OUTPUT
    "Performance benchmark reports (requires Boost.Timer and Boost.System)" OFF)
MARK_AS_ADVANCED(GENERATE_PERFORMANCE_OUTPUT)

# Inferred options off by default, and advanced
# (i.e. not shown to user)
SET(USE_EPIPHANSDK OFF CACHE BOOL "ON OFF" FORCE)
MARK_AS_ADVANCED(USE_EPIPHANSDK)
SET(USE_FFMPEG OFF CACHE BOOL "ON OFF" FORCE)
MARK_AS_ADVANCED(USE_FFMPEG)
SET(USE_OPENCV OFF CACHE BOOL "ON OFF" FORCE)
MARK_AS_ADVANCED(USE_OPENCV)
SET(USE_LIBVLC OFF CACHE BOOL "ON OFF" FORCE)
MARK_AS_ADVANCED(USE_LIBVLC)

# Check if non-free explicitly enabled for relevant components
if(USE_NVENC AND NOT ENABLE_NONFREE)
    MESSAGE(FATAL_ERROR
        "NVENC (NVIDIA Video Codec SDK) has a proprietary licence: you must explicitly enable non-free (ENABLE_NONFREE=ON) to build GIFT-Grab with NVENC support")
endif(USE_NVENC AND NOT ENABLE_NONFREE)

# Check GPL explicitly enabled for relevant components
if(USE_X265 AND NOT ENABLE_GPL)
    MESSAGE(FATAL_ERROR
        "x265 is a GPL library: you must explicitly enable GPL (ENABLE_GPL=ON) to build GIFT-Grab with x265 support")
endif(USE_X265 AND NOT ENABLE_GPL)

# Check at least one colour space activated if using Epiphan DVI2PCIe Duo
if(USE_EPIPHAN_DVI2PCIE_DUO)
    if(NOT USE_BGRA AND NOT USE_I420)
        MESSAGE(FATAL_ERROR "At least one colour space must be activated to use Epiphan DVI2PCIe Duo")
    endif(NOT USE_BGRA AND NOT USE_I420)
endif(USE_EPIPHAN_DVI2PCIE_DUO)

# Determine whether OpenCV needed
if((USE_EPIPHAN_DVI2PCIE_DUO AND USE_BGRA) OR
    USE_XVID)
    SET(USE_OPENCV ON CACHE BOOL "OFF ON" FORCE)
endif()

if(USE_EPIPHAN_DVI2PCIE_DUO AND USE_I420)
    SET(USE_LIBVLC ON CACHE BOOL "OFF ON" FORCE)
endif(USE_EPIPHAN_DVI2PCIE_DUO AND USE_I420)

# Determine whether FFmpeg needed
if(USE_HEVC OR USE_VP9)
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

# X265
if(USE_X265)
    ADD_DEFINITIONS(-DUSE_X265)
endif(USE_X265)

# Support for I420 (YUV420p) colour space
if(USE_I420)
    ADD_DEFINITIONS(-DUSE_I420)
endif(USE_I420)

# libVLC video source
if(USE_LIBVLC)
    ADD_SUBDIRECTORY(cmake/libvlc)
    LIST(APPEND LIBS ${LIBVLC_LIBRARY})
    ADD_DEFINITIONS(-DUSE_LIBVLC)

    INCLUDE_DIRECTORIES(vlc)
    LIST(APPEND HEADERS vlc/vlc_video_source.h)
    LIST(APPEND SOURCES vlc/vlc_video_source.cpp)
endif(USE_LIBVLC)

# Epiphan SDK video source
if(USE_EPIPHANSDK)
    ADD_SUBDIRECTORY(cmake/epiphansdk)
    INCLUDE_DIRECTORIES(${EpiphanSDK_INCLUDE_DIRS})
    LIST(APPEND LIBS ${EpiphanSDK_LIBS})
    ADD_DEFINITIONS(-DUSE_EPIPHANSDK)

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
    ADD_SUBDIRECTORY(cmake/pythonlibs)
    INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_DIRS})
    ADD_SUBDIRECTORY(cmake/boost.python)
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

    SET(NAME_PYTHON py${NAME})
    ADD_LIBRARY(${NAME_PYTHON} SHARED python/wrapper.cpp)
    TARGET_LINK_LIBRARIES(${NAME_PYTHON} ${Boost_LIBRARIES} ${NAME})
    # no lib prefix
    SET_TARGET_PROPERTIES(${NAME_PYTHON} PROPERTIES PREFIX "")
    LIST(APPEND INSTALLABLES ${NAME_PYTHON})
endif(BUILD_PYTHON)


# Install
INSTALL(
    FILES ${HEADERS}
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include/${GiftGrab_PATH_SUFFIX}
)
INSTALL(
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/cmake/GiftGrabConfig.cmake
    DESTINATION ${CMAKE_INSTALL_RPATH}
)
INSTALL(
    TARGETS ${INSTALLABLES}
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/${GiftGrab_PATH_SUFFIX}
    LIBRARY DESTINATION ${CMAKE_INSTALL_RPATH}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/${GiftGrab_PATH_SUFFIX}
)
