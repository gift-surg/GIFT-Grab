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

# generate tests
foreach(CODEC ${CODECS})
    foreach(COLOUR_SPACE ${COLOUR_SPACES})
        if(CODEC STREQUAL Xvid AND COLOUR_SPACE STREQUAL I420)
            # NOP: see #90
        else(CODEC STREQUAL Xvid AND COLOUR_SPACE STREQUAL I420)
            SET(NAME_TEST Test_${CODEC}_${COLOUR_SPACE})
            ADD_TEST(NAME ${NAME_TEST}
                COMMAND py.test --codec=${CODEC} --colour-space=${COLOUR_SPACE} test_unit.py
            )
            LIST(APPEND TESTS_LIST ${NAME_TEST})
        endif(CODEC STREQUAL Xvid AND COLOUR_SPACE STREQUAL I420)
    endforeach(COLOUR_SPACE)
endforeach(CODEC)

# to avoid copying stuff around
SET_TESTS_PROPERTIES(${TESTS_LIST}
    PROPERTIES ENVIRONMENT "PYTHONPATH=${PYTHONPATH}"
)
