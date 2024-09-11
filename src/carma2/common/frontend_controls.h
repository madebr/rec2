#ifndef FRONTEND_CONTROLS_H
#define FRONTEND_CONTROLS_H

#include "c2_hooks.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE(tFrontend_spec, gFrontend_CONTROLS);
C2_HOOK_VARIABLE_DECLARE(tFrontend_slider, gControls_slider_1);
C2_HOOK_VARIABLE_DECLARE(tFrontend_slider, gControls_slider_2);
C2_HOOK_VARIABLE_DECLARE(tFrontend_slider, gControls_slider_3);
C2_HOOK_VARIABLE_DECLARE_ARRAY(int, gFrontend_controls_indices, 15);
C2_HOOK_VARIABLE_DECLARE(int, gINT_00688748);


void C2_HOOK_FASTCALL DisplayJoystickSettings(tFrontend_spec *pFrontend);

void C2_HOOK_FASTCALL Slider_XProc(tFrontend_slider* pSlider);

void C2_HOOK_FASTCALL Slider_YProc(tFrontend_slider* pSlider);

void C2_HOOK_FASTCALL Slider_ForceProc(tFrontend_slider* pSlider);

int C2_HOOK_FASTCALL Controls_Infunc(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL Controls_Outfunc(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL Controls_JoystickDpadToggle(tFrontend_spec* pFrontend);

void C2_HOOK_FASTCALL SaveAllJoystickData(void);

int C2_HOOK_FASTCALL Controls_SwitchKeyMapSet(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL Controls_SlotActivated(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL Controls_KeyUp(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL Controls_KeyDown(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL Controls_Ok(tFrontend_spec* pFrontend);

#endif /* FRONTEND_CONTROLS_H */
