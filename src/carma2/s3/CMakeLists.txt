add_library(s3 STATIC
    include/s3/s3.h

    src/3d.c
    src/3d.h
    src/audio.c
    src/audio.h
    src/resource.c
    src/resource.h
    src/sample.c
    src/sample.h
)
target_include_directories(s3
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
)
target_link_libraries(s3
    PUBLIC
        c2_hooks
        brender
        rec2_headers
        c2_platform
)
add_library(S3::S3 ALIAS s3)
