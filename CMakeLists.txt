cmake_minimum_required(VERSION 3.13.0)

# Fetch project meta info 
include("${CMAKE_CURRENT_LIST_DIR}/project_meta_info.in")
project(${project_name}
        VERSION ${project_version}
        DESCRIPTION ${project_description}
        LANGUAGES CXX
        )

# CMake visibility control        
set(CMAKE_CXX_VISIBILITY_PRESET      hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN  YES)


add_subdirectory(giftgrab)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()
