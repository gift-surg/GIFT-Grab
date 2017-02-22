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
OPTION(USE_NETWORK_SOURCES
    "Capture video from network sources" OFF)
OPTION(USE_BLACKMAGIC_DECKLINK_SDI_4K
    "Capture video with Blackmagic DeckLink SDI 4K" OFF)
OPTION(USE_FILES
    "Support for reading video files" OFF)
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
OPTION(USE_NUMPY
    "Build support for exposing video data as NumPy datatypes (requires Python, Boost.Python and Boost.NumPy)" OFF)
OPTION(USE_BGRA
    "Capture video in BGRA colour space (requires OpenCV)" ON)
OPTION(USE_I420
    "Capture video in I420 colour space (requires libVLC)" ON)
OPTION(USE_UYVY
    "Capture video in UYVY colour space (currently only for Blackmagic DeckLink SDI 4K)" ON)
OPTION(USE_EPIPHANSDK
    "Use Epiphan video grabbing SDK (requires zlib and ptreads)" OFF)
OPTION(GENERATE_PERFORMANCE_OUTPUT
    "Performance benchmark reports (requires Boost.Timer and Boost.System)" OFF)
MARK_AS_ADVANCED(GENERATE_PERFORMANCE_OUTPUT)

# Inferred options off by default, and advanced
# (i.e. not shown to user)
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
if(USE_EPIPHANSDK AND NOT ENABLE_NONFREE)
    MESSAGE(FATAL_ERROR
        "Epiphan video grabbing SDK has a proprietary licence: you must explicitly enable non-free (ENABLE_NONFREE=ON) to build GIFT-Grab with Epiphan video grabbing SDK support.")
endif(USE_EPIPHANSDK AND NOT ENABLE_NONFREE)
if(USE_BLACKMAGIC_DECKLINK_SDI_4K AND NOT ENABLE_NONFREE)
    MESSAGE(FATAL_ERROR
        "Blackmagic Desktop Video SDK has a proprietary licence: you must explicitly enable non-free (ENABLE_NONFREE=ON) to build GIFT-Grab with Blackmagic DeckLink SDI 4K support")
endif(USE_BLACKMAGIC_DECKLINK_SDI_4K AND NOT ENABLE_NONFREE)

# Check GPL explicitly enabled for relevant components
if(USE_X265 AND NOT ENABLE_GPL)
    MESSAGE(FATAL_ERROR
        "x265 is a GPL library: you must explicitly enable GPL (ENABLE_GPL=ON) to build GIFT-Grab with x265 support")
endif(USE_X265 AND NOT ENABLE_GPL)

# GPL and non-free are not compatible
if(ENABLE_GPL AND ENABLE_NONFREE)
    MESSAGE(FATAL_ERROR
        "GPL and non-free are not compatible options, and so GIFT-Grab cannot be built with both")
elseif(ENABLE_GPL)
    MESSAGE(WARNING
        "You have upgraded the GIFT-Grab licence to GPL. This means if you re-distribute the GIFT-Grab code, you must distribute it using the GPL licence.")
elseif(ENABLE_NONFREE)
    MESSAGE(WARNING
        "You have enabled one or more non-free components of GIFT-Grab. This means that GIFT-Grab cannot be re-distributed.")
endif(ENABLE_GPL AND ENABLE_NONFREE)

# Check at least one colour space activated if using Epiphan DVI2PCIe Duo
if(USE_EPIPHAN_DVI2PCIE_DUO)
    if(NOT USE_BGRA AND NOT USE_I420)
        MESSAGE(FATAL_ERROR "At least one colour space must be activated to use Epiphan DVI2PCIe Duo")
    endif(NOT USE_BGRA AND NOT USE_I420)
endif(USE_EPIPHAN_DVI2PCIE_DUO)

# Check at least one colour space activated if using network sources
if(USE_NETWORK_SOURCES)
    if(NOT USE_BGRA AND NOT USE_I420)
        MESSAGE(FATAL_ERROR "At least one colour space must be activated to use network sources")
    endif(NOT USE_BGRA AND NOT USE_I420)
endif(USE_NETWORK_SOURCES)

# Check at least one colour space activated if using Blackmagic DeckLink SDI 4K
if(USE_BLACKMAGIC_DECKLINK_SDI_4K)
    if(NOT USE_UYVY)
        MESSAGE(FATAL_ERROR "UYVY colour space must be activated to use Blackmagic DeckLink SDI 4K")
    endif(NOT USE_UYVY)
endif(USE_BLACKMAGIC_DECKLINK_SDI_4K)

# Determine whether OpenCV needed
if(((USE_EPIPHAN_DVI2PCIE_DUO OR USE_NETWORK_SOURCES) AND USE_BGRA) OR
    USE_XVID)
    SET(USE_OPENCV ON CACHE BOOL "OFF ON" FORCE)
endif()

# Determine whether VLC needed
if(USE_EPIPHAN_DVI2PCIE_DUO AND USE_I420)
    if(NOT USE_EPIPHANSDK)
        SET(USE_LIBVLC ON CACHE BOOL "OFF ON" FORCE)
    endif(NOT USE_EPIPHANSDK)
endif(USE_EPIPHAN_DVI2PCIE_DUO AND USE_I420)
if(USE_NETWORK_SOURCES AND USE_I420)
    SET(USE_LIBVLC ON CACHE BOOL "OFF ON" FORCE)
