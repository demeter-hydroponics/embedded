cmake_minimum_required(VERSION 3.14)

# Name of the test target
set(This c_source_tests)

# Project definition
project(${This} C CXX)

# Require C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# ------------------------------------------------------------------------
# 1. Optionally enable ccache if it is found on the system
# ------------------------------------------------------------------------
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    message(STATUS "Using ccache for faster incremental builds.")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
endif()

# ------------------------------------------------------------------------
# 2. Enable unity (jumbo) builds if you wish (requires CMake >= 3.16)
# ------------------------------------------------------------------------
option(ENABLE_UNITY "Enable Unity builds for faster compilation" ON)

if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.16" AND ENABLE_UNITY)
    message(STATUS "Unity build enabled.")
    set(CMAKE_UNITY_BUILD ON)
endif()

# ------------------------------------------------------------------------
# Compiler flags
# ------------------------------------------------------------------------
set(CMAKE_C_FLAGS
    "${CMAKE_C_FLAGS} -O0 -g -Werror -Wextra -Wall -pedantic -pedantic-errors \
     -Wfloat-equal -Wredundant-decls -Wswitch-default -Wconversion")

set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -O0 -g -Werror -Wextra -Wall -pedantic -pedantic-errors")

add_definitions(-DUNIT_TEST)

# ------------------------------------------------------------------------
# Directory variables
# ------------------------------------------------------------------------
set(GENERATED_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../generated)
set(PROTOBUF_DIR ${GENERATED_DIR}/c)
set(PBLIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/protobuf/submodules/nanopb)
set(LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../libs)

# ------------------------------------------------------------------------
# Include directories
# ------------------------------------------------------------------------
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../inc/driver
    ${CMAKE_CURRENT_SOURCE_DIR}/../inc/device
    ${CMAKE_CURRENT_SOURCE_DIR}/../inc/app
    ${CMAKE_CURRENT_SOURCE_DIR}/../HAL
    ${CMAKE_CURRENT_SOURCE_DIR}/mocks
    SYSTEM ${PBLIB_DIR} # System include to suppress warnings from 3rd-party
    ${GENERATED_DIR}/c
)

# ------------------------------------------------------------------------
# Source files
# ------------------------------------------------------------------------
file(GLOB_RECURSE PROTOBUF_SOURCES ${PROTOBUF_DIR}/**/*.c)
file(GLOB COMMON_PROTOBUF_SOURCE ${PROTOBUF_DIR}/*.c)
file(GLOB PBLIB_SOURCES ${PBLIB_DIR}/*.c)
file(GLOB_RECURSE CPP_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../src/*.cpp)

# Main library with your actual source
add_library(MAIN_SOURCES_LIB
    ${CPP_SOURCES}
    ${PROTOBUF_SOURCES}
    ${COMMON_PROTOBUF_SOURCE}
)

target_compile_options(MAIN_SOURCES_LIB PRIVATE
    -pedantic
    -pedantic-errors
    -Wfloat-equal
    -Wredundant-decls
    -Wswitch-default
    -Wconversion
)

# ------------------------------------------------------------------------
# GoogleTest
# ------------------------------------------------------------------------
add_subdirectory(${LIBS_DIR}/googletest ${CMAKE_BINARY_DIR}/googletest-build)

# Gather test sources
file(GLOB TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# Create the test executable
add_executable(${This}
    main.cpp
    ${TEST_SOURCES}
    ${PBLIB_SOURCES}
)

# Link the test executable with GoogleTest and our library
target_link_libraries(${This}
    gmock
    gmock_main
    gtest
    pthread
    MAIN_SOURCES_LIB
)

# ------------------------------------------------------------------------
# Enable GoogleTest discovery
# ------------------------------------------------------------------------
include(GoogleTest)
gtest_discover_tests(${This})
