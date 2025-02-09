add_library(quicktime_dummy STATIC
    src/dummy.c
)
add_library(QuickTime::Dummy ALIAS quicktime_dummy)
target_include_directories(quicktime_dummy PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
