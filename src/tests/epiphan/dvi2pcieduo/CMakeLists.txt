FILE(COPY
    ${CMAKE_SOURCE_DIR}/tests/epiphan/dvi2pcieduo/test_unit.py
    ${CMAKE_SOURCE_DIR}/tests/epiphan/dvi2pcieduo/test_epiphan_py_module.py
    ${CMAKE_SOURCE_DIR}/tests/epiphan/dvi2pcieduo/test_observer.py
    ${CMAKE_SOURCE_DIR}/tests/epiphan/dvi2pcieduo/conftest.py
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)
SET(CONFIG_DIR ${CMAKE_SOURCE_DIR}/tests/epiphan/dvi2pcieduo/data)

# SDI and DVI ports on their own
LIST(APPEND PORTS SDI)
LIST(APPEND PORTS DVI)
foreach(COLOUR_SPACE ${COLOUR_SPACES})
    foreach(PORT ${PORTS})
        SET(NAME_TEST Test_Epiphan_DVI2PCIeDuo_${PORT}_${COLOUR_SPACE})
        ADD_TEST(NAME ${NAME_TEST}
            COMMAND py.test --port=${PORT} --colour-space=${COLOUR_SPACE} test_unit.py
        )
        LIST(APPEND TESTS_LIST ${NAME_TEST})
    endforeach(PORT)
endforeach(COLOUR_SPACE)

# Both ports of Epiphan DVI2PCIe Duo using the Epiphan Python module
foreach(COLOUR_SPACE ${COLOUR_SPACES})
    SET(NAME_TEST Test_Epiphan_DVI2PCIeDuo_PyModule_${COLOUR_SPACE})
    ADD_TEST(NAME ${NAME_TEST}
        COMMAND py.test --colour-space=${COLOUR_SPACE} --config-dir=${CONFIG_DIR} test_epiphan_py_module.py
    )
    LIST(APPEND TESTS_LIST ${NAME_TEST})
endforeach(COLOUR_SPACE)

# Commented out to test VideoFrame assignment operator
# Both ports of Epiphan DVI2PCIe Duo using the observer pattern
#foreach(COLOUR_SPACE ${COLOUR_SPACES})
#    foreach(PORT ${PORTS})
#        SET(NAME_TEST Test_Epiphan_DVI2PCIeDuo_ObserverPattern_${PORT}_${COLOUR_SPACE})
#        if(COLOUR_SPACE STREQUAL "I420")
#            SET(FRAME_RATE 60)
#        elseif(COLOUR_SPACE STREQUAL "BGRA")
#            SET(FRAME_RATE 30)
#        endif(COLOUR_SPACE STREQUAL "I420")
#
#        ADD_TEST(NAME ${NAME_TEST}
#            COMMAND py.test --port=${PORT} --colour-space=${COLOUR_SPACE} --frame-rate=${FRAME_RATE} --observers=3 test_observer.py
#        )
#        LIST(APPEND TESTS_LIST ${NAME_TEST})
#    endforeach(PORT)
#endforeach(COLOUR_SPACE)

# to avoid copying stuff around
SET_TESTS_PROPERTIES(${TESTS_LIST}
    PROPERTIES ENVIRONMENT "PYTHONPATH=${PYTHONPATH}"
)
