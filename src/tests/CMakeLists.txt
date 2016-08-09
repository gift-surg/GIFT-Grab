# common configuration
if(USE_H265 OR USE_XVID)
    FILE(COPY
        ${CMAKE_SOURCE_DIR}/tests/target/test_unit.py
        ${CMAKE_SOURCE_DIR}/tests/target/conftest.py
        ${CMAKE_SOURCE_DIR}/tests/utils/inspection.py
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
    )
endif(USE_H265 OR USE_XVID)

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

# Epiphan DVI2PCIe Duo tests
if(USE_EPIPHAN_DVI2PCIE_DUO)
    SUBDIRS(epiphan/dvi2pcieduo)
endif(USE_EPIPHAN_DVI2PCIE_DUO)

# to avoid copying stuff around
SET_TESTS_PROPERTIES(${TESTS_LIST}
    PROPERTIES ENVIRONMENT
    "PYTHONPATH=${CMAKE_BINARY_DIR}:$ENV{PYTHONPATH}"
)
