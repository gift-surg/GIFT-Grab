## Sample apps
#OPTION(BUILD_TESTS "Build tests" OFF)
#if(BUILD_TESTS)
#    SET(TEST_NAME frmgrab)
#    ADD_EXECUTABLE(${TEST_NAME} tests/frmgrab.cpp)
#    TARGET_LINK_LIBRARIES(${TEST_NAME} ${LIBS})
#endif(BUILD_TESTS)

## Infrastructure for util apps
#INCLUDE_DIRECTORIES(utils/include)
#SET(SDK_UTIL_HEADERS utils/include/sdk_utils.h)
#SET(SDK_UTIL_SOURCES utils/src/sdk_utils.cpp)

## Util for finding out device ID on system
#SET(FINDDEVICE_EXE finddevice)
#ADD_EXECUTABLE(${FINDDEVICE_EXE}
#    utils/app/finddevice.cpp ${SDK_UTIL_HEADERS} ${SDK_UTIL_SOURCES})
#TARGET_LINK_LIBRARIES(${FINDDEVICE_EXE} ${LIBS})

## Util for computing maximum frame rate at full capacity
#SET(FRAMERATE_EXE framerate)
#ADD_EXECUTABLE(${FRAMERATE_EXE}
#    utils/app/framerate.cpp ${SDK_UTIL_HEADERS} ${SDK_UTIL_SOURCES})
#TARGET_LINK_LIBRARIES(${FRAMERATE_EXE} ${LIBS})
## User should turn this on explicitly
#OPTION(DEFINE_MAX_FRAME_RATES "Automatically compute maximum frame-rates using Epiphan DVI2PCIe Duo on system" OFF)

## And actually define port IDs in corresponding header, only when installing
#SET(DVI2PCIeDuo_HEADER ${CMAKE_CURRENT_BINARY_DIR}/dvi2pcieduo_defs.h)
#SET(DVI2PCIeDuo_HEADER_CODE
#    "EXECUTE_PROCESS(COMMAND ./${FINDDEVICE_EXE} DVI OUTPUT_VARIABLE Epiphan_DVI2PCIeDuo_DVI)\nEXECUTE_PROCESS(COMMAND ./${FINDDEVICE_EXE} SDI OUTPUT_VARIABLE Epiphan_DVI2PCIeDuo_SDI)\n"
#    )
#if(DEFINE_MAX_FRAME_RATES)
#    MESSAGE("You set DEFINE_MAX_FRAME_RATES ${DEFINE_MAX_FRAME_RATES}. So please make sure you have working devices turned on and connected to both ports of your Epiphan DVI2PCIe Duo")
#    SET(DVI2PCIeDuo_HEADER_CODE
#        "${DVI2PCIeDuo_HEADER_CODE}EXECUTE_PROCESS(COMMAND ./${FRAMERATE_EXE} DVI OUTPUT_VARIABLE Epiphan_DVI2PCIeDuo_DVI_MAX_FRAME_RATE)\nEXECUTE_PROCESS(COMMAND ./${FRAMERATE_EXE} SDI OUTPUT_VARIABLE Epiphan_DVI2PCIeDuo_SDI_MAX_FRAME_RATE)\nEXECUTE_PROCESS(COMMAND ./${FRAMERATE_EXE} DVI+SDI OUTPUT_VARIABLE Epiphan_DVI2PCIeDuo_DUAL_MAX_FRAME_RATE)\n"
#        )
#endif(DEFINE_MAX_FRAME_RATES)
#SET(DVI2PCIeDuo_HEADER_CODE
#    "${DVI2PCIeDuo_HEADER_CODE}CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/cmake/dvi2pcieduo_defs.h.in ${DVI2PCIeDuo_HEADER})"
#    )
#MESSAGE("When installing, will run:\n${DVI2PCIeDuo_HEADER_CODE}\nto generate ${DVI2PCIeDuo_HEADER} file")
#INSTALL(CODE ${DVI2PCIeDuo_HEADER_CODE})
