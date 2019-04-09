CMAKE_MINIMUM_REQUIRED(VERSION 3.2.0)

if(NOT PYTHONLIBS_VERSION_STRING)
    # Needed in case PythonLibs discovery script not run, see
    # https://github.com/gift-surg/GIFT-Grab/issues/83
    # TODO handle this more elegantly
    ADD_SUBDIRECTORY(cmake/pythonlibs)
    if(NOT PYTHONLIBS_VERSION_STRING)
        MESSAGE(FATAL_ERROR "Could not determine Python version")
    endif()
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
    LIST(APPEND GG_BOOST_COMPONENTS numpy${GG_BOOST_PYTHON_SUFFIX})
endif(USE_NUMPY)
FIND_PACKAGE(Boost COMPONENTS ${GG_BOOST_COMPONENTS} REQUIRED)
SET(Boost_INCLUDE_DIR ${Boost_INCLUDE_DIR}
    CACHE INTERNAL "Boost.Python include dirs"
)
SET(Boost_LIBRARIES ${Boost_LIBRARIES}
    CACHE INTERNAL "Boost.Python libs"
)