endif(USE_NETWORK_SOURCES AND USE_I420)

# Determine if Blackmagic Desktop Video SDK needed
if(USE_BLACKMAGIC_DECKLINK_SDI_4K)
    SET(USE_BLACKMAGICSDK ON CACHE BOOL "OFF ON" FORCE)
endif(USE_BLACKMAGIC_DECKLINK_SDI_4K)

# Determine whether FFmpeg needed
if(USE_HEVC OR USE_VP9 OR USE_FILES)
    SET(USE_FFMPEG ON CACHE BOOL "OFF ON" FORCE)
endif()

# Check whether USE_NUMPY specified with BUILD_PYTHON
if(USE_NUMPY AND NOT BUILD_PYTHON)
    MESSAGE(FATAL_ERROR "USE_NUMPY can only be activated together with BUILD_PYTHON")
endif(USE_NUMPY AND NOT BUILD_PYTHON)

# Library name
SET(NAME giftgrab)

# Library include dirs
INCLUDE_DIRECTORIES(api)
INCLUDE_DIRECTORIES(utils)

# Variable for header and source files
SET(HEADERS
    api/ivideosource.h
    api/iobservable.h
    api/ivideotarget.h
    api/iobserver.h
    api/broadcastdaemon.h
    api/videoframe.h
    api/maskframe.h
    api/device.h
    api/codec.h
    api/videosourcefactory.h
    api/videotargetfactory.h
    api/except.h
)
SET(SOURCES
    api/iobservable.cpp
    api/videoframe.cpp
    api/broadcastdaemon.cpp
    api/maskframe.cpp
    api/videosourcefactory.cpp
    api/videotargetfactory.cpp
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
    LIST(APPEND HEADERS ffmpeg/ffmpeg_video_source.h)
    LIST(APPEND SOURCES ffmpeg/ffmpeg_video_source.cpp)
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
    ADD_DEFINITIONS(-DEpiphan_DVI2PCIeDuo_DVI=${Epiphan_DVI2PCIeDuo_DVI})
    ADD_DEFINITIONS(-DEpiphan_DVI2PCIeDuo_SDI=${Epiphan_DVI2PCIeDuo_SDI})
    ADD_DEFINITIONS(-DEpiphan_DVI2PCIeDuo_DVI_MAX_FRAME_RATE=${Epiphan_DVI2PCIeDuo_DVI_MAX_FRAME_RATE})
    ADD_DEFINITIONS(-DEpiphan_DVI2PCIeDuo_SDI_MAX_FRAME_RATE=${Epiphan_DVI2PCIeDuo_SDI_MAX_FRAME_RATE})

    INCLUDE_DIRECTORIES(epiphansdk)
    LIST(APPEND HEADERS epiphansdk/epiphansdk_video_source.h)
    LIST(APPEND SOURCES epiphansdk/epiphansdk_video_source.cpp)
endif(USE_EPIPHANSDK)

# Blackmagic SDK video source
if(USE_BLACKMAGICSDK)
    ADD_SUBDIRECTORY(cmake/blackmagicsdk)
    INCLUDE_DIRECTORIES(${BlackmagicSDK_INCLUDE_DIRS})
    LIST(APPEND LIBS ${BlackmagicSDK_LIBS})
    ADD_DEFINITIONS(-DUSE_BLACKMAGICSDK)

    INCLUDE_DIRECTORIES(blackmagicsdk)
    LIST(APPEND HEADERS blackmagicsdk/blackmagicsdk_video_source.h)
    LIST(APPEND SOURCES blackmagicsdk/blackmagicsdk_video_source.cpp)
    LIST(APPEND HEADERS blackmagicsdk/deck_link_display_mode_detector.h)
    LIST(APPEND SOURCES blackmagicsdk/deck_link_display_mode_detector.cpp)
endif(USE_BLACKMAGICSDK)

# Performance benchmarking
if(GENERATE_PERFORMANCE_OUTPUT)
    FIND_PACKAGE(Boost COMPONENTS timer system REQUIRED)
    LIST(APPEND LIBS ${Boost_LIBRARIES})
    ADD_DEFINITIONS(-DGENERATE_PERFORMANCE_OUTPUT)
endif(GENERATE_PERFORMANCE_OUTPUT)

# Need to link against Python and Boost.Python
# libs due to Python GIL
if(BUILD_PYTHON)
    ADD_SUBDIRECTORY(cmake/pythonlibs)
    INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_DIRS})
    ADD_SUBDIRECTORY(cmake/boost.python)
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
    LIST(APPEND LIBS ${PYTHON_LIBRARIES} ${Boost_LIBRARIES})

    ADD_DEFINITIONS(-DBUILD_PYTHON)
    if(USE_NUMPY)
        ADD_DEFINITIONS(-DUSE_NUMPY)
    endif(USE_NUMPY)
    INCLUDE_DIRECTORIES(python)
endif(BUILD_PYTHON)

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

    INCLUDE_DIRECTORIES(tests)
    SUBDIRS(tests)
endif(BUILD_TESTS)

# Python wrappers
if(BUILD_PYTHON)
    SET(NAME_PYTHON py${NAME})
    ADD_LIBRARY(${NAME_PYTHON} SHARED python/gil.h python/wrapper.cpp)
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
