FILE(COPY
    ${CMAKE_SOURCE_DIR}/tests/epiphan/dvi2pcieduo/test_unit.py
    ${CMAKE_SOURCE_DIR}/tests/epiphan/dvi2pcieduo/test_realtime.py
    ${CMAKE_SOURCE_DIR}/tests/epiphan/dvi2pcieduo/conftest.py
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)
SET(CONFIG_DIR ${CMAKE_SOURCE_DIR}/tests/epiphan/dvi2pcieduo/data)

# SDI and DVI ports on their own
LIST(APPEND PORTS SDI)
LIST(APPEND PORTS DVI)
foreach(PORT ${PORTS})
    SET(NAME_TEST Test_Epiphan_DVI2PCIeDuo_${PORT}_${COLOUR_SPACE})
    ADD_TEST(NAME ${NAME_TEST}
        COMMAND py.test --port=${PORT} --colour-space=${COLOUR_SPACE} test_unit.py
    )
    LIST(APPEND TESTS_LIST ${NAME_TEST})
endforeach(PORT)

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
