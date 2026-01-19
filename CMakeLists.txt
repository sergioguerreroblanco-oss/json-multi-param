cmake_minimum_required(VERSION 3.21)

project(TaskScheduler 
    VERSION 1.0.0
    DESCRIPTION "Multithreaded Task Scheduler in C++14"
    HOMEPAGE_URL "https://github.com/sergioguerreroblanco-oss/json-multi-param"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# -----------------------------------------------------------
# Enable testing framework
# -----------------------------------------------------------
include(CTest)

# -----------------------------------------------------------
# Core library
# -----------------------------------------------------------
add_library(json_multi_param STATIC
    src/logger.cpp
    src/param_base.cpp
    src/param_set.cpp
)

target_include_directories(json_multi_param
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/external
)

if(MSVC)
    target_compile_options(json_multi_param PRIVATE /W4 /permissive- /wd26495)
else()
    target_compile_options(json_multi_param PRIVATE -Wall -Wextra -Wpedantic)
endif()

# -----------------------------------------------------------
# Examples
# -----------------------------------------------------------

# Minimal example
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/examples/minimal/main.cpp")
    add_executable(example_minimal
        examples/minimal/main.cpp
    )
    target_link_libraries(example_minimal PRIVATE json_multi_param)
endif()

# DeviceConfig example (only if the file exists)
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/examples/device_config/device_config_example.cpp")
    add_executable(device_config_example
        examples/device_config/device_config_example.cpp
    )
    target_link_libraries(device_config_example PRIVATE json_multi_param)
endif()

# -----------------------------------------------------------
# Tests
# -----------------------------------------------------------
if(BUILD_TESTING)
    enable_testing()

    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    add_executable(tests 
        tests/test_param_set_basic.cpp
    )

    target_link_libraries(tests PRIVATE json_multi_param gtest_main)

    include(GoogleTest)
    gtest_discover_tests(tests)
endif()
