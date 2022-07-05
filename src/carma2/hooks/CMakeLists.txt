if(C2_STANDALONE)
    add_library(c2_hooks INTERFACE)
else()
    add_library(c2_hooks STATIC
        c2_hooks.h
        c2_stdio.c
        c2_stdio.h
        c2_stdlib.h
        c2_string.c
        c2_string.h
        win32_hooks.c
    )
    target_compile_definitions(c2_hooks PUBLIC C2_HOOKS_ENABLED)
    target_link_libraries(c2_hooks PUBLIC hooks_interface)
endif()

target_include_directories(c2_hooks
    INTERFACE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
)

if(NOT C2_STANDALONE)
    add_hook_injector(rec2_inject "CARMA2_HW.EXE")

    add_hook_plugin(rec2)
    target_sources(rec2 PRIVATE
        $<TARGET_OBJECTS:brender>
        $<TARGET_OBJECTS:c2_common>
        $<TARGET_OBJECTS:c2_hooks>
        $<TARGET_OBJECTS:c2_platform>
    )
    target_link_libraries(rec2
        PRIVATE
            brender
            c2_common
            c2_hooks
            c2_platform
    )
endif()
