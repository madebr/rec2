#ifndef FRONTEND_CONTROLS_H
#define FRONTEND_CONTROLS_H

#include "c2_hooks.h"

#include "rec2_types.h"

extern tFrontend_spec gFrontend_CONTROLS;
extern tFrontend_slider gControls_slider_1;
extern tFrontend_slider gControls_slider_2;
extern tFrontend_slider gControls_slider_3;
extern int gFrontend_controls_indices[15];
extern int gINT_00688748;


void C2_HOOK_FASTCALL DisplayJoystickSettings(tFrontend_spec *pFrontend);

void C2_HOOK_FASTCALL Slider_XProc(tFrontend_slider* pSlider);

void C2_HOOK_FASTCALL Slider_YProc(tFrontend_slider* pSlider);

void C2_HOOK_FASTCALL Slider_ForceProc(tFrontend_slider* pSlider);

int C2_HOOK_FASTCALL Controls_Infunc(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL Controls_Outfunc(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL Controls_JoystickDpadToggle(tFrontend_spec* pFrontend);

void C2_HOOK_FASTCALL SaveAllJoystickData(void);

int C2_HOOK_FASTCALL Controls_SwitchKeymapSet(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL Controls_SlotActivated(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL Controls_KeyUp(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL Controls_KeyDown(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL Controls_Ok(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL Controls_JoystickToggle(tFrontend_spec* pFrontend);

#endif /* FRONTEND_CONTROLS_H */