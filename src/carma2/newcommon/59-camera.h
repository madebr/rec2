/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_59_CAMERA_H
#define GUARD_59_CAMERA_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern int gNet_player_to_view_index;
extern int gOpponent_viewing_mode;
extern int gAction_replay_manual_camera_target_type;
extern int gCamera_frozen;
extern tSave_camera gSave_camera[2];
extern br_actor* gPed_actor;

extern float gCamera_cockpit_hither;
extern tActionReplayCameraMode gAction_replay_camera_mode;

// DoCameraControls

// FrozenCamera

// CheckCameraHither

extern tCar_spec* C2_HOOK_FASTCALL GetRaceLeader(void);

// AmIGettingBoredWatchingCameraSpin

extern void C2_HOOK_FASTCALL ViewNetPlayer(void);

extern void C2_HOOK_FASTCALL ViewOpponent(void);

extern void C2_HOOK_FASTCALL ReallyViewOpponent(void);

// CheckDisablePlingMaterials

// SetPanningFieldOfView

// IsCameraTypeAllowed

extern void C2_HOOK_FASTCALL MungeCarMaterials(tCar_spec* pCar, int pInternal_cam);

extern void C2_HOOK_FASTCALL SetCameraType(tActionReplayCameraMode pCamPos);

// ChangeCameraType

extern void C2_HOOK_FASTCALL ToggleCockpit(void);

// PositionCarMountedCamera

extern void C2_HOOK_FASTCALL PositionExternalCamera(tCar_spec* pCar_spec, tU32 pTime);

extern void C2_HOOK_FASTCALL CameraBugFix(tCar_spec* pCar, tU32 pTime);

// PossibleRemoveNonCarFromWorld

// PutNonCarBackInWorld

extern int C2_HOOK_FASTCALL IncidentCam(tCar_spec* pCar, tU32 pTime);

// MoveCamToIncident

// PanningExternalCamera

// CheckForWall

// SetUpPanningCamera

extern void C2_HOOK_FASTCALL SaveCameraPosition(int pIndex);

extern void C2_HOOK_FASTCALL RestoreCameraPosition(int pIndex);

// DrVector3RotateY

// GeneralisedPositionExternalCamera

// NormalPositionExternalCamera

// PositionPedCam

// PositionDroneCam

// MoveWithWheels

// SwingCamera

// PointCameraAtCar

// PointCamera

// CollideCamera2

// CollideCameraWithOtherCars

extern void C2_HOOK_FASTCALL InitialiseExternalCamera(void);

extern void C2_HOOK_FASTCALL FreezeCamera(void);

extern void C2_HOOK_FASTCALL GetAverageGridPosition(tRace_info* pThe_race);

// FindACar

// FindAPed

// MouseClickInActionReplay

#endif // GUARD_59_CAMERA_H
