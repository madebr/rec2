add_library(c2_platform STATIC
    platform.h
    rec2_logging.c
)

if(WIN32)
    target_sources(c2_platform PRIVATE
        win32/win32_ssdx.c
        win32/win32_ssdx.h
        win32/win32_dinput.c
        win32/win32_dinput.h
        win32/win32.c
        win32/win32.h
        win32/win32net.c
        win32/win32net.h
        win32/winmain.c
    )
    target_compile_definitions(c2_platform
        PUBLIC
            DIRECTINPUT_VERSION=0x0500
    )
    find_library(DSOUND_LIBRARY NAMES dsound)
    target_link_libraries(c2_platform
        PRIVATE
            DirectX::dinput
            DirectX::dxguid
            ${DSOUND_LIBRARY}
    )
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

target_include_directories(c2_platform
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
)
target_link_libraries(c2_platform
    PUBLIC
        brender
        brender_softrendr
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
