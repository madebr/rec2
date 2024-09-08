#include "frontend_quit.h"

#include "frontend.h"
#include "frontend_main.h"
#include "globvars.h"
#include "globvrpb.h"
#include "loading.h"
#include "main.h"
#include "sound.h"

C2_HOOK_VARIABLE_IMPLEMENT_INIT(tFrontend_spec, gFrontend_OPTIONS, 0x00632c60, {
    FIXME
});
C2_HOOK_VARIABLE_IMPLEMENT(tFrontend_slider, gOptions_sound_slider, 0x00763820);


void C2_HOOK_FASTCALL Slider_EffectsProc(tFrontend_slider* pSlider) {

    C2V(gProgram_state).effects_volume = (int)(255.f * pSlider->value);
    SetSoundVolumes(0);
    DRS3StartSound(C2V(gEffects_outlet), eSoundId_LeftButton);
}
C2_HOOK_FUNCTION(0x00474660, Slider_EffectsProc)

void C2_HOOK_FASTCALL DisplayVolumeSettings(tFrontend_spec* pFrontend) {

    C2V(gOptions_sound_slider).value = C2V(gProgram_state).effects_volume / 255.f;
    if (C2V(gProgram_state).music_volume < 128) {
        SelectThisItemIn(pFrontend, 1, 15);
    } else {
        SelectThisItemIn(pFrontend, 1, 14);
    }
}

int C2_HOOK_FASTCALL Options_Infunc(tFrontend_spec* pFrontend) {

    Generic_Infunc(pFrontend);
    C2V(gOptions_sound_slider).flags = 0;
    C2V(gOptions_sound_slider).itemid_left_reference = 10;
    C2V(gOptions_sound_slider).itemid_start = 6;
    C2V(gOptions_sound_slider).width = 236;
    C2V(gOptions_sound_slider).callback = Slider_EffectsProc;
    C2V(gOptions_sound_slider).field_0x14 = 0.f;
    C2V(gOptions_sound_slider).field_0x18 = 1.f;
    C2V(gOptions_sound_slider).next = NULL;
    C2V(gCurrent_frontend_scrollbars) = &C2V(gOptions_sound_slider);
    DisplayVolumeSettings(pFrontend);
    if (C2V(gProgram_state).racing) {
        pFrontend->items[0].enabled = kFrontendItemEnabled_enabled;
        pFrontend->items[1].enabled = kFrontendItemEnabled_enabled;
    } else {
        pFrontend->items[0].enabled = kFrontendItemEnabled_disabled;
        if (C2V(gProgram_state).field_0x2c) {
            pFrontend->items[1].enabled = kFrontendItemEnabled_enabled;
        } else {
            pFrontend->items[1].enabled = kFrontendItemEnabled_disabled;
        }
    }
    if (C2V(gNet_mode) == eNet_mode_none) {
        pFrontend->items[4].enabled = kFrontendItemEnabled_enabled;
    } else {
        pFrontend->items[4].enabled = kFrontendItemEnabled_disabled;
    }
    return 1;
}
C2_HOOK_FUNCTION(0x00474530, Options_Infunc)

int C2_HOOK_FASTCALL Options_Outfunc(tFrontend_spec* pFrontend) {

    Generic_Outfunc(pFrontend);
    SaveOptions();
    return 1;
}
C2_HOOK_FUNCTION(0x00474690, Options_Outfunc)
