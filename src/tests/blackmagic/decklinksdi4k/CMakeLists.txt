FILE(COPY
    test_unit.py
    test_observer.py
    conftest.py
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)

# VideoSourceFactory
SET(VIDEO_SOURCE_FACTORY_TEST test_video_source_factory_blackmagic_decklinksdi4k)
ADD_EXECUTABLE(
    ${VIDEO_SOURCE_FACTORY_TEST}
    ${CMAKE_SOURCE_DIR}/tests/blackmagic/decklinksdi4k/test_video_source_factory.cpp
)
TARGET_LINK_LIBRARIES(
    ${VIDEO_SOURCE_FACTORY_TEST}
    ${LIBS} ${NAME}
)
SET(COLOUR_SPACE UYVY)

SET(NAME_TEST Test_Blackmagic_DeckLinkSDI4K_VideoSourceFactory_${COLOUR_SPACE})
ADD_TEST(NAME ${NAME_TEST} COMMAND ${VIDEO_SOURCE_FACTORY_TEST})
LIST(APPEND TESTS_LIST ${NAME_TEST})

# Blackmagic DeckLink SDI 4K on its own
SET(NAME_TEST Test_Blackmagic_DeckLinkSDI4K_${COLOUR_SPACE})
ADD_TEST(NAME ${NAME_TEST}
    COMMAND py.test --colour-space=${COLOUR_SPACE} test_unit.py
)
LIST(APPEND TESTS_LIST ${NAME_TEST})

# Blackmagic DeckLink SDI 4K using the observer design pattern
SET(NAME_TEST Test_Blackmagic_DeckLinkSDI4K_ObserverPattern_${COLOUR_SPACE})
SET(FRAME_RATE 27)
ADD_TEST(NAME ${NAME_TEST}
    COMMAND py.test --colour-space=${COLOUR_SPACE} --frame-rate=${FRAME_RATE} --observers=3 test_observer.py
)
LIST(APPEND TESTS_LIST ${NAME_TEST})

# to avoid copying stuff around
SET_TESTS_PROPERTIES(${TESTS_LIST}
    PROPERTIES ENVIRONMENT "PYTHONPATH=${PYTHONPATH}"
)
