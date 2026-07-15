# Windows
# cmake -G "Visual Studio 18 2026" -S . -B build
# cmake -G "MinGW Makefiles" -S . -B build
# cmake -G "MinGW Makefiles" -S . -B build -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++
# cmake --build build

# Linux
# cmake -S . -B build
# cmake -S . -B build -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++
# cmake --build build

# macOS
# export MACOSX_DEPLOYMENT_TARGET=13.0  # Ventura
# cmake -S . -B build
# cmake --build build

cmake_minimum_required(VERSION 3.13)
project(hello_s3)

set(CMAKE_CXX_STANDARD 11)
set(AWSDIR "${CMAKE_CURRENT_SOURCE_DIR}/../../awssdk")

if (WIN32)
    if (MSVC)
        set(AWSBIN "${AWSDIR}/bin/win/msvc")
    else()
        set(AWSBIN "${AWSDIR}/bin/win/gcc")
    endif()
elseif (UNIX AND NOT APPLE)
    set(AWSBIN "${AWSDIR}/bin/linux/gcc")
else()
    set(AWSBIN "${AWSDIR}/bin/darwin/clang")
endif()

add_executable(${PROJECT_NAME} hello.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE "${AWSDIR}/include")
target_link_directories(${PROJECT_NAME} PRIVATE "${AWSBIN}")
target_link_libraries(${PROJECT_NAME} aws-cpp-sdk-core aws-cpp-sdk-s3)

if (NOT (WIN32 AND MSVC))
    # aws-cpp-sdk-core/s3 import Aws::Crt::* symbols from aws-crt-cpp;
    # MSVC's .lib resolves them via forwarding, GNU ld needs it explicit.
    target_link_libraries(${PROJECT_NAME} aws-crt-cpp)
endif()

if (UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} stdc++)
endif()

if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /MD /EHsc)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wno-ignored-attributes)
endif()

if (WIN32)
    # Same defines as contrib/hbaws/hbaws.hbp, required for the AWS-SDK
    # headers to correctly declare dllimport-ed symbols.
    target_compile_definitions(${PROJECT_NAME} PRIVATE
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

    # Copy every runtime DLL in AWSBIN (aws-cpp-sdk-* plus their transitive
    # aws-c-*/aws-crt-cpp dependencies) next to the executable so it can run.
    file(GLOB AWS_RUNTIME_DLLS "${AWSBIN}/*.dll")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${AWS_RUNTIME_DLLS}
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
    )
endif()
