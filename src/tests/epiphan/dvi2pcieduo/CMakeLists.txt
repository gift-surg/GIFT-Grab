FILE(COPY
    ${CMAKE_SOURCE_DIR}/python/modules/epiphan.py
    ${CMAKE_SOURCE_DIR}/tests/epiphan/dvi2pcieduo/test.py
    ${CMAKE_SOURCE_DIR}/tests/epiphan/dvi2pcieduo/conftest.py
    ${CMAKE_SOURCE_DIR}/tests/epiphan/config
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)
FILE(COPY
    ${CMAKE_SOURCE_DIR}/python/modules/example_epiphan/
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/config
    FILES_MATCHING PATTERN *.yml
)

SET(NAME_TEST Test_EpiphanDVI2PCIeDuo_BGR24)
ADD_TEST(NAME ${NAME_TEST}
    COMMAND py.test --colour-space=BGR24 test.py
)
# to avoid copying pygiftgrab
SET_TESTS_PROPERTIES(${NAME_TEST}
    PROPERTIES ENVIRONMENT
    "PYTHONPATH=${CMAKE_BINARY_DIR}:$ENV{PYTHONPATH}"
)
