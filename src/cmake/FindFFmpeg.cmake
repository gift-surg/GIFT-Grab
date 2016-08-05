# Try to find FFmpeg components. If successful, the following
# variables are set:
#
#  FFmpeg_INCLUDE_DIRS	headers located here
#  FFmpeg_LIBS			link against these

FIND_PACKAGE(PkgConfig REQUIRED)

PKG_SEARCH_MODULE(LIBAVFORMAT REQUIRED libavformat)
PKG_SEARCH_MODULE(LIBAVCODEC REQUIRED libavcodec)
PKG_SEARCH_MODULE(LIBAVUTIL REQUIRED libavutil)
PKG_SEARCH_MODULE(LIBSWSCALE REQUIRED libswscale)

# all include dirs and libs in a single var each
SET(FFmpeg_INCLUDE_DIRS
    ${LIBAVFORMAT_INCLUDE_DIRS}
	${LIBAVCODEC_INCLUDE_DIRS}
	${LIBAVUTIL_INCLUDE_DIRS}
    ${LIBSWSCALE_INCLUDE_DIRS}
)
SET(FFmpeg_LIBS
    ${LIBAVFORMAT_LIBRARIES}
	${LIBAVCODEC_LIBRARIES}
	${LIBAVUTIL_LIBRARIES}
	${LIBSWSCALE_LIBRARIES}
)

# handle the QUIETLY and REQUIRED arguments and set
# FFmpeg_FOUND if all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(
	FFmpeg DEFAULT_MSG
	FFmpeg_LIBS FFmpeg_INCLUDE_DIRS
)
