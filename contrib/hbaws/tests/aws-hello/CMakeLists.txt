# Windows
# cmake -G "Visual Studio 18 2026" -S . -B build
# cmake -G "MinGW Makefiles" -S . -B build
# cmake -G "MinGW Makefiles" -S . -B build -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++
# cmake --build build
# Linux / macOS
# cmake -S . -B build
# cmake -S . -B build

cmake_minimum_required(VERSION 3.13)
project(hello_s3)

set(CMAKE_CXX_STANDARD 11)

if (WIN32)
    # Vendored AWS-SDK binaries (contrib/hbaws/awssdk). No AWSSDK CMake
    # package to find_package() - headers/libs are used directly.
    # bin/win/msvc was built with MSVC (MSVC C++ name mangling, ABI-
    # incompatible with mingw64/clang - confirmed empirically: linking this
    # project with MinGW against MSVC binaries fails with "undefined
    # reference" for every AWS symbol). bin/win/gcc was built with mingw64;
    # the "clang" toolchain here is MSYS2 UCRT64's clang, which targets
    # x86_64-w64-windows-gnu and links against libstdc++ - same GNU/Itanium
    # ABI as mingw64 - so it also uses bin/win/gcc.
    set(AWSDIR "${CMAKE_CURRENT_SOURCE_DIR}/../../awssdk")
    if (MSVC)
        set(AWSBIN "${AWSDIR}/bin/win/msvc")
    else()
        set(AWSBIN "${AWSDIR}/bin/win/gcc")
    endif()

    add_executable(${PROJECT_NAME} hello.cpp)
    target_include_directories(${PROJECT_NAME} PRIVATE "${AWSDIR}/include")
    target_link_directories(${PROJECT_NAME} PRIVATE "${AWSBIN}")
    target_link_libraries(${PROJECT_NAME} aws-cpp-sdk-core aws-cpp-sdk-s3)
    if (NOT MSVC)
        # aws-cpp-sdk-core/s3 import Aws::Crt::* symbols from aws-crt-cpp;
        # MSVC's .lib resolves them via forwarding, GNU ld needs it explicit.
        target_link_libraries(${PROJECT_NAME} aws-crt-cpp)
    endif()

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

    # Copy every runtime DLL in AWSBIN (aws-cpp-sdk-* plus their transitive
    # aws-c-*/aws-crt-cpp dependencies) next to the executable so it can run.
    file(GLOB AWS_RUNTIME_DLLS "${AWSBIN}/*.dll")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${AWS_RUNTIME_DLLS}
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
    )
elseif (UNIX AND NOT APPLE)
    # Linux: vendored AWS-SDK binaries (contrib/hbaws/awssdk), same approach
    # as Windows - no AWSSDK CMake package to find_package(), headers/libs
    # used directly. Linux gcc and clang share the same GNU/Itanium C++ ABI,
    # so bin/linux/gcc serves both.
    set(AWSDIR "${CMAKE_CURRENT_SOURCE_DIR}/../../awssdk")
    set(AWSBIN "${AWSDIR}/bin/linux/gcc")

    add_executable(${PROJECT_NAME} hello.cpp)
    target_include_directories(${PROJECT_NAME} PRIVATE "${AWSDIR}/include")
    target_link_directories(${PROJECT_NAME} PRIVATE "${AWSBIN}")
    target_link_libraries(${PROJECT_NAME} aws-cpp-sdk-core aws-cpp-sdk-s3 aws-crt-cpp stdc++)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wno-ignored-attributes)

    # CMake automatically adds a build-tree RPATH for target_link_directories
    # paths, so the executable finds these .so files without needing
    # LD_LIBRARY_PATH when run straight from the build directory.
else()
    # macOS: still relies on a user-built AWS-SDK via AWS_SDK_ROOT
    # (not vendored yet).
    set(AWSROOT "$ENV{AWS_SDK_ROOT}")

    if (AWSROOT)
        message("- AWS_SDK_ROOT: ${AWSROOT}")
    else()
        message(FATAL_ERROR "AWS_SDK_ROOT environment variable is required to compile ${PROJECT_NAME}")
    endif()

    set(AWSROOT ${AWSROOT}/clang/Release)
    set(CMAKE_PREFIX_PATH "${AWSROOT}" ${CMAKE_PREFIX_PATH})
    find_package(AWSSDK REQUIRED COMPONENTS s3)
    add_executable(${PROJECT_NAME} hello.cpp)
    target_link_libraries(${PROJECT_NAME} ${AWSSDK_LINK_LIBRARIES})
endif()
