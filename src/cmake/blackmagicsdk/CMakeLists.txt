CMAKE_MINIMUM_REQUIRED(VERSION 3.2.0)

if(NOT DEFINED ENV{BlackmagicSDK_DIR})
    MESSAGE(FATAL_ERROR
        "BlackmagicSDK_DIR does not seem to be defined in your environment. This is needed to find Blackmagic Desktop Video SDK components.")
endif(NOT DEFINED ENV{BlackmagicSDK_DIR})

FIND_LIBRARY(DeckLinkAPI_LIB
    NAMES DeckLinkAPI
    PATHS /usr/lib
)
if(NOT DeckLinkAPI_LIB)
    MESSAGE(FATAL_ERROR
        "Blackmagic DeckLink drivers do not seem to be installed on your system. They are needed for capturing video using a Blackmagic card.")
endif(NOT DeckLinkAPI_LIB)

SET(BlackmagicSDK_INCLUDE_DIRS
    $ENV{BlackmagicSDK_DIR}/Linux/include
    CACHE INTERNAL "Blackmagic Desktop Video SDK include dirs"
)

SET(BlackmagicSDK_LIBS
    ${DeckLinkAPI_LIB}
    CACHE INTERNAL "Blackmagic Desktop Video SDK libs to link against"
)
