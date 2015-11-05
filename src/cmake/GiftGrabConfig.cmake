# Try to find GiftGrab components. If successful, the following
# variables are set:
#
#  GiftGrab_INCLUDE_DIRS	headers located here
#  GiftGrab_LIBS			link against these

# to be kept in sync with CMakeLists.txt at top level
# allows defined suffix to be appended to all searched paths
SET(GiftGrab_PATH_SUFFIX giftgrab)

# include dir
FIND_PATH(GiftGrab_INCLUDE_DIR
	NAMES ivideosource.h
	PATHS ${CMAKE_CURRENT_LIST_DIR}/../../include
	PATH_SUFFIXES ${GiftGrab_PATH_SUFFIX}
	)

# libs
FIND_LIBRARY(GiftGrab_LIB
	NAMES giftgrab
	PATHS ${CMAKE_CURRENT_LIST_DIR}/../../lib
	PATH_SUFFIXES ${GiftGrab_PATH_SUFFIX}
	)

# all include dirs and libs in a single var each
SET(GiftGrab_INCLUDE_DIRS
	${GiftGrab_INCLUDE_DIR}
	)
SET(GiftGrab_LIBS
	${GiftGrab_LIB}
	)

# handle the QUIETLY and REQUIRED arguments and set
# GiftGrab_FOUND if all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(
	GiftGrab DEFAULT_MSG
	GiftGrab_LIBS GiftGrab_INCLUDE_DIRS
	)