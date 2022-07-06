cmake_minimum_required(VERSION 3.15)
project(rec2 C)

option(KEEP_ACTIVE_IN_BACKGROUND "Keep the game active when alt-tabbing to another application" OFF)

if(KEEP_ACTIVE_IN_BACKGROUND)
    add_compile_definitions(KEEP_ACTIVE_IN_BACKGROUND)
endif()

if(MSVC)
    add_compile_options(/GL- /W3)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall)
endif()

if(EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
    include("${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
    conan_basic_setup(NO_OUTPUT_DIRS)
endif()

option(C2_STANDALONE "Build standalone Carmageddon 2 executable" OFF)

if(C2_STANDALONE)
    message(WARNING "Standalone Carmageddon 2 executable does not work")
endif()

if(C2_STANDALONE)
else()
    enable_language(CXX)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    add_subdirectory(src/hooks)
endif()

add_subdirectory(src/brender)
add_subdirectory(src/carma2)
