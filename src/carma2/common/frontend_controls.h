#ifndef FRONTEND_CONTROLS_H
#define FRONTEND_CONTROLS_H

#include "c2_hooks.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE(tFrontend_spec, gFrontend_CONTROLS);
C2_HOOK_VARIABLE_DECLARE(tFrontend_slider, gControls_slider_1);
C2_HOOK_VARIABLE_DECLARE(tFrontend_slider, gControls_slider_2);
C2_HOOK_VARIABLE_DECLARE(tFrontend_slider, gControls_slider_3);
C2_HOOK_VARIABLE_DECLARE_ARRAY(int, gFrontend_controls_indices, 15);


void C2_HOOK_FASTCALL DisplayJoystickSettings(tFrontend_spec *pFrontend);

void C2_HOOK_FASTCALL Slider_XProc(tFrontend_slider* pSlider);

void C2_HOOK_FASTCALL Slider_YProc(tFrontend_slider* pSlider);

#endif /* FRONTEND_CONTROLS_H */
