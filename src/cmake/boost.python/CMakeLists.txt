CMAKE_MINIMUM_REQUIRED(VERSION 3.2.0)

if(WIN32)
    SET(Boost_USE_MULTITHREADED ON)
    # The proper way to define these is to use the ciso646 header. However
    # that conflicts with Python's logical operator keywords defined as part
    # of Boost.Python (see https://github.com/boostorg/python/issues/67). So
    # we resort to this kind of workaround. This is inspired by
    # https://github.com/PixarAnimationStudios/OpenSubdiv/pull/817
    if(MSVC)
        list(APPEND NEW_STYLE_LOGICAL_OPERATIONS
            /Dand=&&
            /Dand_eq=&=
            /Dbitand=&
            /Dbitor=|
            /Dcompl=~
            /Dnot=!
            /Dnot_eq=!=
            /Dor=||
            /Dor_eq=|=
            # nvcc does not seem to like a caret being the last character
            # in a command line defined preprocessor symbol, so add an
            # empty trailing comment to avoid this.
            /Dxor=^/**/
            /Dxor_eq=^=
        )
        ADD_DEFINITIONS(${NEW_STYLE_LOGICAL_OPERATIONS})
    endif()
endif()

if(NOT PYTHONLIBS_VERSION_STRING)
    # Needed in case PythonLibs discovery script not run, see
    # https://github.com/gift-surg/GIFT-Grab/issues/83
    # TODO handle this more elegantly
    ADD_SUBDIRECTORY(${CMAKE_SOURCE_DIR}/../../cmake/pythonlibs ${CMAKE_CURRENT_BINARY_DIR}/cmake/pythonlibs-redundant-run)
    if(NOT PYTHONLIBS_VERSION_STRING)
        MESSAGE(FATAL_ERROR "Could not determine Python version")
    endif()
endif()
if(PYTHONLIBS_VERSION_STRING MATCHES ^3\.*)
    if(UNIX)
        SET(GG_BOOST_PYTHON_SUFFIX 3)
    elseif(WIN32)
        if(PYTHONLIBS_VERSION_STRING MATCHES ^3\.7\.*)
            SET(GG_BOOST_PYTHON_SUFFIX 37)
        else()
            MESSAGE(FATAL_ERROR
                "Found Python version ${PYTHONLIBS_VERSION_STRING} (on Windows only 3.7 is experimentally supported)")
        endif()
    endif()
elseif(PYTHONLIBS_VERSION_STRING MATCHES ^2\.7\.*)
    if(UNIX)
        SET(GG_BOOST_PYTHON_SUFFIX "")
    elseif(WIN32)
        SET(GG_BOOST_PYTHON_SUFFIX 27)
    endif()
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
