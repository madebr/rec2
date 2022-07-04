find_path(DETOURS_INCLUDE_PATH
    NAMES detours.h
#    PATH_SUFFIXES include
)

find_library(DETOURS_LIBRARY
    NAMES detours
#    PATH_SUFFIXES lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Detours
    REQUIRED_VARS DETOURS_LIBRARY DETOURS_INCLUDE_PATH
)

if(Detours_FOUND)
    if(NOT TARGET Detours::Detours)
        add_library(Detours::Detours STATIC IMPORTED)
        set_target_properties(Detours::Detours
            PROPERTIES
                IMPORTED_LOCATION "${DETOURS_LIBRARY}"
                INTERFACE_INCLUDE_DIRECTORIES "${DETOURS_INCLUDE_PATH}"
        )
    endif()
endif()
