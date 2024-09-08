#ifndef FRONTEND_OPTIONS_H
#define FRONTEND_OPTIONS_H

#include "c2_hooks.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE(tFrontend_spec, gFrontend_OPTIONS);
C2_HOOK_VARIABLE_DECLARE(tFrontend_slider, gOptions_sound_slider);


void C2_HOOK_FASTCALL Slider_EffectsProc(tFrontend_slider* pSlider);

#endif /* FRONTEND_OPTIONS_H */
