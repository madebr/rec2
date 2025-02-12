set(ZLIB_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(zlib EXCLUDE_FROM_ALL)

add_library(ZLIB::ZLIB ALIAS zlibstatic)
