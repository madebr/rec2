#include "frontend_quit.h"

#include "frontend.h"
#include "frontend_main.h"
#include "globvars.h"
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
