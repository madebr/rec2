if(REC2_PLATFORM_WIN32)
    add_library(Smackw32::Smackw32 SHARED IMPORTED GLOBAL)
    set_property(TARGET Smackw32::Smackw32 PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include")
    set_property(TARGET Smackw32::Smackw32 PROPERTY IMPORTED_IMPLIB "${CMAKE_CURRENT_LIST_DIR}/lib/x86/smackw32.lib")
else()
    add_library(smackw32_smackw32_dummy STATIC
        src/dummy/dummy.c
    )
    target_include_directories(smackw32_smackw32_dummy PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include")
    add_library(Smackw32::Smackw32 ALIAS smackw32_smackw32_dummy)
endif()
