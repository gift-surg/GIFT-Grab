FILE(COPY
    ${CMAKE_SOURCE_DIR}/tests/target/test_unit.py
    ${CMAKE_SOURCE_DIR}/tests/target/conftest.py
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)

# H265 tests
if(USE_H265)
    SET(NAME_TEST Test_H265)
    ADD_TEST(NAME ${NAME_TEST}
        COMMAND py.test --codec=H265 test_unit.py
    )
    LIST(APPEND TESTS_LIST ${NAME_TEST})
endif(USE_H265)

# Xvid tests
if(USE_XVID)
    SET(NAME_TEST Test_Xvid)
    ADD_TEST(NAME ${NAME_TEST}
        COMMAND py.test --codec=Xvid test_unit.py
    )
    LIST(APPEND TESTS_LIST ${NAME_TEST})
endif(USE_XVID)

# to avoid copying stuff around
SET_TESTS_PROPERTIES(${TESTS_LIST}
    PROPERTIES ENVIRONMENT "PYTHONPATH=${PYTHONPATH}"
)
