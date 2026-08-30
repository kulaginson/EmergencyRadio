cmake_minimum_required(VERSION 3.20)

project(EmergencyRadio LANGUAGES CXX)

if(NOT WIN32)
    message(FATAL_ERROR "Windows is required")
endif()

if(NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(FATAL_ERROR "GTA SA requires a 32-bit build")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
    plugin_sdk
    GIT_REPOSITORY https://github.com/DK22Pac/plugin-sdk.git
    GIT_TAG a1df3a6
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(plugin_sdk)

add_library(EmergencyRadio SHARED
    dllmain.cpp
)

target_include_directories(EmergencyRadio PRIVATE
    ${plugin_sdk_SOURCE_DIR}/plugin_sa
    ${plugin_sdk_SOURCE_DIR}/plugin_sa/game_sa
    ${plugin_sdk_SOURCE_DIR}/shared
)

target_link_libraries(EmergencyRadio PRIVATE
    plugin_sa
)

set_target_properties(EmergencyRadio PROPERTIES
    OUTPUT_NAME "EmergencyRadio"
    SUFFIX ".asi"
)

if(MSVC)
    set_property(
        TARGET EmergencyRadio
        PROPERTY MSVC_RUNTIME_LIBRARY
        "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
    )
endif()
