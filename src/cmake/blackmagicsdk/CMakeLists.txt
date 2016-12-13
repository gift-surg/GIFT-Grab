CMAKE_MINIMUM_REQUIRED(VERSION 3.2.0)

if(NOT DEFINED ENV{BlackmagicSDK_DIR})
    MESSAGE(FATAL_ERROR
        "BlackmagicSDK_DIR does not seem to be defined in your environment. This is needed to find Blackmagic Desktop Video SDK components.")
endif(NOT DEFINED ENV{BlackmagicSDK_DIR})

SET(BlackmagicSDK_INCLUDE_DIRS
    $ENV{BlackmagicSDK_DIR}/Linux/include
    CACHE INTERNAL "Blackmagic Desktop Video SDK include dirs"
)

SET(DeckLinkAPI_LIB decklinkapi)
ADD_LIBRARY(${DeckLinkAPI_LIB}
    ${BlackmagicSDK_INCLUDE_DIRS}/DeckLinkAPIDispatch.cpp
)

SET(BlackmagicSDK_LIBS
    ${DeckLinkAPI_LIB}
    CACHE INTERNAL "Blackmagic DeckLink API library compiled from source"
)
