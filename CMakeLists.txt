# Copyright (C) 2021 by the INTELLI team (https://github.com/CpuModulation)

cmake_minimum_required(VERSION 3.14)
project(CpuModulation CXX)

# Judge the os system

if (UNIX AND NOT APPLE)
    add_definitions(-DUSELOG4CXX)
    MESSAGE(STATUS "Enable Log4CXX")
endif (UNIX AND NOT APPLE)

# Custom CMake find instructions and macros
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")
include(cmake/macros.cmake)
include(cmake/default.cmake)

# C++ Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#gcc 10 g++10
message(STATUS "sudo add-apt-repository 'deb http://mirrors.kernel.org/ubuntu hirsute main universe'")
message(STATUS "sudo apt-get update")
message(STATUS "sudo apt install gcc-11 g++-11")
message(STATUS "sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-11 11")
message(STATUS "sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-11 11")

# Set Optimization Flags
set(CMAKE_CXX_FLAGS "-std=c++20 -Wall -Werror=return-type -Werror=unused-variable -Werror=unused-parameter")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DNO_RACE_CHECK -DCpuModulation_DEBUG_MODE=1")
set(CMAKE_CXX_FLAGS_RELEASE "-Wno-ignored-qualifiers -Wno-sign-compare -O3")


message(STATUS "CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}")
message(STATUS "CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE}")



option(ENABLE_UNIT_TESTS "Enable unit tests" OFF)
message(STATUS "Enable testing: ${ENABLE_UNIT_TESTS}")

if (ENABLE_UNIT_TESTS)
    enable_testing()
    # Google Test
    include(FetchContent)

    FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG release-1.11.0
    )

    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    include(GoogleTest)
endif ()

# Print all used include directories
message(STATUS "INCLUDE_DIRS:")
get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach (dir ${dirs})
    message(STATUS "  - ${dir}")
endforeach ()

message(STATUS " Libraries: ${LIBRARIES}")

# Add Source Code
add_subdirectory(src)

# Add Library
get_sources(CpuModulation_SOURCE_FILES)
get_headers(CpuModulation_HEADER_FILES)
add_library(CpuModulation SHARED ${CpuModulation_SOURCE_FILES} ${CpuModulation_HEADER_FILES} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(CpuModulation PUBLIC "include")
target_include_directories(CpuModulation PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries(CpuModulation PUBLIC ${LIBRARIES})

#Add benchmarks with command
add_subdirectory(benchmark)

#Add tests (based on GTest suits)
#add_subdirectory(test)

install(DIRECTORY "include" DESTINATION "/CpuModulation" COMPONENT CpuModulation)

