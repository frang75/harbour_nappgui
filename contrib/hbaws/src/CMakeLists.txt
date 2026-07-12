# Only for debug purposes. Not required in production
set(TARGET_NAME hbaws)
nap_library(${TARGET_NAME} "" NO NRC_NONE)
nap_target_c_standard(${TARGET_NAME} 99)
nap_target_cxx_standard(${TARGET_NAME} 11)

set(AWSDIR "${CMAKE_CURRENT_SOURCE_DIR}/../awssdk")
set(LIBS "aws-cpp-sdk-core;aws-cpp-sdk-s3")

if (CMAKE_GENERATOR MATCHES "Visual Studio")
    set(AWSBIN "${AWSDIR}/bin/win/msvc")
elseif (CMAKE_GENERATOR MATCHES "Xcode")
    set(AWSBIN "${AWSDIR}/bin/darwin/clang")
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    set(AWSBIN "${AWSDIR}/bin/linux/gcc")
else()
    message(FATAL_ERROR "No vendored AWS-SDK binaries for this generator/compiler")
endif()

target_include_directories(${TARGET_NAME} PUBLIC "${AWSDIR}/include")
target_link_directories(${TARGET_NAME} PUBLIC "${AWSBIN}")

if (CMAKE_GENERATOR MATCHES "Visual Studio")
    # Same defines as contrib/hbaws/hbaws.hbp, required for the AWS-SDK
    # headers to correctly declare dllimport-ed symbols.
    target_compile_definitions(${TARGET_NAME} PRIVATE
        USE_WINDOWS_DLL_SEMANTICS
        USE_IMPORT_EXPORT
        AWS_CRT_CPP_USE_IMPORT_EXPORT
        AWS_HTTP_USE_IMPORT_EXPORT
        AWS_IO_USE_IMPORT_EXPORT
        AWS_ENABLE_IO_COMPLETION_PORTS
        AWS_COMMON_USE_IMPORT_EXPORT
        AWS_CAL_USE_IMPORT_EXPORT
        AWS_COMPRESSION_USE_IMPORT_EXPORT
        AWS_MQTT_USE_IMPORT_EXPORT
        AWS_AUTH_USE_IMPORT_EXPORT
        AWS_SDKUTILS_USE_IMPORT_EXPORT
        AWS_CHECKSUMS_USE_IMPORT_EXPORT
        AWS_EVENT_STREAM_USE_IMPORT_EXPORT
        AWS_S3_USE_IMPORT_EXPORT
    )
    target_compile_options(${TARGET_NAME} PRIVATE /MD /EHsc)
else()
    target_compile_options(${TARGET_NAME} PRIVATE -Wno-ignored-attributes)
endif()

target_link_libraries(${TARGET_NAME} PUBLIC "${LIBS}")
target_include_directories(${TARGET_NAME} PUBLIC "../../../include")   # For Harbours headers
target_include_directories(${TARGET_NAME} PUBLIC "..")                 # For hbaws.ch

