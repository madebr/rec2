cmake_minimum_required(VERSION 3.15)
project(hooks)

set_property(GLOBAL PROPERTY HOOK_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

add_library(hooks_interface INTERFACE)
target_include_directories(hooks_interface INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>")

function(add_hook_injector TARGET VICTIMEXE)
    get_property(hook_source_dir GLOBAL PROPERTY HOOK_SOURCE_DIR)

    list(APPEND CMAKE_MODULE_PATH "${hook_source_dir}/cmake")
    find_package(Detours REQUIRED)

    enable_language(CXX)

    add_executable(${TARGET}
        "${hook_source_dir}/injector.cpp"
    )
    add_compile_definitions(${TARGET}
        PRIVATE
            "TARGET=\"$<TARGET_FILE_NAME:${TARGET}>\""
            "VICTIM=\"${VICTIMEXE}\""
    )
    target_link_libraries(${TARGET} PRIVATE Detours::Detours)
    if(MINGW)
        target_link_libraries(${TARGET} PRIVATE libwinpthread.a)
        target_link_options(${TARGET} PRIVATE -static-libgcc -static-libstdc++ -s)
    endif()

    set_target_properties(${TARGET}
        PROPERTIES
            CXX_STANDARD 17
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    )
endfunction()

function(add_hook_plugin TARGET)
    get_property(hook_source_dir GLOBAL PROPERTY HOOK_SOURCE_DIR)

    list(APPEND CMAKE_MODULE_PATH "${hook_source_dir}/cmake")
    find_package(Detours REQUIRED)

    add_library(${TARGET} MODULE
        ${ARGN}
        "${hook_source_dir}/hookmain.c"
        "${hook_source_dir}/hooks.cpp"
        "${hook_source_dir}/hooks.h"
    )
    if(MSVC)
        target_sources(${TARGET} PRIVATE "${hook_source_dir}/hook_msvc.def")
    elseif(MINGW)
        target_sources(${TARGET} PRIVATE "${hook_source_dir}/hook_mingw.def")
    endif()
    target_compile_definitions(${TARGET}
        PRIVATE
            "PLUGIN_NAME=\"$<TARGET_FILE_NAME:${TARGET}>\""
    )
    target_include_directories(${TARGET} PUBLIC "$<BUILD_INTERFACE:${hook_source_dir}>")
    target_link_libraries(${TARGET} PRIVATE Detours::Detours)
    if(MINGW)
        target_link_libraries(${TARGET} PRIVATE libwinpthread.a)
        target_link_options(${TARGET} PRIVATE -static-libgcc -static-libstdc++ -s)
    endif()

    set_target_properties(${TARGET}
        PROPERTIES
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
            PREFIX ""
            LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins"
    )
endfunction()
