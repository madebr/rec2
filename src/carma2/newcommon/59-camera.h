#ifndef GUARD_59_CAMERA_H
#define GUARD_59_CAMERA_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern int gCamera_frozen;
extern br_actor* gPed_actor;

extern float gCamera_cockpit_hither;
extern tActionReplayCameraMode gAction_replay_camera_mode;

// DoCameraControls

// FrozenCamera

// CheckCameraHither

// GetRaceLeader

// AmIGettingBoredWatchingCameraSpin

// ViewNetPlayer

// ViewOpponent

// ReallyViewOpponent

// CheckDisablePlingMaterials

// SetPanningFieldOfView

// IsCameraTypeAllowed

// MungeCarMaterials

// SetCameraType

// ChangeCameraType

// ToggleCockpit

// PositionCarMountedCamera

// PositionExternalCamera

extern void C2_HOOK_FASTCALL CameraBugFix(tCar_spec* pCar, tU32 pTime);

// PossibleRemoveNonCarFromWorld

// PutNonCarBackInWorld

extern int C2_HOOK_FASTCALL IncidentCam(tCar_spec* pCar, tU32 pTime);

// MoveCamToIncident

// PanningExternalCamera

// CheckForWall

// SetUpPanningCamera

// SaveCameraPosition

// RestoreCameraPosition

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

// InitialiseExternalCamera

extern void C2_HOOK_FASTCALL FreezeCamera(void);

extern void C2_HOOK_FASTCALL GetAverageGridPosition(tRace_info* pThe_race);

// FindACar

// FindAPed

// MouseClickInActionReplay

#endif // GUARD_59_CAMERA_H
