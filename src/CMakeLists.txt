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

# OpenCV
FIND_PACKAGE(OpenCV REQUIRED)
if(OpenCV_FOUND)
    ADD_DEFINITIONS(-DUSE_OPENCV)
else(OpenCV_FOUND)
    SET(USE_FILES OFF CACHE BOOL "ON OFF" FORCE)
    SET(USE_BLACKMAGIC OFF CACHE BOOL "ON OFF" FORCE)
    MESSAGE(FATAL_ERROR "OpenCV could not be found.")
endif(OpenCV_FOUND)

# Library name
SET(NAME giftgrab)

# Library include dirs
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/api)

# Variable for header and source files
SET(HEADERS
    api/ivideosource.h
    api/videoframe.h
    api/maskframe.h
    )
SET(SOURCES
    api/videoframe.cpp
    api/maskframe.cpp
    )
SET(LIBS)

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