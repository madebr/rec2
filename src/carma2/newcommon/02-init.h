#ifndef GUARD_02_INIT_H
#define GUARD_02_INIT_H

#include "c2_hooks.h"

#include "brender/brender.h"

extern int gBr_initialized;

extern br_material* gDefault_track_material;

// Modify2DCopyPixelmaps

// SetupFFBValues

extern void C2_HOOK_FASTCALL AllocateActors(void);

extern void C2_HOOK_FASTCALL AllocateCamera(void);

// ReinitialiseForwardCamera

// AllocateRearviewPixelmap

// ReinitialiseRearviewCamera

// ReinitialiseRenderStuff

extern void C2_HOOK_FASTCALL InstallFindFailedHooks(void);

extern void C2_HOOK_FASTCALL AllocateStandardLamp(void);

// extern void C2_HOOK_FASTCALL InitializeBRenderEnvironment(void);

// InitBRFonts

extern void C2_HOOK_FASTCALL Init2DStuffForPolyFonts(void);

extern void C2_HOOK_FASTCALL InitLineStuff(void);

extern void C2_HOOK_FASTCALL Init2DStuff(void);

extern void C2_HOOK_FASTCALL CheckTimedDemo(void);

// extern void C2_HOOK_FASTCALL InitialiseApplication(int pArgc, const char **pArgv);

extern void C2_HOOK_FASTCALL DoDevelopmentThings(void);

extern void C2_HOOK_FASTCALL InitialiseDeathRace(int pArgc, const char** pArgv);


extern void C2_HOOK_FASTCALL InitGameAccordingToSkillLevel(void);

extern void C2_HOOK_FASTCALL InitGame(int pStart_race);

// LoadInTrack

// InitRace

// GetScreenSize

// SetScreenSize

#endif // GUARD_02_INIT_H
