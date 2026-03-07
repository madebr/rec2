#include "frontend_options.h"

#include "controls.h"
#include "frontend.h"
#include "frontend_controls.h"
#include "frontend_graphics.h"
#include "frontend_loadgame.h"
#include "frontend_main.h"
#include "frontend_quit.h"
#include "globvars.h"
#include "globvrpb.h"
#include "init.h"
#include "loading.h"
#include "loadsave.h"
#include "main.h"
#include "network.h"
#include "sound.h"

// GLOBAL: CARMA2_HW 0x00632c60
tFrontend_spec gFrontend_OPTIONS = {
    "Options",
    0,
    18,
    Options_Infunc,
    Options_Outfunc,
    Generic_MenuHandler,
    &gFrontend_MAIN,
    0,
    0,
    0,
    6,
    0,
    0,
    {
        { 0x14,  Options_AbortRace,  NULL,                       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x15,  Options_AbortGame,  NULL,                       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x16,  temp,               &gFrontend_CONTROLS,   0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xed,  temp,               &gFrontend_GRAPHICS,   0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x18,  temp,               &gFrontend_LOAD_GAME,  0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x19,  temp,               NULL,                       0, 0,  0,  0, 0, 0, 0, 0, 1, },
        { 0x402, temp,               NULL,                       0, 0,  0,  0, 0, 0, 0, 0, 1, },
        { 0x402, temp,               NULL,                       0, 0,  0,  0, 0, 0, 0, 0, 1, },
        { 0x402, temp,               NULL,                       0, 0,  0,  0, 0, 0, 0, 0, 1, },
        { 0x1a,  temp,               NULL,                       0, 0,  0,  0, 0, 0, 0, 0, 1, },
        { 0x402, temp,               NULL,                       0, 0,  0,  0, 0, 0, 0, 1, 1, 0, 1, 1, },
        { 0x402, temp,               NULL,                       0, 0,  0,  0, 0, 0, 0, 1, 1, 1, 0, 0, },
        { 0x402, temp,               NULL,                       0, 0,  0,  0, 0, 0, 0, 1, 1, 1, 0, 0, },
        { 0x1b,  temp,               NULL,                       0, 0,  0,  0, 0, 0, 0, 1, 1, },
        { 0x38,  Options_MusicOn,    NULL,                       0, 0,  0,  0, 0, 0, 0, 1, 1, },
        { 0x39,  Options_MusicOff,   NULL,                       0, 0,  0,  0, 0, 0, 0, 1, 1, },
        { 0x7,   Options_Ok,         (void*)1,                   0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x1c,  temp,               &gFrontend_QUIT,       0, 17, 18, 0, 0, 0, 0, 1, 1, },
    },
};

// GLOBAL: CARMA2_HW 0x00763820
tFrontend_slider gOptions_sound_slider;


// FUNCTION: CARMA2_HW 0x00474660
void C2_HOOK_FASTCALL Slider_EffectsProc(tFrontend_slider* pSlider) {

    gProgram_state.effects_volume = (int)(255.f * pSlider->value);
    SetSoundVolumes(0);
    DRS3StartSound(gEffects_outlet, eSoundId_LeftButton);
}

void C2_HOOK_FASTCALL DisplayVolumeSettings(tFrontend_spec* pFrontend) {

    gOptions_sound_slider.value = gProgram_state.effects_volume / 255.f;
    if (gProgram_state.music_volume < 128) {
        SelectThisItemIn(pFrontend, 1, 15);
    } else {
        SelectThisItemIn(pFrontend, 1, 14);
    }
}

// FUNCTION: CARMA2_HW 0x00474530
int C2_HOOK_FASTCALL Options_Infunc(tFrontend_spec* pFrontend) {

    Generic_Infunc(pFrontend);
    gOptions_sound_slider.flags = 0;
    gOptions_sound_slider.itemid_left_reference = 10;
    gOptions_sound_slider.itemid_start = 6;
    gOptions_sound_slider.width = 236;
    gOptions_sound_slider.callback = Slider_EffectsProc;
    gOptions_sound_slider.field_0x14 = 0.f;
    gOptions_sound_slider.field_0x18 = 1.f;
    gOptions_sound_slider.next = NULL;
    gCurrent_frontend_scrollbars = &gOptions_sound_slider;
    DisplayVolumeSettings(pFrontend);
    if (gProgram_state.racing) {
        pFrontend->items[0].enabled = kFrontendItemEnabled_enabled;
        pFrontend->items[1].enabled = kFrontendItemEnabled_enabled;
    } else {
        pFrontend->items[0].enabled = kFrontendItemEnabled_disabled;
        if (gProgram_state.field_0x2c) {
            pFrontend->items[1].enabled = kFrontendItemEnabled_enabled;
        } else {
            pFrontend->items[1].enabled = kFrontendItemEnabled_disabled;
        }
    }
    if (gNet_mode == eNet_mode_none) {
        pFrontend->items[4].enabled = kFrontendItemEnabled_enabled;
    } else {
        pFrontend->items[4].enabled = kFrontendItemEnabled_disabled;
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x00474690
int C2_HOOK_FASTCALL Options_Outfunc(tFrontend_spec* pFrontend) {

    Generic_Outfunc(pFrontend);
    SaveOptions();
    return 1;
}

// FUNCTION: CARMA2_HW 0x00474730
int C2_HOOK_FASTCALL Options_AbortRace(tFrontend_spec* pFrontend) {
    if (gProgram_state.racing) {
        gAbandon_game = 1;
        if (gNet_mode != eNet_mode_host) {
            gProgram_state.prog_status = eProg_idling;
        }
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x00474760
int C2_HOOK_FASTCALL Options_AbortGame(tFrontend_spec* pFrontend) {

    if (gProgram_state.racing || gNet_mode != eNet_mode_none) {
        if (gProgram_state.racing) {
            gAbandon_game = 1;
        }
        gProgram_state.prog_status = eProg_idling;
        gNo_current_game = 1;
        return 1;
    } else {
        gValid_stashed_save_game = gProgram_state.racing;
        ShutdownNetIfRequired();
        LoadRaces(gRace_list, &gNumber_of_races, -1);
        InitGame(gDev_initial_race);
        gFrontend_next_menu = kFrontend_menu_main;
        return 3;
    }
}

// FUNCTION: CARMA2_HW 0x004746a0
int C2_HOOK_FASTCALL Options_MusicOn(tFrontend_spec* pFrontend) {

    gProgram_state.music_volume = 255;
    DisplayVolumeSettings(pFrontend);
    SetSoundVolumes(1);
    return 0;
}

// FUNCTION: CARMA2_HW 0x004746e0
int C2_HOOK_FASTCALL Options_MusicOff(tFrontend_spec* pFrontend) {

    gProgram_state.music_volume = 0;
    DisplayVolumeSettings(pFrontend);
    SetSoundVolumes(1);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00474720
int C2_HOOK_FASTCALL Options_Ok(tFrontend_spec* pFrontend) {

    return 0;
}
