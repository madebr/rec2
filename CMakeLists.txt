cmake_minimum_required(VERSION 3.15)

cmake_policy(SET CMP0091 NEW)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

project(rec2 C)

include(CMakeDependentOption)

message(STATUS "Compiler: ${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION}")
# Carmageddon 2 was built with Visual Studio 5 (MSVC 11.0)
if(WIN32 AND CMAKE_C_COMPILER_ID STREQUAL "MSVC" AND CMAKE_C_COMPILER_VERSION VERSION_EQUAL "11.0")
    option(REC2_MATCHING "Matching build" TRUE)
else()
    set(REC2_MATCHING FALSE CACHE BOOL "Matching build" FORCE)
endif()

if(REC2_MATCHING)
    message(STATUS "Matching build")
    add_compile_definitions(-DREC2_MATCHING)
endif()

if(REC2_MATCHING)
    include_directories("${PROJECT_SOURCE_DIR}/src/msvc500/include")
    add_compile_definitions(-DREC2_MATCHING)
    add_compile_options(-Gr) # __fastcall calling convention
endif()

set(REC2_PLATFORM_CHOICES)
if(WIN32)
    list(APPEND REC2_PLATFORM_CHOICES "WIN32")
endif()
if(NOT REC2_MATCHING)
    list(APPEND REC2_PLATFORM_CHOICES "SDL3")
