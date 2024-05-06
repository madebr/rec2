#ifndef REC2_INIT_H
#define REC2_INIT_H

#include "brender/brender.h"

#include "c2_hooks.h"

C2_HOOK_VARIABLE_DECLARE(br_material*, gDefault_track_material);

C2_HOOK_VARIABLE_DECLARE(int, gRender_indent);
C2_HOOK_VARIABLE_DECLARE(int, gBr_initialized);
C2_HOOK_VARIABLE_DECLARE(int, gBrZb_initialized);
C2_HOOK_VARIABLE_DECLARE(undefined4, gDAT_0074c6d0);
C2_HOOK_VARIABLE_DECLARE_ARRAY(int, gInitial_powerup_slots, 3);

void C2_HOOK_FASTCALL InitialiseApplication(int pArgc, const char** pArgv);

int C2_HOOK_FASTCALL GetScreenSize(void);

void C2_HOOK_FASTCALL SetScreenSize(int pNew_size);

void C2_HOOK_FASTCALL InitializeBRenderEnvironment(void);

void C2_HOOK_FASTCALL AllocateSelf(void);

void C2_HOOK_FASTCALL AllocateCamera(void);

void C2_HOOK_FASTCALL Init2DStuff(void);

void C2_HOOK_FASTCALL InitLineStuff(void);

void C2_HOOK_FASTCALL InitSmokeStuff(void);

void C2_HOOK_FASTCALL AllocateStandardLamp(void);

void C2_HOOK_FASTCALL InitGame(int pStart_race);

int C2_HOOK_FASTCALL Fix2DTextureWidth(int pWidth);

int C2_HOOK_FASTCALL Fix2DTextureHeight(int pHeight);

void C2_HOOK_FASTCALL DisposeGameIfNecessary(void);

void C2_HOOK_FASTCALL ReinitialiseRenderStuff(void);

void C2_HOOK_FASTCALL ReinitialiseForwardCamera(void);

void C2_HOOK_FASTCALL AllocateRearviewPixelmap(void);

void C2_HOOK_FASTCALL ReinitialiseRearviewCamera(void);

void C2_HOOK_FASTCALL InitRace(void);

void C2_HOOK_FASTCALL DisposeRace(void);

void C2_HOOK_FASTCALL LoadInTrack(void);

void C2_HOOK_FASTCALL DisposeTrack(void);

#endif //REC2_INIT_H
