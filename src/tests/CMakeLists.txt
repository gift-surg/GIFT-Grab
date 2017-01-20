# to avoid copying stuff around
SET(PYTHONPATH "${CMAKE_BINARY_DIR}:$ENV{PYTHONPATH}")
SET(PYTHONPATH "${CMAKE_SOURCE_DIR}/python/modules:${PYTHONPATH}")
SET(PYTHONPATH "${CMAKE_SOURCE_DIR}/tests:${PYTHONPATH}")

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

# set colour space
if(USE_BGRA)
    LIST(APPEND COLOUR_SPACES BGRA)
endif(USE_BGRA)
if(USE_I420)
    LIST(APPEND COLOUR_SPACES I420)
endif(USE_I420)
if(USE_UYVY)
    LIST(APPEND COLOUR_SPACES UYVY)
endif(USE_UYVY)

# test VideoFrame
SUBDIRS(videoframe)

# test pipelined processing
SUBDIRS(pipeline)

# common configuration
if(USE_HEVC OR USE_XVID OR USE_VP9)
    SUBDIRS(target)
endif(USE_HEVC OR USE_XVID OR USE_VP9)

# Epiphan DVI2PCIe Duo tests
if(USE_EPIPHAN_DVI2PCIE_DUO)
    SUBDIRS(epiphan/dvi2pcieduo)
endif(USE_EPIPHAN_DVI2PCIE_DUO)

# Network sources tests
if(USE_NETWORK_SOURCES)
    SUBDIRS(network)
endif(USE_NETWORK_SOURCES)

# Blackmagic DeckLink SDI 4K tests
if(USE_BLACKMAGIC_DECKLINK_SDI_4K)
    SUBDIRS(blackmagic/decklinksdi4k)
endif(USE_BLACKMAGIC_DECKLINK_SDI_4K)

# Support for reading video files
if(USE_FILES)
    ADD_SUBDIRECTORY(files)
endif(USE_FILES)
