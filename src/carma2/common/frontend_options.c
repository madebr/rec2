#include "frontend_options.h"

#include "frontend.h"
#include "frontend_main.h"
#include "globvars.h"
#include "main.h"
#include "sound.h"

C2_HOOK_VARIABLE_IMPLEMENT_INIT(tFrontend_spec, gFrontend_OPTIONS, 0x00632c60, {
    "Options",
    0,
    18,
    Options_Infunc,
    Options_Outfunc,
    Generic_MenuHandler,
    &C2V(gFrontend_MAIN),
    0,
    0,
    0,
    6,
    0,
    0,
    {
        { 0x14,  Options_AbortRace,  NULL,                       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x15,  Options_AbortGame,  NULL,                       0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x16,  temp,               &C2V(gFrontend_CONTROLS),   0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0xed,  temp,               &C2V(gFrontend_GRAPHICS),   0, 17, 18, 0, 0, 0, 0, 1, 1, },
        { 0x18,  temp,               &C2V(gFrontend_LOAD_GAME),  0, 17, 18, 0, 0, 0, 0, 1, 1, },
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
        { 0x1c,  temp,               &C2V(gFrontend_QUIT),       0, 17, 18, 0, 0, 0, 0, 1, 1, },
    },
});
C2_HOOK_VARIABLE_IMPLEMENT(tFrontend_slider, gOptions_sound_slider, 0x00763820);


void C2_HOOK_FASTCALL Slider_EffectsProc(tFrontend_slider* pSlider) {

    C2V(gProgram_state).effects_volume = (int)(255.f * pSlider->value);
    SetSoundVolumes(0);
    DRS3StartSound(C2V(gEffects_outlet), eSoundId_LeftButton);
}
C2_HOOK_FUNCTION(0x00474660, Slider_EffectsProc)
