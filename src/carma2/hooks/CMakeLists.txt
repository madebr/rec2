add_library(c2_hooks STATIC
    c2_hooks.c
    c2_hooks.h
)

target_include_directories(c2_hooks
    INTERFACE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
)
