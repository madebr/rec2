#ifndef GUARD_62_GRAPHICS3_H
#define GUARD_62_GRAPHICS3_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern int gMini_map_visible;
extern tCar_icon gCar_icons[128];
extern br_actor* gCar_icons_model_actor;
extern int gCount_smashable_race_targets;

// CarArrowColour

// ToggleTargetLock

// CycleTargetLock

// CycleMapOptions

// ToggleMapTransparency

// InitTargetLock

// SetIntegerMapRenders

// OppositeColour

// DrawArrow

// DrawMapBlip

// DrawSmashBlip

// DrawMapSmallBlip

// CopyMapToScreen

// DrawCarArrow

// DoSomeThingsToCheckpoints

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

// AddSmashTarget

extern int NearestPowerOfTwo(int pValue) ;

#endif // GUARD_62_GRAPHICS3_H
