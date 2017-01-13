FILE(COPY
    ${CMAKE_SOURCE_DIR}/tests/target/test_unit.py
    ${CMAKE_SOURCE_DIR}/tests/target/conftest.py
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

# VideoTargetFactory
SET(VIDEO_TARGET_FACTORY_TEST test_video_target_factory)
ADD_EXECUTABLE(
    ${VIDEO_TARGET_FACTORY_TEST}
    test_video_target_factory.cpp
)
TARGET_LINK_LIBRARIES(
    ${VIDEO_TARGET_FACTORY_TEST}
    ${LIBS} ${NAME}
)

# generate tests
foreach(CODEC ${CODECS})
    foreach(COLOUR_SPACE ${COLOUR_SPACES})
        if(CODEC STREQUAL Xvid)
            if(COLOUR_SPACE STREQUAL I420 OR COLOUR_SPACE STREQUAL UYVY)
                continue()
                # NOP: see #90
            endif(COLOUR_SPACE STREQUAL I420 OR COLOUR_SPACE STREQUAL UYVY)
        endif(CODEC STREQUAL Xvid)

        SET(NAME_TEST Test_${CODEC}_${COLOUR_SPACE})
        ADD_TEST(NAME ${NAME_TEST}
            COMMAND py.test --codec=${CODEC} --colour-space=${COLOUR_SPACE} test_unit.py
        )
        LIST(APPEND TESTS_LIST ${NAME_TEST})

        SET(NAME_TEST Test_VideoTargetFactory_${CODEC}_${COLOUR_SPACE})
        ADD_TEST(NAME ${NAME_TEST}
            COMMAND ${VIDEO_TARGET_FACTORY_TEST} ${CODEC} ${COLOUR_SPACE}
        )
        LIST(APPEND TESTS_LIST ${NAME_TEST})
    endforeach(COLOUR_SPACE)
endforeach(CODEC)

# to avoid copying stuff around
SET_TESTS_PROPERTIES(${TESTS_LIST}
    PROPERTIES ENVIRONMENT "PYTHONPATH=${PYTHONPATH}"
)
