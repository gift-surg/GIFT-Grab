FILE(COPY
    ${CMAKE_SOURCE_DIR}/tests/files/test_observer.py
    ${CMAKE_SOURCE_DIR}/tests/files/conftest.py
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)

# list codecs
if(USE_HEVC)
    LIST(APPEND CODECS HEVC)
endif(USE_HEVC)
if(USE_XVID)
    LIST(APPEND CODECS Xvid)
endif(USE_XVID)
if(USE_VP9)
    LIST(APPEND CODECS VP9)
endif(USE_VP9)

# VideoSourceFactory
SET(VIDEO_SOURCE_FACTORY_TEST test_video_source_factory_files)
ADD_EXECUTABLE(
    ${VIDEO_SOURCE_FACTORY_TEST} test_video_source_factory.cpp
)
TARGET_LINK_LIBRARIES(
    ${VIDEO_SOURCE_FACTORY_TEST} ${LIBS} ${NAME}
)

# generate tests
SET(FILEPATH /this/file/does/not/exist.video)
SET(FRAME_RATE 30)
SET(FRAME_COUNT 5)
SET(FRAME_WIDTH 1920)
SET(FRAME_HEIGHT 1080)
foreach(CODEC ${CODECS})
    foreach(COLOUR_SPACE ${COLOUR_SPACES})
        SET(NAME_TEST Test_Files_ObserverPattern_${CODEC}_${COLOUR_SPACE})
        ADD_TEST(NAME ${NAME_TEST}
            COMMAND py.test --codec=${CODEC} --colour-space=${COLOUR_SPACE}
            --filepath=${FILEPATH}
            --frame-rate=${FRAME_RATE} --frame-count=${FRAME_COUNT}
            --frame-width=${FRAME_WIDTH} --frame-height=${FRAME_HEIGHT}
            test_observer.py
        )
        LIST(APPEND TESTS_LIST ${NAME_TEST})

        SET(NAME_TEST Test_VideoSourceFactory_Files_${CODEC}_${COLOUR_SPACE})
        ADD_TEST(NAME ${NAME_TEST}
            COMMAND ${VIDEO_SOURCE_FACTORY_TEST} ${CODEC} ${COLOUR_SPACE}
            ${FILEPATH}
            ${FRAME_RATE} ${FRAME_COUNT}
            ${FRAME_WIDTH} ${FRAME_HEIGHT}
        )
        LIST(APPEND TESTS_LIST ${NAME_TEST})
    endforeach(COLOUR_SPACE)
endforeach(CODEC)

# to avoid copying stuff around
SET_TESTS_PROPERTIES(${TESTS_LIST}
    PROPERTIES ENVIRONMENT "PYTHONPATH=${PYTHONPATH}"
)
