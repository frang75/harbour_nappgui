# Windows
# cmake -G "Visual Studio 17 2022" -S . -B build
# cmake -G "MinGW Makefiles" -S . -B build
# Linux / macOS
# cmake -S . -B build
# cmake -S . -B build

cmake_minimum_required(VERSION 3.13)
project(hello_s3)

set(CMAKE_CXX_STANDARD 11)

if (WIN32)
    # Vendored AWS-SDK binaries (contrib/hbaws/awssdk). No AWSSDK CMake
    # package to find_package() - headers/libs are used directly.
    # bin/win/msvc was built with MSVC, so its C++ name mangling is
    # ABI-incompatible with mingw64/clang (confirmed empirically: linking
    # this project with MinGW against these binaries fails with "undefined
    # reference" for every AWS symbol). Only MSVC consumers are supported
    # until compiler-specific binaries are vendored under bin/win/<compiler>.
    set(AWSDIR "${CMAKE_CURRENT_SOURCE_DIR}/../../awssdk")
    set(AWSBIN "${AWSDIR}/bin/win/msvc")

    add_executable(${PROJECT_NAME} hello.cpp)
    target_include_directories(${PROJECT_NAME} PRIVATE "${AWSDIR}/include")
    target_link_directories(${PROJECT_NAME} PRIVATE "${AWSBIN}")
    target_link_libraries(${PROJECT_NAME} aws-cpp-sdk-core aws-cpp-sdk-s3)

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

    if (MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE /MD /EHsc)
    else()
        target_compile_options(${PROJECT_NAME} PRIVATE -Wno-ignored-attributes)
    endif()

    # Copy every runtime DLL in bin/win/msvc (aws-cpp-sdk-* plus their
    # transitive aws-c-*/aws-crt-cpp dependencies) next to the executable
    # so it can run.
    file(GLOB AWS_RUNTIME_DLLS "${AWSBIN}/*.dll")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${AWS_RUNTIME_DLLS}
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
    )
else()
    # Linux / macOS: still relies on a user-built AWS-SDK via AWS_SDK_ROOT
    # (not vendored yet).
    set(AWSROOT "$ENV{AWS_SDK_ROOT}")

    if (AWSROOT)
        message("- AWS_SDK_ROOT: ${AWSROOT}")
    else()
        message(FATAL_ERROR "AWS_SDK_ROOT environment variable is required to compile ${PROJECT_NAME}")
    endif()

    if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
        set(AWSROOT ${AWSROOT}/gcc)
    elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "AppleClang")
        set(AWSROOT ${AWSROOT}/clang)
    else()
        message(FATAL_ERROR "Unknown compiler ${CMAKE_CXX_COMPILER_ID}")
    endif()

    set(AWSROOT ${AWSROOT}/Release)
    set(CMAKE_PREFIX_PATH "${AWSROOT}" ${CMAKE_PREFIX_PATH})
    find_package(AWSSDK REQUIRED COMPONENTS s3)
    add_executable(${PROJECT_NAME} hello.cpp)
    target_link_libraries(${PROJECT_NAME} ${AWSSDK_LINK_LIBRARIES})
endif()
