if(WIN32)
    set(PLATFORM_SOURCES
        win32/win32.c
        win32/win32.h
        win32/winmain.c
    )
    add_compile_definitions(DIRECTINPUT_VERSION=0x0200)
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

add_library(c2_platform STATIC
    ${PLATFORM_SOURCES}
)
target_include_directories(c2_platform
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
)
target_link_libraries(c2_platform
    PUBLIC
        brender
        c2_common
        c2_hooks
)

if(C2_STANDALONE)
    add_executable(rec2)
    target_link_libraries(rec2
        PRIVATE
            c2_common
            c2_hooks
            c2_platform
    )
endif()
