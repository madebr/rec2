if(REC2_FIX_BUGS)
    add_definitions(-DREC2_FIX_BUGS)
endif()

add_library(rec2_headers INTERFACE)
target_include_directories(rec2_headers INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")
add_library(rec2::headers ALIAS rec2_headers)

add_subdirectory(hooks)
add_subdirectory(newcommon)
add_subdirectory(s3)
add_subdirectory(platform)
