cmake_minimum_required(VERSION 3.13.0)

# Fetch project meta info 
include("${CMAKE_CURRENT_LIST_DIR}/project_meta_info.in")
project(${project_name}
        VERSION ${project_version}
        DESCRIPTION ${project_description}
        LANGUAGES CXX
        )
