FILE(COPY
    ${CMAKE_SOURCE_DIR}/tests/epiphan/dvi2pcieduo/test_unit.py
    ${CMAKE_SOURCE_DIR}/tests/epiphan/dvi2pcieduo/test_realtime.py
    ${CMAKE_SOURCE_DIR}/tests/epiphan/dvi2pcieduo/conftest.py
    ${CMAKE_SOURCE_DIR}/tests/epiphan/dvi2pcieduo/config
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)
FILE(COPY
    ${CMAKE_SOURCE_DIR}/python/modules/example_epiphan/
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/config
    FILES_MATCHING PATTERN *.yml
)

# to avoid copying pygiftgrab and inspection module
SET(PYTHONPATH
    "${CMAKE_SOURCE_DIR}/tests/utils:${CMAKE_BINARY_DIR}:$ENV{PYTHONPATH}"
)
# to avoid copying epiphan module
SET(PYTHONPATH "${CMAKE_SOURCE_DIR}/python/modules:${PYTHONPATH}")

# set colour space
if(USE_I420)
    SET(COLOUR_SPACE I420)
else(USE_I420)
    SET(COLOUR_SPACE BGR24)
endif(USE_I420)

SET(NAME_TEST Test_Epiphan_DVI2PCIeDuo_SDI)
ADD_TEST(NAME ${NAME_TEST}
    COMMAND py.test --port=SDI --colour-space=${COLOUR_SPACE} test_unit.py
)
SET_TESTS_PROPERTIES(${NAME_TEST}
    PROPERTIES ENVIRONMENT
    "PYTHONPATH=${PYTHONPATH}"
)

SET(NAME_TEST Test_Epiphan_DVI2PCIeDuo_DVI)
ADD_TEST(NAME ${NAME_TEST}
    COMMAND py.test --port=DVI --colour-space=${COLOUR_SPACE} test_unit.py
)
SET_TESTS_PROPERTIES(${NAME_TEST}
    PROPERTIES ENVIRONMENT
    "PYTHONPATH=${PYTHONPATH}"
)

if(NOT USE_I420)
    SET(NAME_TEST Test_Epiphan_DVI2PCIeDuo_RealTime_MultiThreaded_BGR24)
    ADD_TEST(NAME ${NAME_TEST}
        COMMAND py.test --colour-space=BGR24 test_realtime.py
    )
    SET_TESTS_PROPERTIES(${NAME_TEST}
        PROPERTIES ENVIRONMENT
        "PYTHONPATH=${PYTHONPATH}"
    )
else(NOT USE_I420)
    SET(NAME_TEST Test_Epiphan_DVI2PCIeDuo_RealTime_MultiThreaded_I420)
    ADD_TEST(NAME ${NAME_TEST}
        COMMAND py.test --colour-space=I420 test_realtime.py
    )
    SET_TESTS_PROPERTIES(${NAME_TEST}
        PROPERTIES ENVIRONMENT
        "PYTHONPATH=${PYTHONPATH}"
    )
endif(NOT USE_I420)
