add_library(brender STATIC
    core/fw/devlist.c
    core/fw/devlist.h
    core/fw/devsetup.c
    core/fw/devsetup.h
#    core/fw/diag.c
#    core/fw/diag.h
    core/fw/fwsetup.c
    core/fw/fwsetup.h
    core/fw/mem.c
    core/fw/mem.h

    core/pixelmap/pmdsptch.c
    core/pixelmap/pmdsptch.h

    #    core/std/brstdlib.c
    #    core/std/brstdlib.h

    core/v1db/dbsetup.c
    core/v1db/dbsetup.h
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
