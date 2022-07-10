add_library(brender STATIC
    core/fw/brbegin.c
    core/fw/brbegin.h
    core/fw/brlists.c
    core/fw/brlists.h
    core/fw/brprintf.c
    core/fw/brprintf.h
    core/fw/devlist.c
    core/fw/devlist.h
    core/fw/devsetup.c
    core/fw/devsetup.h
#    core/fw/diag.c
#    core/fw/diag.h
    core/fw/fwsetup.c
    core/fw/fwsetup.h
    core/fw/loader.c
    core/fw/loader.h
    core/fw/mem.c
    core/fw/mem.h
    core/fw/pattern.c
    core/fw/pattern.h
    core/fw/resource.c
    core/fw/resource.h
    core/fw/scratch.c
    core/fw/scratch.h

    core/math/matrix23.c
    core/math/matrix23.h

    core/pixelmap/pmdsptch.c
    core/pixelmap/pmdsptch.h

    core/std/brstddiag.c
    core/std/brstddiag.h
    core/std/brstdlib.c
    core/std/brstdlib.h
    core/std/logwrite.c
    core/std/logwrite.h

    core/v1db/dbsetup.c
    core/v1db/dbsetup.h
    core/v1db/def_mdl.c
    core/v1db/def_mdl.h
    core/v1db/regsupt.c
    core/v1db/regsupt.h
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
