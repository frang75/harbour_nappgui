# Only for debug purposes. Not required in production
set(TARGET_NAME hbaws)

set(AWSROOT "$ENV{AWS_SDK_ROOT}")

if (AWSROOT)
    message("- AWS_SDK_ROOT: ${AWSROOT}")
else()
    message(FATAL_ERROR "AWS_SDK_ROOT environment variable is required to compile ${TARGET_NAME}")
endif()

add_library(${TARGET_NAME} hb_aws.c hbaws.cpp hbaws.h)
target_include_directories(${TARGET_NAME} PUBLIC "../../../include")   # For Harbours headers
target_include_directories(${TARGET_NAME} PUBLIC "..")                 # For hbaws.ch

if (MSVC)
    target_include_directories(${TARGET_NAME} PUBLIC "${AWSROOT}/msvc64/Release/include")   # For AWS-SDK headers
endif()

