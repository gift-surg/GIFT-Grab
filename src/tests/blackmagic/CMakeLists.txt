FILE(COPY
    test_unit.py
    test_observer.py
    test_stereo.py
    conftest.py
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)

if(USE_BLACKMAGIC_DECKLINK_SDI_4K)
    LIST(APPEND BLACKMAGIC_DEVICES DeckLinkSDI4K)
endif(USE_BLACKMAGIC_DECKLINK_SDI_4K)
if(USE_BLACKMAGIC_DECKLINK_4K_EXTREME_12G)
    LIST(APPEND BLACKMAGIC_DEVICES DeckLink4KExtreme12G)
endif(USE_BLACKMAGIC_DECKLINK_4K_EXTREME_12G)

# VideoSourceFactory
SET(VIDEO_SOURCE_FACTORY_TEST test_video_source_factory_blackmagic)
ADD_EXECUTABLE(
    ${VIDEO_SOURCE_FACTORY_TEST}
    ${CMAKE_SOURCE_DIR}/tests/blackmagic/test_video_source_factory.cpp
)
TARGET_LINK_LIBRARIES(
    ${VIDEO_SOURCE_FACTORY_TEST}
    ${LIBS} ${NAME}
)

foreach(BLACKMAGIC_DEVICE ${BLACKMAGIC_DEVICES})
    foreach(COLOUR_SPACE ${COLOUR_SPACES})
        if(COLOUR_SPACE STREQUAL I420)
            continue()
        endif(COLOUR_SPACE STREQUAL I420)
        if(COLOUR_SPACE STREQUAL BGRA AND BLACKMAGIC_DEVICE STREQUAL DeckLinkSDI4K)
            continue()
        endif(COLOUR_SPACE STREQUAL BGRA AND BLACKMAGIC_DEVICE STREQUAL DeckLinkSDI4K)

        SET(NAME_TEST Test_Blackmagic_${BLACKMAGIC_DEVICE}_VideoSourceFactory_${COLOUR_SPACE})
        ADD_TEST(NAME ${NAME_TEST} COMMAND ${VIDEO_SOURCE_FACTORY_TEST} ${BLACKMAGIC_DEVICE} ${COLOUR_SPACE})
        LIST(APPEND TESTS_LIST ${NAME_TEST})

        # Blackmagic device on its own
        SET(NAME_TEST Test_Blackmagic_${BLACKMAGIC_DEVICE}_${COLOUR_SPACE})
        ADD_TEST(NAME ${NAME_TEST}
            COMMAND py.test --device=${BLACKMAGIC_DEVICE} --colour-space=${COLOUR_SPACE} test_unit.py
        )
        LIST(APPEND TESTS_LIST ${NAME_TEST})

        # Blackmagic device using the observer design pattern
        SET(NAME_TEST Test_Blackmagic_${BLACKMAGIC_DEVICE}_ObserverPattern_${COLOUR_SPACE})
        if(BLACKMAGIC_DEVICE STREQUAL DeckLink4KExtreme12G)
            SET(FRAME_RATE 24)  # frame rate seems to be reduced, due to stereo?
        else()
            SET(FRAME_RATE 27)
        endif()
        ADD_TEST(NAME ${NAME_TEST}
            COMMAND py.test --device=${BLACKMAGIC_DEVICE} --colour-space=${COLOUR_SPACE} --frame-rate=${FRAME_RATE} --observers=3 test_observer.py
        )
        LIST(APPEND TESTS_LIST ${NAME_TEST})

        # Blackmagic device supporting stereo (3D) video streams
        if(BLACKMAGIC_DEVICE STREQUAL DeckLink4KExtreme12G)
            SET(NAME_TEST Test_Blackmagic_${BLACKMAGIC_DEVICE}_StereoFrames_${COLOUR_SPACE})
            ADD_TEST(NAME ${NAME_TEST}
                COMMAND py.test --device=${BLACKMAGIC_DEVICE} --colour-space=${COLOUR_SPACE} test_stereo.py
            )
            LIST(APPEND TESTS_LIST ${NAME_TEST})
        endif()
    endforeach(COLOUR_SPACE)
endforeach(BLACKMAGIC_DEVICE)

# to avoid copying stuff around
SET_TESTS_PROPERTIES(${TESTS_LIST}
    PROPERTIES ENVIRONMENT "PYTHONPATH=${PYTHONPATH}"
)
