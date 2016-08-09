# we do not officially support libVLC yet!
SET(USE_LIBVLC OFF CACHE BOOL "ON OFF" FORCE)

# libVLC
if(USE_LIBVLC)
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
