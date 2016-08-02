FILE(COPY
    ${CMAKE_SOURCE_DIR}/tests/target/test.py
    ${CMAKE_SOURCE_DIR}/tests/target/conftest.py
    ${CMAKE_SOURCE_DIR}/tests/utils/inspection.py
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)

SET(NAME_TEST Test_H265)
ADD_TEST(NAME ${NAME_TEST}
    COMMAND py.test --codec=H265 test.py
)
# to avoid copying pygiftgrab
SET_TESTS_PROPERTIES(${NAME_TEST}
    PROPERTIES ENVIRONMENT
    "PYTHONPATH=${CMAKE_BINARY_DIR}:$ENV{PYTHONPATH}"
)
