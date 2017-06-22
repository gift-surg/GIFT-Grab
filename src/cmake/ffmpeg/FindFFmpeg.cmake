# Try to find FFmpeg components. If successful, the following
# variables are set:
#
#  FFmpeg_INCLUDE_DIRS	headers located here
#  FFmpeg_LIBS			link against these
#  FFmpeg_LIBRARY_DIRS  for CMake's LINK_DIRECTORIES

FIND_PACKAGE(PkgConfig REQUIRED)

PKG_SEARCH_MODULE(LIBAVFORMAT REQUIRED libavformat)
PKG_SEARCH_MODULE(LIBAVCODEC REQUIRED libavcodec)
PKG_SEARCH_MODULE(LIBAVUTIL REQUIRED libavutil)
PKG_SEARCH_MODULE(LIBSWSCALE REQUIRED libswscale)
PKG_SEARCH_MODULE(LIBAVFILTER REQUIRED libavfilter)

# all include dirs and libs in a single var each
SET(FFmpeg_INCLUDE_DIRS
    ${LIBAVFORMAT_INCLUDE_DIRS}
    ${LIBAVCODEC_INCLUDE_DIRS}
    ${LIBAVUTIL_INCLUDE_DIRS}
    ${LIBSWSCALE_INCLUDE_DIRS}
    ${LIBAVFILTER_INCLUDE_DIRS}
)
SET(FFmpeg_LIBRARY_DIRS
    ${LIBAVFORMAT_LIBRARY_DIRS}
    ${LIBAVCODEC_LIBRARY_DIRS}
    ${LIBAVUTIL_LIBRARY_DIRS}
    ${LIBSWSCALE_LIBRARY_DIRS}
    ${LIBAVFILTER_LIBRARY_DIRS}
)
SET(FFmpeg_LIBS
    ${LIBAVFORMAT_LIBRARIES}
    ${LIBAVCODEC_LIBRARIES}
    ${LIBAVUTIL_LIBRARIES}
    ${LIBSWSCALE_LIBRARIES}
    ${LIBAVFILTER_LIBRARIES}
)

# handle the QUIETLY and REQUIRED arguments and set
# FFmpeg_FOUND if all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(
    FFmpeg DEFAULT_MSG
    FFmpeg_LIBS
)

# excluded FFmpeg_INCLUDE_DIRS and FFmpeg_LIBRARY_DIRS
# from the above check, because pkg-config does not
# report them if FFmpeg libraries are installed in
# standard system-wide locations already known to the
# system compiler
