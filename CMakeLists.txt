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

# Add core library 
add_subdirectory(giftgrab)

# Install conan dependencies
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

# Add giftgrab library 
add_library(giftgrab)
target_link_libraries(giftgrab PUBLIC giftgrab_core ${CONAN_LIBS})
target_sources(giftgrab
  PUBLIC
  src/opencv/opencv_video_source.h
  src/opencv/opencv_video_target.h
  PRIVATE
  src/opencv/opencv_video_source.cpp
  src/opencv/opencv_video_target.cpp
)