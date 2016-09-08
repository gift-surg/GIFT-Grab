# to avoid copying stuff around
SET(PYTHONPATH "${CMAKE_BINARY_DIR}:$ENV{PYTHONPATH}")
SET(PYTHONPATH "${CMAKE_SOURCE_DIR}/python/modules:${PYTHONPATH}")

# set colour space
if(USE_I420)
    SET(COLOUR_SPACE I420)
else(USE_I420)
    SET(COLOUR_SPACE BGR24)
endif(USE_I420)

# common configuration
if(USE_HEVC OR USE_XVID OR USE_VP9)
    SUBDIRS(target)
endif(USE_HEVC OR USE_XVID OR USE_VP9)

# Epiphan DVI2PCIe Duo tests
if(USE_EPIPHAN_DVI2PCIE_DUO)
    SUBDIRS(epiphan/dvi2pcieduo)
endif(USE_EPIPHAN_DVI2PCIE_DUO)
