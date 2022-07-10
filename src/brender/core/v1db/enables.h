#ifndef REC2_ENABLES_H
#define REC2_ENABLES_H

#include "c2_hooks.h"

#include "brender/br_types.h"

void C2_HOOK_STDCALL actorEnable(br_v1db_enable* e, br_actor* a);

void C2_HOOK_STDCALL actorDisable(br_v1db_enable* e, br_actor* a);

void C2_HOOK_CDECL BrLightEnable(br_actor* l);

void C2_HOOK_CDECL BrLightDisable(br_actor* l);

void C2_HOOK_CDECL BrClipPlaneEnable(br_actor* c);

void C2_HOOK_CDECL BrClipPlaneDisable(br_actor* c);

void C2_HOOK_CDECL BrHorizonPlaneEnable(br_actor* h);

void C2_HOOK_CDECL BrHorizonPlaneDisable(br_actor* h);

br_actor* C2_HOOK_CDECL BrEnvironmentSet(br_actor* a);

br_boolean C2_HOOK_STDCALL setupView(br_matrix34* view_to_this, br_matrix34* this_to_view, br_matrix34* world_to_view, br_int_32 w2vt, br_actor* world, br_actor* a);

void C2_HOOK_STDCALL BrSetupLights(br_actor* world, br_matrix34* world_to_view, br_int_32 w2vt);

void C2_HOOK_STDCALL BrSetupClipPlanes(br_actor* world, br_matrix34* world_to_view, br_int_32 w2vt, br_matrix4* view_to_screen);

void C2_HOOK_STDCALL BrSetupEnvironment(br_actor* world, br_matrix34* world_to_view, br_int_32 w2vt);

void C2_HOOK_STDCALL BrSetupHorizons(br_actor* world, br_matrix34* world_to_view, br_int_32 w2vt);

void C2_HOOK_STDCALL BrActorEnableCheck(br_actor* a);

#endif // REC2_ENABLES_H
