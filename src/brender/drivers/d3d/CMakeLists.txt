
add_library(brender_driver_d3d SHARED
    d3d.def
    backend.c
    backend.h
    devclut.c
    devclut.h
    device.c
    device.h
    devpixmp.c
    devpixmp.h
    devpmsys.c
    devpmsys.h
    driver.c
    match.c
    match.h
    object.c
    object.h
    outfcty.c
    outfcty.h
    plib.c
    plib.h
    prim_l15.c
    prim_l15.h
    prim_p15.h
    prim_p15.c
    prim_t15.c
    prim_t15.h
    pstate.c
    pstate.h
    renderfuncs.c
    renderfuncs.h
    sbuffer.c
    sbuffer.h
    texture.c
    texture.h
    timestmp.c
    timestmp.h
    types.h
)
target_compile_definitions(brender_driver_d3d PRIVATE DIRECTDRAW_VERSION=0x500)
#target_include_directories(brender_driver_d3d
#    PRIVATE
#        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
#        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
#)
target_link_libraries(brender_driver_d3d PRIVATE BRender::ddi)
target_link_libraries(brender_driver_d3d
    PRIVATE
        brender-brcore1-imp
        brender-brpmap1-imp
        brender-brhost1-imp
        c2_hooks
        DirectX::d3dim
)
target_compile_definitions(brender_driver_d3d
    PRIVATE
        BASED_FLOAT
)
set_property(TARGET brender_driver_d3d PROPERTY PREFIX "")

if(REC2_MATCHING)
    set(CMAKE_C_FLAGS "/W3 /GX /DWIN32 /D_WINDOWS")
    set(CMAKE_C_FLAGS_DEBUG "/Gm /Od /D_DEBUG /MLd")
    set(CMAKE_C_FLAGS_RELEASE "/O2 /DNDEBUG /ML")
    set(CMAKE_C_FLAGS_RELWITHDEBINFO "/O2 /DNDEBUG /ML")
    set(CMAKE_C_FLAGS_MINSIZEREL "/Os /DNDEBUG /ML")
endif()
