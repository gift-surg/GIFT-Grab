# currently a very limited test suite, see issue #153
if(USE_NUMPY AND USE_BGRA AND USE_XVID)
    FILE(COPY
        ${CMAKE_SOURCE_DIR}/tests/pipeline/test_observer.py
        ${CMAKE_SOURCE_DIR}/tests/pipeline/conftest.py
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
    )

    SET(FILEPATH ${CMAKE_SOURCE_DIR}/tests/pipeline/data/video_15frames_30fps.avi)
    SET(NAME_TEST Test_Pipeline_ObserverPattern_NumPy_BGRA)
    ADD_TEST(NAME ${NAME_TEST}
        COMMAND py.test --filepath=${FILEPATH} test_observer.py
    )
    LIST(APPEND TESTS_LIST ${NAME_TEST})

    # to avoid copying stuff around
    SET_TESTS_PROPERTIES(${TESTS_LIST}
        PROPERTIES ENVIRONMENT "PYTHONPATH=${PYTHONPATH}"
    )
endif(USE_NUMPY AND USE_BGRA AND USE_XVID)
