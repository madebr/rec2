add_library(brender INTERFACE
)

target_include_directories(brender
    INTERFACE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
)
