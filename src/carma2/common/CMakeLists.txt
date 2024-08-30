add_library(rec2_headers INTERFACE)
target_include_directories(rec2_headers INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>"
)

add_library(c2_common STATIC
    ../include/rec2_macros.h
    ../include/rec2_types.h

    animation.c
    animation.h
    brucetrk.c
    brucetrk.h
    car.c
    car.h
    controls.c
    controls.h
    crush.c
    crush.h
    cutscene.c
    cutscene.h
    demo.c
    demo.h
    depth.c
    depth.h
    drfile.c
    drfile.h
    drmem.c
    drmem.h
    displays.c
    displays.h
    drone.c
    drone.h
    errors.c
    errors.h
    explosions.c
    explosions.h
    finteray.c
    finteray.h
    flicplay.c
    flicplay.h
    font.c
    font.h
    frontend.c
    frontend.h
    frontend_controls.c
    frontend_controls.h
    frontend_credits.c
    frontend_credits.h
    frontend_main.c
    frontend_main.h
    frontend_loadgame.c
    frontend_loadgame.h
    frontend_netsync.c
    frontend_netsync.h
    frontend_network.c
    frontend_network.h
    frontend_networksummary.c
    frontend_networksummary.h
    frontend_newgame.c
    frontend_newgame.h
    frontend_options.c
    frontend_options.h
    frontend_quit.c
    frontend_quit.h
    frontend_startgame.c
    frontend_startgame.h
    frontend_wrecks.c
    frontend_wrecks.h
    globvars.c
    globvars.h
    globvrbm.c
    globvrbm.h
    globvrkm.c
    globvrkm.h
    globvrme.c
    globvrme.h
    globvrpb.c
    globvrpb.h
    grafdata.c
    grafdata.h
    graphics.c
    graphics.h
    init.c
    init.h
    intrface.c
    intrface.h
    input.c
    input.h
    joystick.c
    joystick.h
    loading.c
    loading.h
    loadsave.c
    loadsave.h
    main.c
    main.h
    mainloop.c
    mainloop.h
    mainmenu.c
    mainmenu.h
    netgame.c
    netgame.h
    network.c
    network.h
    newgame.c
    newgame.h
    oil.c
    oil.h
    opponent.c
    opponent.h
    options.c
    options.h
    pedestrn.c
    pedestrn.h
    piping.c
    piping.h
    polyfont.c
    polyfont.h
    pratcam.c
    pratcam.h
    physics.c
    physics.h
    powerups.h
    powerups.c
    racemem.c
    racemem.h
    racestrt.c
    racestrt.h
    racesumm.c
    racesumm.h
    raycast.c
    raycast.h
    replay.c
    replay.h
    shrapnel.c
    shrapnel.h
    smashing.c
    smashing.h
    sound.c
    sound.h
    spark.c
    spark.h
    skidmark.c
    skidmark.h
    structur.c
    structur.h
    temp.c
    temp.h
    timers.c
    timers.h
    tinted.c
    tinted.h
    trig.c
    trig.h
    utility.c
    utility.h
    video.c
    video.h
    world.c
    world.h
)
target_include_directories(c2_common
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
)
target_link_libraries(c2_common
    PUBLIC
        rec2_headers
        brender
        c2_hooks
        c2_platform
        S3::S3
        TIFF::TIFF
)

if(REC2_DUMPERS)
    target_compile_definitions(c2_common PRIVATE dumpers)
    add_custom_command(OUTPUT
            "${CMAKE_CURRENT_BINARY_DIR}/dumper.c"
            "${CMAKE_CURRENT_BINARY_DIR}/dumper.h"
        COMMAND Python3::Interpreter "${rec2_SOURCE_DIR}/scripts/gendumper.py"
            brender/br_types.h
            rec2_types.h
            -j "${CMAKE_CURRENT_BINARY_DIR}/dumper.h"
            -o "${CMAKE_CURRENT_BINARY_DIR}/dumper.c"
            -I "${rec2_SOURCE_DIR}/scripts/include-sre"
            -I "${rec2_SOURCE_DIR}/src/hooks"
            -I "${rec2_SOURCE_DIR}/src/brender/include"
            -I "${CMAKE_CURRENT_SOURCE_DIR}/../hooks"
            -I "${CMAKE_CURRENT_SOURCE_DIR}/../include"
            -I "${CMAKE_CURRENT_SOURCE_DIR}/../s3/include"
            -D _SRE_
            --dump-typedefs tTrack_spec tRace_info tBrender_storage tProgram_state v11group v11face tCollision_shape_polyhedron
            --skip-typedefs br_model tBrender_storage v11group v11model tCollision_shape_polyhedron_data
    )
    target_sources(c2_common PRIVATE
        "${CMAKE_CURRENT_BINARY_DIR}/dumper.c"
        "${CMAKE_CURRENT_BINARY_DIR}/dumper.h"
        custom_dumper.c
    )
    target_include_directories(c2_common
        PRIVATE "${CMAKE_CURRENT_BINARY_DIR}"
    )
endif()
