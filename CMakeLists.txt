cmake_minimum_required(VERSION 3.20)

project(EmergencyRadio LANGUAGES CXX)

if(NOT WIN32)
    message(FATAL_ERROR "Windows is required")
endif()

if(NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(FATAL_ERROR "GTA San Andreas requires Win32/x86")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

# RakHook — поддерживает SA-MP 0.3.7-R1
FetchContent_Declare(
    rakhook
    GIT_REPOSITORY https://github.com/imring/RakHook.git
    GIT_TAG master
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(rakhook)

# plugin-sdk только как headers/source tree.
# Мы НЕ собираем весь plugin-sdk.
FetchContent_Declare(
    plugin_sdk
    GIT_REPOSITORY https://github.com/DK22Pac/plugin-sdk.git
    GIT_TAG master
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(plugin_sdk)

add_library(EmergencyRadio SHARED
    dllmain.cpp
)

target_include_directories(EmergencyRadio PRIVATE

    ${rakhook_SOURCE_DIR}/include

    ${plugin_sdk_SOURCE_DIR}/plugin_sa
    ${plugin_sdk_SOURCE_DIR}/plugin_sa/game_sa
    ${plugin_sdk_SOURCE_DIR}/shared

)

target_link_libraries(EmergencyRadio PRIVATE
    rakhook::rakhook
)

set_target_properties(EmergencyRadio PROPERTIES
    OUTPUT_NAME "EmergencyRadio"
    SUFFIX ".asi"
)

if(MSVC)
    target_compile_options(EmergencyRadio PRIVATE
        /EHsc
        /MP
    )
endif()
