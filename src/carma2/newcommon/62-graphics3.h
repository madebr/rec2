/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

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
extern br_matrix23 gMatrix23_0068c880;
extern br_vector3 gOrigin_map;
extern br_vector2 gOrigin_headup_map;
extern int gMap_render_y_i;
extern int gMap_render_x_i;
extern int gMap_render_height_i;
extern int gMap_render_width_i;

// CarArrowColour

// ToggleTargetLock

// CycleTargetLock

extern void C2_HOOK_FASTCALL CycleMapOptions(void);

extern void C2_HOOK_FASTCALL ToggleMapTransparency(void);

extern void C2_HOOK_FASTCALL InitTargetLock(void);

extern void C2_HOOK_FASTCALL SetIntegerMapRenders(void);

// OppositeColour

// DrawArrow

// DrawMapBlip

// DrawSmashBlip

// DrawMapSmallBlip

// CopyMapToScreen

// DrawCarArrow

extern void C2_HOOK_FASTCALL DoSomeThingsToCheckpoints(br_pixelmap* pMap, tU32 pTime, tDoSomethingsToCheckpoints_cbfn* pCallback, int pDraw_prev);

// CalcMapCheckpoint

extern void C2_HOOK_FASTCALL CalcMapCheckpoint2(br_pixelmap* pMap, int pCheckpoint, tU32 pTime, int pTarget);

// DrawCheckpoint

// DrawOffsetCheckpoint

// DoMapOverlays

// StartMap

// FinishMap

extern void C2_HOOK_FASTCALL CheckpointLine(br_pixelmap* pMap, int pCheckpoint, tU32 pTime, int pTarget);

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
