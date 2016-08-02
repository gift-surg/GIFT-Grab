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

SET(NAME_TEST Test_Epiphan_DVI2PCIeDuo)
if(USE_COLOUR_SPACE_I420)
    SET(COLOUR_SPACE I420)
else(USE_COLOUR_SPACE_I420)
    SET(COLOUR_SPACE BGR24)
endif(USE_COLOUR_SPACE_I420)
ADD_TEST(NAME ${NAME_TEST}
    COMMAND py.test --colour-space=${COLOUR_SPACE} test.py
)
# to avoid copying pygiftgrab
SET_TESTS_PROPERTIES(${NAME_TEST}
    PROPERTIES ENVIRONMENT
    "PYTHONPATH=${CMAKE_BINARY_DIR}:$ENV{PYTHONPATH}"
)
