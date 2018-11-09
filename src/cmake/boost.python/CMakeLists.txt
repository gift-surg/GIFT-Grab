CMAKE_MINIMUM_REQUIRED(VERSION 3.12.4)

if(NOT PYTHONLIBS_VERSION_STRING)
    MESSAGE(FATAL_ERROR "Could not determine Python version")
endif()
if(PYTHONLIBS_VERSION_STRING MATCHES ^3\.*)
    SET(GG_BOOST_PYTHON_SUFFIX 3)
elseif(PYTHONLIBS_VERSION_STRING MATCHES ^2\.7\.*)
    SET(GG_BOOST_PYTHON_SUFFIX "")
else()
    MESSAGE(FATAL_ERROR "Found unsupported Python version ${PYTHONLIBS_VERSION_STRING}")
endif()

SET(GG_BOOST_COMPONENTS python${GG_BOOST_PYTHON_SUFFIX})
if(USE_NUMPY)
    if(BOOST_ROOT AND GG_BOOST_PYTHON_SUFFIX)
        # The following line is a workaround to a problem related to issue #50.
        # In the case of specifying a custom Boost via e.g. BOOST_ROOT on a system
        # with another (global) Boost installation, pkg-config's linker flags lead
        # to the implicit discovery of the system-wide Boost.Python (version 2!)
        # library, which gets linked into GIFT-Grab and causes conflicts with the
        # specified Boost installation
        LIST(APPEND GG_BOOST_COMPONENTS python)
    endif()
    LIST(APPEND GG_BOOST_COMPONENTS numpy${GG_BOOST_PYTHON_SUFFIX})
endif(USE_NUMPY)
FIND_PACKAGE(Boost COMPONENTS ${GG_BOOST_COMPONENTS} REQUIRED)
SET(Boost_INCLUDE_DIR ${Boost_INCLUDE_DIR}
    CACHE INTERNAL "Boost.Python include dirs"
)
SET(Boost_LIBRARIES ${Boost_LIBRARIES}
    CACHE INTERNAL "Boost.Python libs"
)
