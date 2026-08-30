cmake_minimum_required(VERSION 3.20)

project(EmergencyRadio LANGUAGES CXX)

if(NOT WIN32)
    message(FATAL_ERROR "Windows is required")
endif()

# GTA SA is 32-bit.
if(NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(FATAL_ERROR "EmergencyRadio must be built as Win32/x86")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(EmergencyRadio SHARED
    dllmain.cpp
)

target_compile_definitions(EmergencyRadio PRIVATE
    WIN32_LEAN_AND_MEAN
    _CRT_SECURE_NO_WARNINGS
)

target_compile_options(EmergencyRadio PRIVATE
    /EHsc
    /O2
)

set_target_properties(EmergencyRadio PROPERTIES
    OUTPUT_NAME "EmergencyRadio"
    SUFFIX ".asi"
    PREFIX ""
)

if(MSVC)
    set_property(
        TARGET EmergencyRadio
        PROPERTY MSVC_RUNTIME_LIBRARY
        "MultiThreadedDLL"
    )
endif()
