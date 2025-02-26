add_library(c2_platform STATIC
    platform.h
    rec2_logging.c
)

add_library(c2_platform_brender STATIC
    platform.h
)

if(WIN32)
    target_sources(c2_platform_brender PRIVATE
        win32/win32_br_platform.c
    )
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
            DIRECTSOUND_VERSION=0x0500
    )
<<<<<<< HEAD
    find_library(DSOUND_LIBRARY NAMES dsound)
    find_library(WINMM_LIBRARY NAMES winmm)
    find_library(WS2_32_LIBRARY NAMES ws2_32)
    option(SOUND_ENABLED "Enable sound with DirectSound" ON)
    target_link_libraries(c2_platform
        PRIVATE
            DirectX::dinput
            DirectX::dxguid
            winmm
            ws2_32
            IForce2::IForce2
    )
    if(SOUND_ENABLED)
        target_link_libraries(c2_platform PRIVATE dsound)
        target_compile_definitions(c2_platform PRIVATE DSOUND_ENABLED)
    endif()
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
        brender_softrendr_float
        c2_common
        c2_hooks
)
target_include_directories(c2_platform_brender
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
)
target_link_libraries(c2_platform_brender
    PUBLIC
        brender
        c2_hooks
)

if(C2_STANDALONE)
    add_executable(rec2 WIN32
        $<TARGET_OBJECTS:brender>
        $<TARGET_OBJECTS:brender_softrendr_float>
        $<TARGET_OBJECTS:c2_common>
        $<TARGET_OBJECTS:c2_platform>
    )
    target_link_libraries(rec2 PRIVATE
        brender
        brender_softrendr_float
        c2_common
        c2_platform
    )
endif()
