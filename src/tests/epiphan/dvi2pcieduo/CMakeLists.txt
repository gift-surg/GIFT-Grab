FILE(COPY
    ${CMAKE_SOURCE_DIR}/tests/epiphan/dvi2pcieduo/test.py
    ${CMAKE_SOURCE_DIR}/tests/epiphan/dvi2pcieduo/conftest.py
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)

SET(NAME_TEST Test_Epiphan_DVI2PCIeDuo_SDI)
ADD_TEST(NAME ${NAME_TEST}
    COMMAND py.test --port=SDI test.py
)
# to avoid copying pygiftgrab
SET_TESTS_PROPERTIES(${NAME_TEST}
    PROPERTIES ENVIRONMENT
    "PYTHONPATH=${CMAKE_BINARY_DIR}:$ENV{PYTHONPATH}"
)

SET(NAME_TEST Test_Epiphan_DVI2PCIeDuo_DVI)
ADD_TEST(NAME ${NAME_TEST}
    COMMAND py.test --port=DVI test.py
)
# to avoid copying pygiftgrab
SET_TESTS_PROPERTIES(${NAME_TEST}
    PROPERTIES ENVIRONMENT
    "PYTHONPATH=${CMAKE_BINARY_DIR}:$ENV{PYTHONPATH}"
)
