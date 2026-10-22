#ifndef GUARD_62_GRAPHICS3_H
#define GUARD_62_GRAPHICS3_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern int gMini_map_visible;
extern int gCount_smashable_race_targets;
extern tSmashable_race_target gSmashable_race_targets[300];
extern tCar_icon gCar_icons[128];
extern br_actor* gCar_icons_model_actor;
extern int gTarget_lock_enabled;

// CarArrowColour

// ToggleTargetLock

// CycleTargetLock

// CycleMapOptions

// ToggleMapTransparency

extern void C2_HOOK_FASTCALL InitTargetLock(void);

// SetIntegerMapRenders

// OppositeColour

// DrawArrow

// DrawMapBlip

// DrawSmashBlip

// DrawMapSmallBlip

// CopyMapToScreen

// DrawCarArrow

extern void C2_HOOK_FASTCALL DoSomeThingsToCheckpoints(br_pixelmap* pMap, tU32 pTime, tDoSomethingsToCheckpoints_cbfn* pCallback, int pDraw_prev);

// CalcMapCheckpoint

// CalcMapCheckpoint2

// DrawCheckpoint

// DrawOffsetCheckpoint

// DoMapOverlays

// StartMap

// FinishMap

// CheckpointLine

// DoDirectionFinderStuff

// MungeMoodMessage

// DoOpponentStatusHeadup

// AllocateOppoBarActor

// DoNearestOpponentRelativeHeightMarkerStuff

// MapOverlay

// InitNearestCar

// MapStuffBeforeRender

// MapStuffAfterRender

// static void C2_HOOK_FASTCALL ReadMoodMessages(void);

extern void C2_HOOK_FASTCALL InitMap(void);

extern void C2_HOOK_FASTCALL ConvertCarIcons(br_pixelmap* pMap);

// static void C2_HOOK_FASTCALL MapChanged(void);

// CheckMapMoveKeys

extern void C2_HOOK_FASTCALL InitSmashTargets(void);

extern void C2_HOOK_FASTCALL AddSmashTarget(br_model* pModel, br_actor* pActor, int pUnknown);

extern int NearestPowerOfTwo(int pValue) ;

#endif // GUARD_62_GRAPHICS3_H
