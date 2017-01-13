FILE(COPY
    test_unit.py
    test_observer.py
    conftest.py
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)

# If user has not defined network source delay
if(NOT DEFINED TESTING_NETWORK_SOURCE_DELAY)
    SET(TESTING_NETWORK_SOURCE_DELAY 0.0)
endif(NOT DEFINED TESTING_NETWORK_SOURCE_DELAY)

# VideoSourceFactory
SET(VIDEO_SOURCE_FACTORY_TEST test_video_source_factory_network_source)
ADD_EXECUTABLE(
    ${VIDEO_SOURCE_FACTORY_TEST} test_video_source_factory.cpp
)
TARGET_LINK_LIBRARIES(
    ${VIDEO_SOURCE_FACTORY_TEST}
    ${LIBS} ${NAME}
)
foreach(COLOUR_SPACE ${COLOUR_SPACES})
    if(COLOUR_SPACE STREQUAL UYVY)
        continue()
    endif(COLOUR_SPACE STREQUAL UYVY)

    SET(NAME_TEST Test_Network_VideoSourceFactory_${COLOUR_SPACE})
    ADD_TEST(NAME ${NAME_TEST}
        COMMAND ${VIDEO_SOURCE_FACTORY_TEST} ${TESTING_NETWORK_SOURCE_ADDRESS} ${COLOUR_SPACE} ${TESTING_NETWORK_SOURCE_DELAY}
    )
    LIST(APPEND TESTS_LIST ${NAME_TEST})
endforeach(COLOUR_SPACE)

# The actual network source
foreach(COLOUR_SPACE ${COLOUR_SPACES})
    if(COLOUR_SPACE STREQUAL UYVY)
        continue()
    endif(COLOUR_SPACE STREQUAL UYVY)

    SET(NAME_TEST Test_Network_${COLOUR_SPACE})
    ADD_TEST(NAME ${NAME_TEST}
        COMMAND py.test --address=${TESTING_NETWORK_SOURCE_ADDRESS} --colour-space=${COLOUR_SPACE} --init-delay=${TESTING_NETWORK_SOURCE_DELAY} test_unit.py
    )
    LIST(APPEND TESTS_LIST ${NAME_TEST})
endforeach(COLOUR_SPACE)

# Network source using the observer pattern
foreach(COLOUR_SPACE ${COLOUR_SPACES})
    if(COLOUR_SPACE STREQUAL UYVY)
        continue()
    endif(COLOUR_SPACE STREQUAL UYVY)

    SET(NAME_TEST Test_Network_ObserverPattern_${COLOUR_SPACE}_${TESTING_NETWORK_SOURCE_FRAME_RATE}fps)

    ADD_TEST(NAME ${NAME_TEST}
        COMMAND py.test --address=${TESTING_NETWORK_SOURCE_ADDRESS} --colour-space=${COLOUR_SPACE} --frame-rate=${TESTING_NETWORK_SOURCE_FRAME_RATE} --observers=3 --init-delay=${TESTING_NETWORK_SOURCE_DELAY} test_observer.py
    )
    LIST(APPEND TESTS_LIST ${NAME_TEST})
endforeach(COLOUR_SPACE)

# to avoid copying stuff around
SET_TESTS_PROPERTIES(${TESTS_LIST}
    PROPERTIES ENVIRONMENT "PYTHONPATH=${PYTHONPATH}"
)
