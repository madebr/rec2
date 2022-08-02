if(CMAKE_C_COMPILER_ID MATCHES "MSVC")
    set(SMACKW32_IMPLIB "${CMAKE_CURRENT_BINARY_DIR}/smackw32.dll.lib")
    add_custom_command(
            OUTPUT "${SMACKW32_IMPLIB}"
            COMMAND ${CMAKE_AR} /NOLOGO ${CMAKE_STATIC_LINKER_FLAGS} /DEF:smackw32.def "/OUT:${SMACKW32_IMPLIB}"
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
            DEPENDS smackw32.def
    )
else()
    set(SMACKW32_IMPLIB "${CMAKE_CURRENT_BINARY_DIR}/libsmackw32.dll.a")
    add_custom_command(
            OUTPUT "${SMACKW32_IMPLIB}"
            COMMAND "${CMAKE_DLLTOOL}" ${CMAKE_STATIC_LINKER_FLAGS} --input-def smackw32.def --output-lib "${SMACKW32_IMPLIB}"
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
            DEPENDS smackw32.def
    )
endif()

add_custom_target(smackw32-implib DEPENDS "${SMACKW32_IMPLIB}")
target_sources(smackw32-implib PRIVATE smackw32.def)
set(SMACKW_LIB "${SMACKW32_IMPLIB}")
set(SMACKW_DEP "smackw32-implib")

if(SMACKW_LIB)
    add_library(smackw INTERFACE)
    target_link_libraries(smackw
        INTERFACE
            "${SMACKW_LIB}"
    )
    target_include_directories(smackw
        INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include"
    )
    add_dependencies(smackw ${SMACKW_DEP})
endif()
