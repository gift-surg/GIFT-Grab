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

SET(NAME_TEST Test_Epiphan_DVI2PCIeDuo_SDI)
ADD_TEST(NAME ${NAME_TEST}
    COMMAND py.test --port=SDI test_unit.py
)
SET_TESTS_PROPERTIES(${NAME_TEST}
    PROPERTIES ENVIRONMENT
    "PYTHONPATH=${PYTHONPATH}"
)

SET(NAME_TEST Test_Epiphan_DVI2PCIeDuo_DVI)
ADD_TEST(NAME ${NAME_TEST}
    COMMAND py.test --port=DVI test_unit.py
)
SET_TESTS_PROPERTIES(${NAME_TEST}
    PROPERTIES ENVIRONMENT
    "PYTHONPATH=${PYTHONPATH}"
)

SET(NAME_TEST Test_Epiphan_DVI2PCIeDuo_RealTime_MultiThreaded_BGR24)
ADD_TEST(NAME ${NAME_TEST}
    COMMAND py.test --colour-space=BGR24 test_realtime.py
)
SET_TESTS_PROPERTIES(${NAME_TEST}
    PROPERTIES ENVIRONMENT
    "PYTHONPATH=${PYTHONPATH}"
)

SET(NAME_TEST Test_Epiphan_DVI2PCIeDuo_RealTime_MultiThreaded_I420)
ADD_TEST(NAME ${NAME_TEST}
    COMMAND py.test --colour-space=I420 test_realtime.py
)
SET_TESTS_PROPERTIES(${NAME_TEST}
    PROPERTIES ENVIRONMENT
    "PYTHONPATH=${PYTHONPATH}"
)
