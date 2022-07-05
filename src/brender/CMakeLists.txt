add_library(brender STATIC
    core/fw/devsetup.c
    core/fw/devsetup.h
    core/fw/diag.c
    core/fw/diag.h
    core/std/brstdlib.c
    core/std/brstdlib.h
    core/fw/fwsetup.c
    core/fw/fwsetup.h
)

target_include_directories(brender
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    PRIVATE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
)
target_link_libraries(brender
    PUBLIC
        c2_hooks
)
