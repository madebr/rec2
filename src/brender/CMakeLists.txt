add_library(brender_headers INTERFACE)
add_library(BRender::Headers ALIAS brender_headers)
target_include_directories(brender_headers INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")


add_library(brender-static STATIC
    $<TARGET_OBJECTS:c2_platform_brender>

    core/fw/assocarr.c
    core/fw/assocarr.h
    core/fw/brbegin.c
    core/fw/brbegin.h
    core/fw/brbhook.c
    core/fw/brbhook.h
    core/fw/brlists.c
    core/fw/brlists.h
    core/fw/brprintf.c
    core/fw/brprintf.h
    core/fw/brqsort.c
    core/fw/brqsort.h
    core/fw/bswap.c
    core/fw/bswap.h
    core/fw/datafile.c
    core/fw/datafile.h
    core/fw/devlist.c
    core/fw/devlist.h
    core/fw/devsetup.c
    core/fw/devsetup.h
    core/fw/diag.c
    core/fw/diag.h
    core/fw/epiprolog.c
    core/fw/epiprolog.h
    core/fw/error.c
    core/fw/error.h
    core/fw/file.c
    core/fw/file.h
    core/fw/fwsetup.c
    core/fw/fwsetup.h
    core/fw/genfile.c
    core/fw/genfile.h
    core/fw/image.c
    core/fw/image.h
    core/fw/lexer.c
    core/fw/lexer.h
    core/fw/loader.c
    core/fw/loader.h
    core/fw/mem.c
    core/fw/mem.h
    core/fw/object.c
    core/fw/object.h
    core/fw/objectc.c
    core/fw/objectc.h
    core/fw/pattern.c
    core/fw/pattern.h
    core/fw/register.c
    core/fw/register.h
    core/fw/resource.c
    core/fw/resource.h
    core/fw/resreg.c
    core/fw/resreg.h
    core/fw/scratch.c
    core/fw/scratch.h
    core/fw/token.c
    core/fw/token.h
    core/fw/tokenval.c
    core/fw/tokenval.h

    core/host/cpuid.c
    core/host/cpuid.h
    core/host/farmem.c
    core/host/farmem.h
    core/host/himage.c
    core/host/himage.h
    core/host/hook.c
    core/host/hook.h
    core/host/hostcfg.c
    core/host/hostcfg.h
    core/host/hstsetup.c
    core/host/hstsetup.h
    core/host/memmgmt.c
    core/host/memmgmt.h
    core/host/prot.c
    core/host/prot.h
    core/host/real.c
    core/host/real.h
    core/host/realmem.c
    core/host/realmem.h

    core/math/angles.c
    core/math/angles.h
    core/math/fixed.c
    core/math/fixed.h
    core/math/matrix23.c
    core/math/matrix23.h
    core/math/matrix34.c
    core/math/matrix34.h
    core/math/matrix4.c
    core/math/matrix4.h
    core/math/plane.c
    core/math/plane.h
    core/math/quat.c
    core/math/quat.h
    core/math/transfrm.c
    core/math/transfrm.h
    core/math/vector.c
    core/math/vector.h

    core/pixelmap/font3x5f.c
    core/pixelmap/font3x5f.h
    core/pixelmap/font4x6p.c
    core/pixelmap/font4x6p.h
    core/pixelmap/font7x9p.c
    core/pixelmap/font7x9p.h
    core/pixelmap/fontptrs.c
    core/pixelmap/fontptrs.h
    core/pixelmap/genclip.c
    core/pixelmap/genclip.h
    core/pixelmap/gencopy.c
    core/pixelmap/gencopy.h
    core/pixelmap/pixelmap.c
    core/pixelmap/pixelmap.h
    core/pixelmap/pmdsptch.c
    core/pixelmap/pmdsptch.h
    core/pixelmap/pmgen.c
    core/pixelmap/pmgen.h
    core/pixelmap/pmfile.c
    core/pixelmap/pmfile.h
    core/pixelmap/pmmem.c
    core/pixelmap/pmmem.h
    core/pixelmap/pmmemops.c
    core/pixelmap/pmmemops.h
    core/pixelmap/pmnull.c
    core/pixelmap/pmnull.h
    core/pixelmap/pmsetup.c
    core/pixelmap/pmsetup.h
    core/pixelmap/pmutils.c
    core/pixelmap/pmutils.h

    core/std/brexcept.c
    core/std/brexcept.h
    core/std/brmath.c
    core/std/brmath.h
    core/std/brstddiag.c
    core/std/brstddiag.h
    core/std/brstdfile.c
    core/std/brstdfile.h
    core/std/brstdlib.c
    core/std/brstdlib.h
    core/std/brstdmem.c
    core/std/brstdmem.h
    core/std/logwrite.c
    core/std/logwrite.h

    core/v1db/actsupt.h
    core/v1db/actsupt.c
    core/v1db/chunkids.h
    core/v1db/custsupt.c
    core/v1db/custsupt.h
    core/v1db/dbsetup.c
    core/v1db/dbsetup.h
    core/v1db/def_mat.c
    core/v1db/def_mat.h
    core/v1db/def_mdl.c
    core/v1db/def_mdl.h
    core/v1db/def_otab.c
    core/v1db/def_otab.h
    core/v1db/enables.c
    core/v1db/enables.h
    core/v1db/matsupt.c
    core/v1db/matsupt.h
    core/v1db/modrend.c
    core/v1db/modrend.h
    core/v1db/modsupt.c
    core/v1db/modsupt.h
    core/v1db/otable.c
    core/v1db/otable.h
    core/v1db/prepmap.c
    core/v1db/prepmap.h
    core/v1db/prepmatl.c
    core/v1db/prepmatl.h
    core/v1db/prepmesh.c
    core/v1db/prepmesh.h
    core/v1db/preptab.c
    core/v1db/preptab.h
    core/v1db/render.c
    core/v1db/render.h
    core/v1db/regsupt.c
    core/v1db/regsupt.h
    core/v1db/stackids.h
    core/v1db/sys_conf.c
    core/v1db/sys_conf.h
    core/v1db/v1dbfile.c
    core/v1db/v1dbfile.h
)
if(MSVC)
    set_property(TARGET brender-static PROPERTY ARCHIVE_OUTPUT_NAME "libbrender")
endif()

if(REC2_FIX_BUGS)
    target_compile_definitions(brender-static PRIVATE BRENDER_FIX_BUGS)
endif()
target_include_directories(brender-static
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    PRIVATE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
)
target_link_libraries(brender-static
    PUBLIC
        c2_hooks
        c2_platform
)

add_library(brender_softrendr_float STATIC
    drivers/softrend/alpha.c
    drivers/softrend/alpha.h
    drivers/softrend/clip.c
    drivers/softrend/clip.h
    drivers/softrend/convert.c
    drivers/softrend/convert.h
    drivers/softrend/device.c
    drivers/softrend/device.h
    drivers/softrend/driver.c
    drivers/softrend/faceops.c
    drivers/softrend/faceops.h
    drivers/softrend/genrend.c
    drivers/softrend/genrend.h
    drivers/softrend/gprim.c
    drivers/softrend/gprim.h
    drivers/softrend/gv1buckt.c
    drivers/softrend/gv1buckt.h
    drivers/softrend/gv1model.c
    drivers/softrend/gv1model.h
    drivers/softrend/heap.c
    drivers/softrend/heap.h
    drivers/softrend/light24.c
    drivers/softrend/light24.h
    drivers/softrend/light8.c
    drivers/softrend/light8.h
    drivers/softrend/lighting.c
    drivers/softrend/lighting.h
    drivers/softrend/lightmac.h
    drivers/softrend/mapping.c
    drivers/softrend/mapping.h
    drivers/softrend/object.c
    drivers/softrend/object.h
    drivers/softrend/onscreen.c
    drivers/softrend/onscreen.h
    drivers/softrend/renderer.c
    drivers/softrend/renderer.h
    drivers/softrend/rendfcty.c
    drivers/softrend/rendfcty.h
    drivers/softrend/rndstate.c
    drivers/softrend/rndstate.h
    drivers/softrend/setup.c
    drivers/softrend/setup.h
    drivers/softrend/sstate.c
    drivers/softrend/sstate.h
    drivers/softrend/state.c
    drivers/softrend/state.h
    drivers/softrend/subdiv.c
    drivers/softrend/subdiv.h
    drivers/softrend/timestmp.h
    drivers/softrend/types.h
    drivers/softrend/v1model.c
    drivers/softrend/v1model.h
    drivers/softrend/zsrmacro.h
)
target_include_directories(brender_softrendr_float
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
)
target_link_libraries(brender_softrendr_float
    PRIVATE
        brender-static
        c2_hooks
)
target_compile_definitions(brender_softrendr_float
    PRIVATE
        BrDrv1Begin=BrDrv1SoftRendBegin
        BASED_FLOAT
)

if(REC2_STANDALONE)
    add_library(brender-shared SHARED
        dll/brender1.def
        $<TARGET_OBJECTS:c2_platform_brender>
        $<TARGET_OBJECTS:brender-static>
        $<TARGET_OBJECTS:brender_softrendr_float>
    )
    set_property(TARGET brender-shared PROPERTY PREFIX "")
    set_property(TARGET brender-shared PROPERTY OUTPUT_NAME "brender")

    add_library(brender-brcore1-shared SHARED
        dll/dllmain.c
        dll/brcore1.def
    )
    target_link_libraries(brender-brcore1-shared PRIVATE brender-shared)
    set_property(TARGET brender-brcore1-shared PROPERTY PREFIX "")
    set_property(TARGET brender-brcore1-shared PROPERTY OUTPUT_NAME "brcore1")
    if(MSVC)
        target_compile_options(brender-brcore1-shared PRIVATE "/Zl")
        target_link_options(brender-brcore1-shared PRIVATE "/ENTRY:DllMain")
    endif()

    add_library(brender-brpmap1-shared SHARED
        dll/dllmain.c
        dll/brpmap1.def
    )
    target_link_libraries(brender-brpmap1-shared PRIVATE brender-shared)
    set_property(TARGET brender-brpmap1-shared PROPERTY PREFIX "")
    set_property(TARGET brender-brpmap1-shared PROPERTY OUTPUT_NAME "brpmap1")
    if(MSVC)
        target_compile_options(brender-brpmap1-shared PRIVATE "/Zl")
        target_link_options(brender-brpmap1-shared PRIVATE "/ENTRY:DllMain")
    endif()

    add_library(brender-brhost1-shared SHARED
        dll/dllmain.c
        dll/brhost1.def
    )
    target_link_libraries(brender-brhost1-shared PRIVATE brender-shared)
    set_property(TARGET brender-brhost1-shared PROPERTY PREFIX "")
    set_property(TARGET brender-brhost1-shared PROPERTY OUTPUT_NAME "brhost1")
    if(MSVC)
        target_compile_options(brender-brhost1-shared PRIVATE "/Zl")
        target_link_options(brender-brhost1-shared PRIVATE "/ENTRY:DllMain")
    endif()

    if(REC2_PLATFORM_WIN32)
        add_library(brender_driver_d3d SHARED
            drivers/d3d/d3d.def
            drivers/d3d/backend.c
            drivers/d3d/backend.h
            drivers/d3d/devclut.c
            drivers/d3d/devclut.h
            drivers/d3d/device.c
            drivers/d3d/device.h
            drivers/d3d/devpixmp.c
            drivers/d3d/devpixmp.h
            drivers/d3d/devpmsys.c
            drivers/d3d/devpmsys.h
            drivers/d3d/driver.c
            drivers/d3d/match.c
            drivers/d3d/match.h
            drivers/d3d/object.c
            drivers/d3d/object.h
            drivers/d3d/outfcty.c
            drivers/d3d/outfcty.h
            drivers/d3d/plib.c
            drivers/d3d/plib.h
            drivers/d3d/prim_l15.c
            drivers/d3d/prim_l15.h
            drivers/d3d/prim_p15.h
            drivers/d3d/prim_p15.c
            drivers/d3d/prim_t15.c
            drivers/d3d/prim_t15.h
            drivers/d3d/pstate.c
            drivers/d3d/pstate.h
            drivers/d3d/renderfuncs.c
            drivers/d3d/renderfuncs.h
            drivers/d3d/sbuffer.c
            drivers/d3d/sbuffer.h
            drivers/d3d/texture.c
            drivers/d3d/texture.h
            drivers/d3d/timestmp.c
            drivers/d3d/timestmp.h
            drivers/d3d/types.h
        )
        target_compile_definitions(brender_driver_d3d PRIVATE DIRECTDRAW_VERSION=0x500)
        target_include_directories(brender_driver_d3d
            PRIVATE
                "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
                "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        )
        target_link_libraries(brender_driver_d3d
            PRIVATE
                brender-brcore1-shared
                brender-brpmap1-shared
                brender-brhost1-shared
                c2_hooks
        )
        target_compile_definitions(brender_driver_d3d
            PRIVATE
                BASED_FLOAT
        )
        set_property(TARGET brender_driver_d3d PROPERTY PREFIX "")
        set_property(TARGET brender_driver_d3d PROPERTY OUTPUT_NAME "d3d_rec2")
    endif()

    add_library(BRender::BRender ALIAS brender-shared)
else()
    add_library(BRender::BRender ALIAS brender-static)
endif()
