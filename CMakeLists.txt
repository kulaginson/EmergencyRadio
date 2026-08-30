cmake_minimum_required(VERSION 3.20)

project(EmergencyRadio LANGUAGES CXX)

if(NOT WIN32)
    message(FATAL_ERROR "Windows is required")
endif()

if(NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(FATAL_ERROR "32-bit x86 build is required")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
    plugin_sdk
    GIT_REPOSITORY https://github.com/DK22Pac/plugin-sdk.git
    GIT_TAG master
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(plugin_sdk)

message(STATUS "plugin-sdk source: ${plugin_sdk_SOURCE_DIR}")
