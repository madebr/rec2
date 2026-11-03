/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_22_REPLAY_H
#define GUARD_22_REPLAY_H

#include "c2_hooks.h"
#include "rec2_types.h"
#include "23-piping.h"

extern int gPaused;

extern const int gPow2_array[32];

extern tU8 gCamera_type_allowed_replay[9];
extern tU8 gCamera_type_allowed_gameplay[9];

extern int gAR_camera_type;

extern int gCar_car_collisions;

extern const float gCar_simplification_factor[2][5];
extern tPolyhedron_edge_indexes gDefault_box_collision_wireframe_edges[12];

extern const char* gReplay_pixie_names[11];
extern br_pixelmap* gReplay_pixelmaps[11];

extern void C2_HOOK_FASTCALL ActualActionReplayHeadups(int pSpecial_zappy_bastard);

extern void C2_HOOK_FASTCALL DoActionReplayPostSwap(void);

extern void C2_HOOK_FASTCALL DoZappyActionReplayHeadups(int pFrame_number);

extern void C2_HOOK_FASTCALL DoActionReplayHeadups(void);

extern void C2_HOOK_FASTCALL PreProcess(int pFrame_period);

extern void C2_HOOK_FASTCALL PostProcess(int pFrame_period);

// DoActionReplay

// MoveToEndOfReplay

// MoveToStartOfReplay

// PollActionReplayControls

extern void C2_HOOK_FASTCALL CheckReplayTurnOn(void);

extern void C2_HOOK_FASTCALL InitializeActionReplayPix(void);

// FreeActionReplayPix

extern void C2_HOOK_FASTCALL SynchronizeActionReplay(void);

extern void C2_HOOK_FASTCALL TurnOnActionReplay(void);

extern void C2_HOOK_FASTCALL TurnOffActionReplay(void);

extern void C2_HOOK_FASTCALL AfterActionReplay(void);

extern void C2_HOOK_FASTCALL ToggleReplay(int* pArg1, int* pArg2);

// DisplayMovieStatusPanel

extern void C2_HOOK_FASTCALL ApplyTime(tU32 pTime);

extern void C2_HOOK_FASTCALL ARSetPause(int pPause);

extern tU32 C2_HOOK_FASTCALL ARGetStartTime(void);

extern void C2_HOOK_FASTCALL FUN_0040b500(tPed_character_instance* pCharacter, int pBone_index, int pArg3, int pArg4);

extern br_uint_8 C2_HOOK_CDECL ActorToActorMatrix34Local(br_matrix34* m, br_actor* a, br_actor* b);

extern void C2_HOOK_CDECL RelinkActorKeepWorldTransform(br_actor* pParent, br_actor* pActor);

extern void C2_HOOK_FASTCALL WorkerCrush(int field_0x4, void* pField_0x8, void* pField_0xe);

extern void C2_HOOK_FASTCALL WorkerCrushUndo(int field_0x4, void* pField_0x8, void* pField_0xe);

extern void C2_HOOK_FASTCALL WorkerFlap(int field_0x4, tU16 field_0x8);

extern void C2_HOOK_FASTCALL WorkerSplitWeldUndo(void* roadMachine);

extern void C2_HOOK_FASTCALL WorkerPHILObject(int field_0x0, br_matrix34* pField_0x4);

extern void C2_HOOK_FASTCALL WorkerSmudge(tPipe_chunk** pChunk, int pScale);

extern void C2_HOOK_FASTCALL WorkerPedMove(tPedestrian* pPed, int field_0x4, int field_0xc, int field_0x8_1, int field_0x14, int zero, br_vector3* pField_0x24, int field_0x34, br_vector3* pField_0x38);

extern void C2_HOOK_FASTCALL WorkerPedMoveUndo(tPedestrian* pPed, int field_0x6, int field_0x10, int zero, int zero2, int field_0xa, br_vector3* pField_0x18, int field_0x30, br_vector3* pField_0x38);

extern void C2_HOOK_FASTCALL WorkerPedFPChange(int field_0x0, tPipe_stack_word field_0x4);

extern void C2_HOOK_FASTCALL WorkerPedPhysics(int field_0x0, br_matrix34* pMatrix);

extern void C2_HOOK_FASTCALL WorkerPedModelChangeUndo(int field_0x0, int field_0x4, int field_0xc, int field_0xa);

extern void C2_HOOK_FASTCALL WorkerPedModelChange(int field_0x0, int field_0x4, int field_0x6, int field_0x8);

extern void C2_HOOK_FASTCALL WorkerPedDismember(int field_0x0, int field_0x4, int field_0x7);

extern void C2_HOOK_FASTCALL WorkerProxRay(int field_0x0, tU16 field_0x6, tU16 field_0x4, int field_0x8);

extern void C2_HOOK_FASTCALL WorkerRepulseRay(int field_0x0, int field_0x4, int field_0x8, int field_0xc);

extern void C2_HOOK_FASTCALL WorkerSpecial(void);

extern void C2_HOOK_FASTCALL DrawStatusMessage(int pReplay_only);

extern void C2_HOOK_FASTCALL WorkerUndoSmashRemoveFaces(int field_0x0, int field_0x8, void* pField_0xc, int field_0x4);

extern void C2_HOOK_FASTCALL WorkerSpark(int field_0x0, void* pField_0x4, void* pField_0x10);

extern void C2_HOOK_FASTCALL WorkerShrapnel(int field_0x0, void* pField_0x4, tU16 field_0x10, int field_0x14);

extern void C2_HOOK_FASTCALL WorkerShrapnelShower(tU32 field_0x4, tU32 field_0x84, tU32 field_0x44, br_vector3* field_0x8, br_vector3* field_0x20, br_vector3* field_0x2c, tU32 field_0x50, tU32 field_0x48, tU32 field_0x0, br_vector3* field_0x54, br_vector3* field_0x88, br_vector3* field_0x14, tU32 field_0xa0, br_vector3* field_0x38, tU32 field_0);

extern void C2_HOOK_FASTCALL WorkerShrapnelShowerUndo(tU32 chunk_0x48, tU32 prev_0x4, tU32 prev_0x84, tU32 prev_0x44, br_vector3* prev_0x8, br_vector3* prev_0x20, br_vector3* prev_0x2c, tU32 prev_0x50, tU32 prev_0x48, tU32 prev_0x0, br_vector3* prev_0x54, br_vector3* prev_0x88, br_vector3* prev_0x14, tU32 prev_0xa0, br_vector3* prev_0x38);

extern void C2_HOOK_FASTCALL WorkerExtendedSplash(int field_0x0, int field_0x4, int field_0x8, br_vector3* pField_0xc, br_vector3* pField_0x18);

#endif // GUARD_22_REPLAY_H
