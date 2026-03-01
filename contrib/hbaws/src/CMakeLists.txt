# Only for debug purposes. Not required in production
set(TARGET_NAME hbaws)
nap_library(${TARGET_NAME} "" NO NRC_NONE)
nap_target_c_standard(${TARGET_NAME} 99)
nap_target_cxx_standard(${TARGET_NAME} 11)

set(AWSROOT "$ENV{AWS_SDK_ROOT}")

if (AWSROOT)
    message("- AWS_SDK_ROOT: ${AWSROOT}")
else()
    message(FATAL_ERROR "AWS_SDK_ROOT environment variable is required to compile ${TARGET_NAME}")
endif()

if (CMAKE_GENERATOR MATCHES "Visual Studio")
    set(AWSROOT ${AWSROOT}/msvc64/Release)
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    set(AWSROOT ${AWSROOT}/gcc/Release)
else()
    message(FATAL_ERROR "Unknown HBAWS compiler")
endif()

set(CMAKE_PREFIX_PATH "${AWSROOT}" ${CMAKE_PREFIX_PATH})
find_package(AWSSDK REQUIRED COMPONENTS s3)

target_link_libraries(${TARGET_NAME} ${AWSSDK_LINK_LIBRARIES})

target_include_directories(${TARGET_NAME} PUBLIC "../../../include")   # For Harbours headers
target_include_directories(${TARGET_NAME} PUBLIC "..")                 # For hbaws.ch

if (CMAKE_GENERATOR MATCHES "Visual Studio")
    target_include_directories(${TARGET_NAME} PUBLIC "${AWSROOT}/msvc64/Release/include")   # For AWS-SDK headers
endif()

