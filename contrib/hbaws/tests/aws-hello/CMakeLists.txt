# Windows
# cmake -G "Visual Studio 17 2022" -S . -B build
# cmake -G "MinGW Makefiles" -S . -B build
# Linux
# cmake -S . -B build

cmake_minimum_required(VERSION 3.13)
project(hello_s3)

set(CMAKE_CXX_STANDARD 11)

set(AWSROOT "$ENV{AWS_SDK_ROOT}")

if (AWSROOT)
    message("- AWS_SDK_ROOT: ${AWSROOT}")
else()
    message(FATAL_ERROR "AWS_SDK_ROOT environment variable is required to compile ${PROJECT_NAME}")
endif()

if (CMAKE_GENERATOR MATCHES "Visual Studio")
    set(AWSROOT ${AWSROOT}/msvc64)

elseif(CMAKE_GENERATOR MATCHES "MinGW Makefiles")
    set(AWSROOT ${AWSROOT}/mingw64)

elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    set(AWSROOT ${AWSROOT}/gcc)

else()
    message(FATAL_ERROR "Unknown compiler")

endif()

set(AWSROOT ${AWSROOT}/Release)
set(CMAKE_PREFIX_PATH "${AWSROOT}" ${CMAKE_PREFIX_PATH})
find_package(AWSSDK REQUIRED COMPONENTS s3)

add_executable(${PROJECT_NAME} hello.cpp)
target_link_libraries(${PROJECT_NAME} ${AWSSDK_LINK_LIBRARIES})
