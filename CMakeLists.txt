cmake_minimum_required(VERSION 3.15)

cmake_policy(SET CMP0091 NEW)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

project(rec2 C)

option(KEEP_ACTIVE_IN_BACKGROUND "Keep the game active when alt-tabbing to another application" OFF)
option(REC2_VENDORED "Use vendored dependencies" ON)
option(REC2_WERROR "Treat warnings as errors" OFF)

if(MSVC AND REC2_VENDORED)
    add_subdirectory(external/dxsdk EXCLUDE_FROM_ALL)
else()
    if(MSVC)
        set(dxsdk_path "${CMAKE_CURRENT_SOURCE_DIR}/external/dxsdk/dxsdk/lib")
    endif()
    find_library(DINPUT_LIBRARY NAMES dinput PATHS "${dxsdk_path}")
    add_library(DirectX::dinput SHARED IMPORTED)
    set_property(TARGET DirectX::dinput PROPERTY IMPORTED_IMPLIB "${DINPUT_LIBRARY}")

    find_library(DXGUID_LIBRARY NAMES dxguid PATHS "${dxsdk_path}")
    add_library(DirectX::dxguid STATIC IMPORTED)
    set_property(TARGET DirectX::dxguid PROPERTY IMPORTED_LOCATION "${DXGUID_LIBRARY}")
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

if(REC2_VENDORED)
    add_subdirectory(external/libtiff EXCLUDE_FROM_ALL)
else()
    find_package(TIFF REQUIRED)
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

set(CARM2_LOCATION "" CACHE FILEPATH "Path of Carmageddon II")
if(CARM2_LOCATION)
    if(C2_STANDALONE)
        add_custom_target(copy_rec2
            COMMEND "Copying rec2 to \"${CARM2_LOCATION}\""
            COMMAND "${CMAKE_COMMAND}" -E copy "$<TARGET_FILE_NAME:rec2>" "${CARM2_LOCATION}/$<TARGET_FILE_NAME:rec2>"
        )
    else()
        add_custom_target(copy_hooks
            COMMENT "Copying rec2 hooks to \"${CARM2_LOCATION}\""
            COMMAND "${CMAKE_COMMAND}" -E make_directory "${CARM2_LOCATION}/plugins"
            COMMAND "${CMAKE_COMMAND}" -E copy "$<TARGET_FILE:rec2>" "${CARM2_LOCATION}/plugins/$<TARGET_FILE_NAME:rec2>"
            COMMAND "${CMAKE_COMMAND}" -E copy "$<TARGET_FILE:rec2-injector>" "${CARM2_LOCATION}/$<TARGET_FILE_NAME:rec2-injector>"
        )
    endif()
endif()
