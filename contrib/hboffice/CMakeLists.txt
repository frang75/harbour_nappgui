#------------------------------------------------------------------------------
# HBOffice CMake build script
# Copyright (C) 2024 Francisco García Collado
#------------------------------------------------------------------------------
# https://gitlab.kitware.com/cmake/cmake/-/issues/25196
if(${CMAKE_VERSION} VERSION_GREATER "3.99.999")
    cmake_minimum_required(VERSION 3.10)
elseif(${CMAKE_VERSION} VERSION_GREATER "3.26.999")
    cmake_minimum_required(VERSION 3.5)
else()
    cmake_minimum_required(VERSION 3.0)
endif()
project(HBOffice)

# Configurations
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE INTERNAL "Configuration Types" FORCE)

# Welcome
message (STATUS "---------------------------------------------")
message (STATUS "Harbour LibreOffice Wrapper")
message (STATUS "Generator: ${CMAKE_GENERATOR}")
message (STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message (STATUS "---------------------------------------------")

# Generate targets
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
add_subdirectory(src/officesdk)
