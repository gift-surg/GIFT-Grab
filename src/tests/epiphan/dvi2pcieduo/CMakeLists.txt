FILE(COPY
    ${CMAKE_SOURCE_DIR}/tests/epiphan/dvi2pcieduo/test_unit.py
    ${CMAKE_SOURCE_DIR}/tests/epiphan/dvi2pcieduo/test_realtime.py
    ${CMAKE_SOURCE_DIR}/tests/epiphan/dvi2pcieduo/conftest.py
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)
SET(CONFIG_DIR ${CMAKE_SOURCE_DIR}/python/modules/data/epiphan)

# SDI port on its own
SET(NAME_TEST Test_Epiphan_DVI2PCIeDuo_SDI)
ADD_TEST(NAME ${NAME_TEST}
    COMMAND py.test --port=SDI --colour-space=${COLOUR_SPACE} test_unit.py
)
LIST(APPEND TESTS_LIST ${NAME_TEST})

# DVI port on its own
SET(NAME_TEST Test_Epiphan_DVI2PCIeDuo_DVI)
ADD_TEST(NAME ${NAME_TEST}
    COMMAND py.test --port=DVI --colour-space=${COLOUR_SPACE} test_unit.py
)
LIST(APPEND TESTS_LIST ${NAME_TEST})

# Both ports in real-time
SET(NAME_TEST Test_Epiphan_DVI2PCIeDuo_RealTime_${COLOUR_SPACE})
ADD_TEST(NAME ${NAME_TEST}
    COMMAND py.test --colour-space=${COLOUR_SPACE} --config-dir=${CONFIG_DIR} test_realtime.py
)
LIST(APPEND TESTS_LIST ${NAME_TEST})

# to avoid copying stuff around
SET_TESTS_PROPERTIES(${TESTS_LIST}
    PROPERTIES ENVIRONMENT "PYTHONPATH=${PYTHONPATH}"
)
