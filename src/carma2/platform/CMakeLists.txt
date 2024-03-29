add_library(c2_platform STATIC
    platform.h
)

if(WIN32)
    target_sources(c2_platform PRIVATE
        win32/win32_dinput.c
        win32/win32_dinput.h
        win32/win32.c
        win32/win32.h
        win32/winmain.c
    )
    target_compile_definitions(c2_platform
        PUBLIC
            DIRECTINPUT_VERSION=0x0500
    )
    find_library(DINPUT_LIBRARY NAMES dinput)
    find_library(DXGUID_LIBRARY NAMES dxguid)
    if(NOT DINPUT_LIBRARY OR NOT DXGUID_LIBRARY)
        message(FATAL_ERROR "Could not find dinput and/or dxguid (provided by older DirectX SDK's)")
    endif()
    target_link_libraries(c2_platform
        PRIVATE
            ${DINPUT_LIBRARY}
            ${DXGUID_LIBRARY}
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