endif()
if(WIN32 AND CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(REC2_PLATFORM_DEFAULT "WIN32")
else()
    set(REC2_PLATFORM_DEFAULT "SDL3")
endif()
set(REC2_PLATFORM "${REC2_PLATFORM_DEFAULT}" CACHE STRING "rec2 platform")
set_property(CACHE REC2_PLATFORM PROPERTY STRINGS "${REC2_PLATFORM_CHOICES}")
if(NOT REC2_PLATFORM IN_LIST REC2_PLATFORM_CHOICES)
    message(FATAL_ERROR "Invalid REC2_PLATFORM=${REC2_PLATFORM} (choices=${REC2_PLATFORM_CHOICES})")
endif()
message(STATUS "REC2_PLATFORM=${REC2_PLATFORM} (choices=${REC2_PLATFORM_CHOICES})")
foreach(choice IN LISTS REC2_PLATFORM_CHOICES)
    if(REC2_PLATFORM STREQUAL "${choice}")
        set(REC2_PLATFORM_${choice} TRUE)
    else()
        set(REC2_PLATFORM_${choice} FALSE)
    endif()
endforeach()

option(KEEP_ACTIVE_IN_BACKGROUND "Keep the game active when alt-tabbing to another application" OFF)
cmake_dependent_option(REC2_VENDORED "Use vendored dependencies" ON "NOT REC2_MATCHING" ON)
option(REC2_WERROR "Treat warnings as errors" OFF)
option(REC2_FIX_BUGS "Fix rec2 bugs" ON)

find_package(Python3 COMPONENTS Interpreter)
set(GENDUMPER_AVAILABLE FALSE)
if(Python3_Interpreter_FOUND)
    execute_process(
        COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/scripts/gendumper.py" --help
        RESULT_VARIABLE RES_GENDUMPER_HELP
        OUTPUT_VARIABLE stdout_GENDUMPER_HELP
        ERROR_VARIABLE stderr_GENDUMPER_HELP
    )
    if(RES_GENDUMPER_HELP EQUAL 0)
        set(GENDUMPER_AVAILABLE TRUE)
    endif()
endif()
cmake_dependent_option(REC2_DUMPERS "Build struct/enum dumpers" FALSE GENDUMPER_AVAILABLE FALSE)

if(NOT MSVC)
    add_compile_options(-Wno-format-overflow)
endif()

if(REC2_VENDORED)
    add_subdirectory(external/libtiff EXCLUDE_FROM_ALL)
    add_subdirectory(external/zlib EXCLUDE_FROM_ALL)
else()
    find_package(TIFF REQUIRED)
    find_package(ZLIB REQUIRED)
endif()

add_subdirectory(external/dummyqt)
add_subdirectory(external/iforce2)
add_subdirectory(external/smackw32)

if(MSVC AND REC2_VENDORED)
    add_subdirectory(external/dxsdk EXCLUDE_FROM_ALL)
else()
    if(MSVC)
        set(dxsdk_path "${CMAKE_CURRENT_SOURCE_DIR}/external/dxsdk/dxsdk/lib")

        add_library(DirectX::dinput SHARED IMPORTED)
        find_library(DINPUT_LIBRARY NAMES dinput PATHS "${dxsdk_path}")
        set_property(TARGET DirectX::dinput PROPERTY IMPORTED_IMPLIB "${DINPUT_LIBRARY}")

        add_library(DirectX::dxguid SHARED IMPORTED)
        find_library(DXGUID_LIBRARY NAMES dxguid PATHS "${dxsdk_path}")
        set_property(TARGET DirectX::dxguid PROPERTY IMPORTED_IMPLIB "${DXGUID_LIBRARY}")

        add_library(DirectX::d3dim SHARED IMPORTED)
        find_library(D3DIM_LIBRARY NAMES d3dim PATHS "${dxsdk_path}")
        set_property(TARGET DirectX::d3dim PROPERTY IMPORTED_IMPLIB "${D3DIM_LIBRARY}")
    else()
        add_library(DirectX_dinput INTERFACE)
        target_link_libraries(DirectX_dinput INTERFACE "dinput")
        add_library(DirectX::dinput ALIAS DirectX_dinput)

        add_library(DirectX_dxguid INTERFACE)
        target_link_libraries(DirectX_dxguid INTERFACE "dxguid")
        add_library(DirectX::dxguid ALIAS DirectX_dxguid)

        add_library(DirectX_d3dim INTERFACE)
        target_link_libraries(DirectX_d3dim INTERFACE "d3dim")
        add_library(DirectX::d3dim ALIAS DirectX_d3dim)
    endif()
endif()

if(REC2_WERROR)
    if(MSVC)
        add_compile_options(/WX)
    else()
        add_compile_options(-Werror)
    endif()
endif()

if(KEEP_ACTIVE_IN_BACKGROUND)
    add_compile_definitions(KEEP_ACTIVE_IN_BACKGROUND)
endif()


cmake_dependent_option(REC2_STANDALONE "Build standalone Carmageddon 2 executable" OFF "REC2_PLATFORM_WIN32;NOT REC2_MATCHING" ON)
if(REC2_STANDALONE)
    add_compile_definitions(-DREC2_STANDALONE)
endif()
cmake_dependent_option(REC2_ABORT_UNIMPLEMENTED "abort() on unimplemented functions" OFF REC2_STANDALONE OFF)
if(REC2_ABORT_UNIMPLEMENTED)
    add_compile_definitions(-DREC2_ABORT_UNIMPLEMENTED)
endif()

if(MSVC)
    add_compile_options(/GL- /W3)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wno-format-overflow)
endif()

add_library(hooks-header-only INTERFACE)
target_include_directories(hooks-header-only INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
if(REC2_STANDALONE)
else()
    enable_language(CXX)
    if(NOT MSVC)
        add_link_options(-Wl,--disable-stdcall-fixup)
    endif()

    if(REC2_VENDORED)
        add_subdirectory(external/detours EXCLUDE_FROM_ALL)
    endif()
    add_subdirectory(src/hooks)
endif()

add_subdirectory(src/brender)
add_subdirectory(src/carma2)
add_subdirectory(src/launcher)

set(CARM2_LOCATION "" CACHE FILEPATH "Path of Carmageddon II")
if(CARM2_LOCATION)
    add_custom_target(copy_rec2
        COMMEND "Copying rec2 to \"${CARM2_LOCATION}\""
        COMMAND "${CMAKE_COMMAND}" -E copy "$<TARGET_FILE_NAME:rec2>" "${CARM2_LOCATION}/$<TARGET_FILE_NAME:rec2>"
    )
endif()

if(REC2_MATCHING)
    include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/reccmp.cmake")
    reccmp_configure()

    reccmp_add_target(rec2 ID CARMA2_HW)
    reccmp_add_target(brender_driver_d3d ID D3D)
    reccmp_add_target(carma2 ID LAUNCHER)
endif()
