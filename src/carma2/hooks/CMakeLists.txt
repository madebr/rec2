if(C2_STANDALONE)
    add_library(c2_hooks INTERFACE)
else()
    add_library(c2_hooks STATIC
        c2_ctype.c
        c2_ctype.h
        c2_hooks.c
        c2_hooks.h
        c2_malloc.c
        c2_malloc.h
        c2_stdio.c
        c2_stdio.h
        c2_stdlib.c
        c2_stdlib.h
        c2_string.c
        c2_string.h
        c2_time.c
        c2_time.h
        win32_hooks.c
    )
    add_subdirectory(smackw32)
    target_compile_definitions(c2_hooks PUBLIC C2_HOOKS_ENABLED)
    target_link_libraries(c2_hooks
        PUBLIC
            hooks_interface
            smackw
    )
endif()

target_include_directories(c2_hooks
    INTERFACE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
)

if(NOT C2_STANDALONE)
    add_hook_injector(rec2-injector)

    add_hook_plugin(rec2)
    target_compile_definitions(rec2 PRIVATE "HOOK_INIT_FUNCTION=c2_hook_init")
    target_compile_definitions(rec2 PRIVATE "HOOK_DEINIT_FUNCTION=c2_hook_deinit")
    target_sources(rec2 PRIVATE
        $<TARGET_OBJECTS:brender>
        $<TARGET_OBJECTS:brender_softrendr>
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
