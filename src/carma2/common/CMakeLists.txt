add_library(c2_common STATIC
    errors.c
    errors.h
#    init.c
#    init.h
    main.c
    main.h
    loading.c
    loading.h

    drmem.c
    drmem.h
    utility.c
    utility.h
    structur.c
    structur.h
)
target_include_directories(c2_common
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
)
target_link_libraries(c2_common
    PUBLIC
        brender
        c2_hooks
        c2_platform
)
