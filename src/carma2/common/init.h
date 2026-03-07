#ifndef REC2_INIT_H
#define REC2_INIT_H

#include "brender/brender.h"

#include "c2_hooks.h"

extern br_material* gDefault_track_material;

extern int gRender_indent;
extern int gBr_initialized;
extern int gBrZb_initialized;
extern undefined4 gDAT_0074c6d0;
extern int gInitial_powerup_slots[3];

void C2_HOOK_FASTCALL InitialiseApplication(int pArgc, const char **pArgv);

void C2_HOOK_FASTCALL DoDevelopmentThings(void);

void C2_HOOK_FASTCALL InitLightingEffects(void);

void C2_HOOK_FASTCALL KillLightingEffects(void);

void C2_HOOK_FASTCALL InitialiseDeathRace(int pArgc, const char** pArgv);

int C2_HOOK_FASTCALL GetScreenSize(void);

void C2_HOOK_FASTCALL SetScreenSize(int pNew_size);

void C2_HOOK_FASTCALL InitializeBRenderEnvironment(void);

void C2_HOOK_FASTCALL AllocateActors(void);

void C2_HOOK_FASTCALL AllocateCamera(void);

void C2_HOOK_FASTCALL Init2DStuff(void);

void C2_HOOK_FASTCALL InitLineStuff(void);

void C2_HOOK_FASTCALL InitSmokeStuff(void);

void C2_HOOK_FASTCALL InstallFindFailedHooks(void);

void C2_HOOK_FASTCALL AllocateStandardLamp(void);

void C2_HOOK_FASTCALL InitGameAccordingToSkillLevel(void);

void C2_HOOK_FASTCALL InitGame(int pStart_race);

int C2_HOOK_FASTCALL HighResHeadupWidth(int pWidth);

int C2_HOOK_FASTCALL HighResHeadupHeight(int pHeight);

void C2_HOOK_FASTCALL DisposeGameIfNecessary(void);

void C2_HOOK_FASTCALL ReinitialiseRenderStuff(void);

void C2_HOOK_FASTCALL ReinitialiseForwardCamera(void);

void C2_HOOK_FASTCALL AllocateRearviewPixelmap(void);

void C2_HOOK_FASTCALL ReinitialiseRearviewCamera(void);

void C2_HOOK_FASTCALL Modify2DCopyPixelmaps(void);

void C2_HOOK_FASTCALL InitRace(void);

void C2_HOOK_FASTCALL DisposeRace(void);

void C2_HOOK_FASTCALL LoadInTrack(void);

void C2_HOOK_FASTCALL DisposeTrack(void);

#endif //REC2_INIT_H