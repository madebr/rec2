add_library(s3 STATIC
    include/s3/s3.h

    include/s3/internal/3d.h
    include/s3/internal/audio.h
    include/s3/internal/resource.h
    include/s3/internal/s3sound.h
    include/s3/internal/sample.h

    src/3d.c
    src/audio.c
    src/resource.c
    src/s3sound.c
    src/sample.c
)
target_include_directories(s3
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    PRIVATE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/s3/internal>"
)
target_link_libraries(s3
    PUBLIC
        c2_hooks
        BRender::Headers
        rec2_headers
        c2_platform
)
add_library(S3::S3 ALIAS s3)
if(NOT MSVC)
    target_link_libraries(s3 PRIVATE m)
endif()

if(REC2_MATCHING)
    set(CMAKE_C_FLAGS "/W3 /GX /DWIN32 /D_WINDOWS")
    set(CMAKE_C_FLAGS_DEBUG "/Gm /Od /D_DEBUG /MLd")
    set(CMAKE_C_FLAGS_RELEASE "/O0 /DNDEBUG /ML")
    set(CMAKE_C_FLAGS_RELWITHDEBINFO "/DNDEBUG /ML")
    set(CMAKE_C_FLAGS_MINSIZEREL "/DNDEBUG /ML")
endif()
