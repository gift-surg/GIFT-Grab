# Obtain Catch header for testing
SET(CATCH_DIR "${CMAKE_BINARY_DIR}/extern/Catch")
SET(CATCH_HEADER_URL
    "https://raw.githubusercontent.com/philsquared/Catch/v1.5.8/single_include/catch.hpp"
)
FILE(DOWNLOAD
     ${CATCH_HEADER_URL}
     "${CATCH_DIR}/catch.hpp"
     SHOW_PROGRESS STATUS CATCH_HEADER_DOWNLOAD_STATUS
)
LIST(GET CATCH_HEADER_DOWNLOAD_STATUS 0 1
     CATCH_HEADER_DOWNLOAD_STATUS_EXIT_CODE
     CATCH_HEADER_DOWNLOAD_STATUS_ERROR_STRING
)
if(CATCH_HEADER_DOWNLOAD_STATUS_EXIT_CODE)  # not-zero exit code => error!
    MESSAGE(
        "Catch is needed for the tests, but could not download the Catch header"
    )
    MESSAGE(
        "The URL was: ${CATCH_HEADER_URL}"
    )
    MESSAGE(FATAL_ERROR
        "The error was: ${CATCH_HEADER_DOWNLOAD_STATUS_ERROR_STRING}"
    )
endif(CATCH_HEADER_DOWNLOAD_STATUS_EXIT_CODE)
INCLUDE_DIRECTORIES(${CATCH_DIR})

FILE(COPY
    ${CMAKE_SOURCE_DIR}/tests/epiphan/dvi2pcieduo/test_unit.py
    ${CMAKE_SOURCE_DIR}/tests/epiphan/dvi2pcieduo/test_epiphan_py_module.py
    ${CMAKE_SOURCE_DIR}/tests/epiphan/dvi2pcieduo/test_observer.py
    ${CMAKE_SOURCE_DIR}/tests/epiphan/dvi2pcieduo/conftest.py
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)
SET(CONFIG_DIR ${CMAKE_SOURCE_DIR}/tests/epiphan/dvi2pcieduo/data)

LIST(APPEND PORTS SDI)
LIST(APPEND PORTS DVI)

# VideoSourceFactory
SET(VIDEO_SOURCE_FACTORY_TEST test_video_source_factory)
ADD_EXECUTABLE(
    ${VIDEO_SOURCE_FACTORY_TEST}
    ${CMAKE_SOURCE_DIR}/tests/epiphan/dvi2pcieduo/test_video_source_factory.cpp
)
TARGET_LINK_LIBRARIES(
    ${VIDEO_SOURCE_FACTORY_TEST}
    ${LIBS} ${NAME}
)
foreach(COLOUR_SPACE ${COLOUR_SPACES})
    foreach(PORT ${PORTS})
        SET(NAME_TEST Test_Epiphan_DVI2PCIeDuo_VideoSourceFactory_${PORT}_${COLOUR_SPACE})
        ADD_TEST(NAME ${NAME_TEST}
            COMMAND ${VIDEO_SOURCE_FACTORY_TEST} ${PORT} ${COLOUR_SPACE}
        )
        LIST(APPEND TESTS_LIST ${NAME_TEST})
    endforeach(PORT)
endforeach(COLOUR_SPACE)

# SDI and DVI ports on their own
foreach(COLOUR_SPACE ${COLOUR_SPACES})
    foreach(PORT ${PORTS})
        SET(NAME_TEST Test_Epiphan_DVI2PCIeDuo_${PORT}_${COLOUR_SPACE})
        ADD_TEST(NAME ${NAME_TEST}
            COMMAND py.test --port=${PORT} --colour-space=${COLOUR_SPACE} test_unit.py
        )
        LIST(APPEND TESTS_LIST ${NAME_TEST})
    endforeach(PORT)
endforeach(COLOUR_SPACE)

# Disabled the Python module tests until issue #114 is resolved
## Both ports of Epiphan DVI2PCIe Duo using the Epiphan Python module
#foreach(COLOUR_SPACE ${COLOUR_SPACES})
#    SET(NAME_TEST Test_Epiphan_DVI2PCIeDuo_PyModule_${COLOUR_SPACE})
#    ADD_TEST(NAME ${NAME_TEST}
#        COMMAND py.test --colour-space=${COLOUR_SPACE} --config-dir=${CONFIG_DIR} test_epiphan_py_module.py
#    )
#    LIST(APPEND TESTS_LIST ${NAME_TEST})
#endforeach(COLOUR_SPACE)

# Both ports of Epiphan DVI2PCIe Duo using the observer pattern
foreach(COLOUR_SPACE ${COLOUR_SPACES})
    foreach(PORT ${PORTS})
        if(COLOUR_SPACE STREQUAL "I420")
            if(USE_EPIPHANSDK)
                SET(FRAME_RATE 40)
            elseif(USE_LIBVLC)
                SET(FRAME_RATE 19)
            endif(USE_EPIPHANSDK)
        elseif(COLOUR_SPACE STREQUAL "BGRA")
            SET(FRAME_RATE 30)
        endif(COLOUR_SPACE STREQUAL "I420")
        SET(NAME_TEST Test_Epiphan_DVI2PCIeDuo_ObserverPattern_${PORT}_${COLOUR_SPACE}_${FRAME_RATE}fps)

        ADD_TEST(NAME ${NAME_TEST}
            COMMAND py.test --port=${PORT} --colour-space=${COLOUR_SPACE} --frame-rate=${FRAME_RATE} --observers=3 test_observer.py
        )
        LIST(APPEND TESTS_LIST ${NAME_TEST})
    endforeach(PORT)
endforeach(COLOUR_SPACE)

# to avoid copying stuff around
SET_TESTS_PROPERTIES(${TESTS_LIST}
    PROPERTIES ENVIRONMENT "PYTHONPATH=${PYTHONPATH}"
)
