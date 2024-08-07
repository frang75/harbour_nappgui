#------------------------------------------------------------------------------
# NAppGUI CMake build script
# Copyright (C) 2018 Francisco García Collado
# MIT License (See LICENSE.txt)
#------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.0)
project(NAppGUI)

set(NAPPGUI_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

# Configurations
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;ReleaseWithAssert" CACHE INTERNAL "Configuration Types" FORCE)
set(CMAKE_CXX_FLAGS_RELEASEWITHASSERT ${CMAKE_CXX_FLAGS_RELEASE})
set(CMAKE_C_FLAGS_RELEASEWITHASSERT ${CMAKE_C_FLAGS_RELEASE})
set(CMAKE_EXE_LINKER_FLAGS_RELEASEWITHASSERT ${CMAKE_EXE_LINKER_FLAGS_RELEASE})
set(CMAKE_SHARED_LINKER_FLAGS_RELEASEWITHASSERT ${CMAKE_SHARED_LINKER_FLAGS_RELEASE})

# Globals
set(NAPPGUI_SHARED False CACHE BOOL "Build NAppGUI shared libraries.")
set(NAPPGUI_DEMO True CACHE BOOL "Build NAppGUI demo examples.")
set(NAPPGUI_WEB False)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(INSTALL_PERM "OWNER_READ;OWNER_WRITE;OWNER_EXECUTE;GROUP_READ;GROUP_EXECUTE;WORLD_READ;WORLD_EXECUTE")

# Welcome
string(TIMESTAMP CURRENT_YEAR %Y)
include(${NAPPGUI_ROOT_PATH}/prj/NAppVersion.cmake)
include(${NAPPGUI_ROOT_PATH}/prj/NAppUtils.cmake)
message (STATUS "---------------------------------------------")
message (STATUS "NAppGUI Cross-Platform SDK")
message (STATUS "2015-${CURRENT_YEAR} Francisco Garcia Collado")
message (STATUS "MIT License")
message (STATUS "---------------------------------------------")
nap_build_opt("Version" "${NAPPGUI_MAJOR}.${NAPPGUI_MINOR}.${NAPPGUI_PATCH}.${NAPPGUI_BUILD}")
nap_build_opt("Generator" "${CMAKE_GENERATOR}")
nap_build_opt("Build shared" "${NAPPGUI_SHARED}")

# Config compiler
include(${NAPPGUI_ROOT_PATH}/prj/NAppCompilers.cmake)
nap_config_compiler()
nap_build_opt("Compiler" "${CMAKE_CXX_COMPILER_ID}")
nap_build_opt("Compiler Vers" "${CMAKE_CXX_COMPILER_VERSION}")
nap_build_opt("Build type" "${CMAKE_BUILD_TYPE}")
nap_build_opt("Build arch" "${CMAKE_ARCHITECTURE}")
nap_build_opt("Host arch" "${CMAKE_HOST_ARCHITECTURE}")
nap_build_opt("PackageID" "${CMAKE_PACKAGE_ID}")
message (STATUS "---------------------------------------------")

# Tools required to generate NAppGUI projets ('nrc')
# They need to be compiled before NAppGUI configure/generate
include(${NAPPGUI_ROOT_PATH}/prj/NAppGenerateTools.cmake)
nap_generate_tools()

# Generate targets
include(${NAPPGUI_ROOT_PATH}/prj/NAppTarget.cmake)
include(${NAPPGUI_ROOT_PATH}/CMakeTargets.cmake)

foreach (target ${ALL_TARGETS})
    add_subdirectory(${target})
endforeach()
