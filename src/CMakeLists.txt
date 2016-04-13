PROJECT(GiftGrab)
CMAKE_MINIMUM_REQUIRED(VERSION 3.2.0)

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
OPTION(USE_FILES "Support for loading video from files" OFF)
OPTION(USE_EPIPHAN "Support for streaming video using Epiphan framegrabbers" OFF)
OPTION(USE_BLACKMAGIC "Support for streaming video using BlackMagic framegrabbers" OFF)
OPTION(USE_FFMPEG "Support for saving video to files using FFmpeg" OFF)

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
if(USE_FILES OR USE_EPIPHAN)
    SET(USE_OPENCV ON CACHE BOOL "OFF ON" FORCE)
endif(USE_FILES OR USE_EPIPHAN)

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
    INCLUDE_DIRECTORIES(${FFmpeg_DIR}/include)
    INCLUDE_DIRECTORIES(ffmpeg)
    LIST(APPEND HEADERS ffmpeg/ffmpeg_video_target.h)
    LIST(APPEND SOURCES ffmpeg/ffmpeg_video_target.cpp)
    LIST(APPEND LIBS
        ${FFmpeg_DIR}/lib/libavformat.so
        ${FFmpeg_DIR}/lib/libavcodec.so
        ${FFmpeg_DIR}/lib/libavutil.so
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

# Compile and link
ADD_LIBRARY(${NAME} ${HEADERS} ${SOURCES})
TARGET_LINK_LIBRARIES(${NAME} ${LIBS})

# Install
SET(GiftGrab_PATH_SUFFIX giftgrab)
INSTALL(
    FILES ${HEADERS} cmake/GiftGrabConfig.cmake
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include/${GiftGrab_PATH_SUFFIX}
    )
INSTALL(TARGETS ${NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/${GiftGrab_PATH_SUFFIX}
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/${GiftGrab_PATH_SUFFIX}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/${GiftGrab_PATH_SUFFIX}
    )

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

OPTION(BUILD_TESTS "Build tests" OFF)
if(BUILD_TESTS)
    if(USE_EPIPHAN)
        SET(TEST_NAME save_1min_epiphan)
        ADD_EXECUTABLE(${TEST_NAME}
            ${CMAKE_CURRENT_SOURCE_DIR}/tests/save_1min_epiphan.cpp)
TARGET_LINK_LIBRARIES(${TEST_NAME} ${NAME} ${LIBS})
    endif(USE_EPIPHAN)
endif(BUILD_TESTS)
