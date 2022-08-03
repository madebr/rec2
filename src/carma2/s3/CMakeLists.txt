add_library(s3 INTERFACE)
target_include_directories(s3 INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
)
add_library(S3::S3 ALIAS s3)
