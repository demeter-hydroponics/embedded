cmake_minimum_required(VERSION 3.14)

set(This c_source_tests)

project(${This} C CXX)

# Specify the C and C++ standards
set(CMAKE_CXX_STANDARD 14) # Ensure at least C++14
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Og -g -Werror -Wextra -Wall -pedantic -pedantic-errors -Wfloat-equal -Wredundant-decls -Wswitch-default -pedantic -Wconversion")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Og -g -Werror -Wextra -Wall")

add_definitions(-DUNIT_TEST)

set(GENERATED_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../generated)
set(PROTOBUF_DIR ${GENERATED_DIR}/c)
set(PBLIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/protobuf/submodules/nanopb)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../inc)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../inc/driver)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../inc/device)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../inc/app)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../HAL)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/mocks)

include_directories(SYSTEM ${PBLIB_DIR}) # system in order to avoid the thing from issuing warnings
include_directories(${GENERATED_DIR}/c)

file(GLOB_RECURSE PROTOBUF_SOURCES ${PROTOBUF_DIR}/**/*.c)
file(GLOB COMMON_PROTOBUF_SOURCE ${PROTOBUF_DIR}/*.c)
file(GLOB PBLIB_SOURCES ${PBLIB_DIR}/*.c)

# Add the sources from ../src
file(GLOB_RECURSE CPP_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../src/*.cpp)

add_library(MAIN_SOURCES_LIB ${CPP_SOURCES} ${PROTOBUF_SOURCES} ${COMMON_PROTOBUF_SOURCE})
target_compile_options(MAIN_SOURCES_LIB PRIVATE -pedantic -pedantic-errors -Wfloat-equal -Wredundant-decls -Wswitch-default -pedantic -Wconversion)

# Define the libs directory variable
set(LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../libs)

# Add GoogleTest Submodule with explicit binary directory
add_subdirectory(${LIBS_DIR}/googletest ${CMAKE_BINARY_DIR}/googletest-build)

# Get all the test sources
file(GLOB TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# Create the test executable
add_executable(${This} main.cpp ${TEST_SOURCES} ${PBLIB_SOURCES})

# Link the test executable with GoogleTest, GoogleMock, and pthread
target_link_libraries(${This} gmock gmock_main gtest pthread MAIN_SOURCES_LIB)

# Enable GoogleTest CMake support
include(GoogleTest)
gtest_discover_tests(${This})
