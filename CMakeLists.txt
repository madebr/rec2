cmake_minimum_required(VERSION 3.15)

cmake_policy(SET CMP0091 NEW)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

project(rec2 C)

option(REC2_VENDORED "Use vendored dependencies" ON)
option(REC2_WERROR "Treat warnings as errors" OFF)

if(REC2_WERROR)
    if(MSVC)
        add_compile_options(/WX)
    else()
        add_compile_options(-Werror)
    endif()
endif()

option(C2_STANDALONE "Build standalone Carmageddon 2 executable" OFF)

if(C2_STANDALONE)
    message(WARNING "Standalone Carmageddon 2 executable does not work")
endif()

if(MSVC)
    add_compile_options(/GL- /W3)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall)
endif()

if(C2_STANDALONE)
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
