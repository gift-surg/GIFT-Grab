# generate tests
if(USE_NUMPY)
    FILE(COPY
        ${CMAKE_SOURCE_DIR}/tests/videoframe/test_numpy_compatibility.py
        ${CMAKE_SOURCE_DIR}/tests/videoframe/conftest.py
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
    )

    foreach(COLOUR_SPACE ${COLOUR_SPACES})
        SET(NAME_TEST Test_VideoFrame_NumPy_${COLOUR_SPACE})
        ADD_TEST(NAME ${NAME_TEST}
            COMMAND py.test --colour-space=${COLOUR_SPACE} test_numpy_compatibility.py
        )
        LIST(APPEND TESTS_LIST ${NAME_TEST})
    endforeach(COLOUR_SPACE)

    # to avoid copying stuff around
    SET_TESTS_PROPERTIES(${TESTS_LIST}
        PROPERTIES ENVIRONMENT "PYTHONPATH=${PYTHONPATH}"
    )
endif(USE_NUMPY)
