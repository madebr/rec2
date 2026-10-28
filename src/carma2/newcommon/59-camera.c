/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "59-camera.h"

#include "10-loading2.h"
#include "21-mainloop.h"
#include "22-replay.h"
#include "37-brucetrk.h"
#include "57-drone.h"
#include "globvars.h"
#include "globvrkm.h"
#include "globvrpb.h"
#include "rec2_macros.h"

// GLOBAL: CARMA2_HW 0x00679278
// GLOBAL: CARMA2_SW 0x006739e8
int gAction_replay_manual_camera_target_type;

// GLOBAL: CARMA2_HW 0x006792b4
// GLOBAL: CARMA2_SW 0x00673a24
br_actor* gPed_actor;

// GLOBAL: CARMA2_HW 0x006792b8
int gCamera_frozen;

// GLOBAL: CARMA2_HW 0x006792c8
// GLOBAL: CARMA2_SW 0x00673a38
tSave_camera gSave_camera[2];

// GLOBAL: CARMA2_HW 0x007634ac
// GLOBAL: CARMA2_SW 0x00759a0c
float gCamera_cockpit_hither;

// GLOBAL: CARMA2_HW 0x0079efa8
// GLOBAL: CARMA2_SW 0x00795548
tActionReplayCameraMode gAction_replay_camera_mode;

// DoCameraControls

// FrozenCamera

// CheckCameraHither

// GetRaceLeader

// AmIGettingBoredWatchingCameraSpin

// ViewNetPlayer

// ViewOpponent

// FUNCTION: CARMA2_HW 0x0040e680
// FUNCTION: CARMA2_SW 0x0040e660
void C2_HOOK_FASTCALL ReallyViewOpponent(void) {
    int i;

    if (gNet_mode == eNet_mode_client) {
        for (i = 0; i < gNumber_of_net_players; i++) {
            if (gNet_players[i].ID == gCar_to_view_id) {
                gCar_to_view_id = 0;
                gCamera_yaw = 0;
                gCar_to_view = gNet_players[i].car;
                InitialiseExternalCamera();
                PositionExternalCamera(gCar_to_view, 200);
                return;
            }
        }
    }
}

// CheckDisablePlingMaterials

// SetPanningFieldOfView

// IsCameraTypeAllowed

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x0040e700
// FUNCTION: CARMA2_SW 0x0040e6e0
void C2_HOOK_FASTCALL MungeCarMaterials(tCar_spec* pCar, int pInternal_cam) {
    int i;

    for (i = 0; i < pCar->count_window_materials; i++) {
        int internal_map_missing = pInternal_cam;

        if (pInternal_cam) {
            int j;

            for (j = 0; j < pCar->window_materials[i].count_maps; j++) {
                if (pCar->window_materials[i].material->colour_map == pCar->window_materials[i].maps[j]) {
                    internal_map_missing = 0;
                }
            }
            if (internal_map_missing) {
                pCar->window_materials[i].material->flags &= ~BR_MATF_TWO_SIDED;
            }
        }
        if (!internal_map_missing) {
            pCar->window_materials[i].material->flags |= BR_MATF_TWO_SIDED;
        }
        BrMaterialUpdate(pCar->window_materials[i].material, BR_MATU_RENDERING);
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x0040e790
// FUNCTION: CARMA2_SW 0x0040e770
void C2_HOOK_FASTCALL SetCameraType(tActionReplayCameraMode pCamPos) {

    gAction_replay_camera_mode = pCamPos;
    switch (pCamPos) {
    case kActionReplayCameraMode_Standard:
    case kActionReplayCameraMode_Rigid:
    case kActionReplayCameraMode_Reversing:
        InitialiseExternalCamera();
        break;
    case kActionReplayCameraMode_Manual:
        gAction_replay_manual_camera_target_type = 0;
        break;
    default:
        break;
    }
    MungeCarMaterials(&gProgram_state.current_car, gAction_replay_camera_mode == kActionReplayCameraMode_Internal);
}

// ChangeCameraType

// FUNCTION: CARMA2_HW 0x0040e900
// FUNCTION: CARMA2_SW 0x0040e8e0
void C2_HOOK_FASTCALL ToggleCockpit(void) {
    C2_HOOK_BUG_ON(REC2_ASIZE(gCamera_type_allowed_replay) != 9);
    C2_HOOK_BUG_ON(REC2_ASIZE(gCamera_type_allowed_gameplay) != 9);

    if ((gCar_to_view != &gProgram_state.current_car && !gProgram_state.cockpit_on) || gMap_view == 2) {
        return;
    }
    if (gAction_replay_camera_mode == kActionReplayCameraMode_Internal) {
        tActionReplayCameraMode old_camera_mode = tActionReplayCameraMode_00679324;

        if ((gAction_replay_mode && !gCamera_type_allowed_replay[tActionReplayCameraMode_00679324])
                || (!gAction_replay_mode && !gCamera_type_allowed_gameplay[tActionReplayCameraMode_00679324])
                || tActionReplayCameraMode_00679324 >= (int)REC2_ASIZE(gCamera_type_allowed_replay)
                || (tActionReplayCameraMode_00679324 == kActionReplayCameraMode_Drone && !OKToViewDrones())
                || (old_camera_mode == kActionReplayCameraMode_Peds && !gPed_count)) {
            SetCameraType(kActionReplayCameraMode_Standard);
        } else {
            SetCameraType(tActionReplayCameraMode_00679324);
        }
    } else {
        tActionReplayCameraMode_00679324 = gAction_replay_camera_mode;
        SetCameraType(kActionReplayCameraMode_Internal);
    }
}

// PositionCarMountedCamera

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x0040ea30
// STUB: CARMA2_SW 0x0040ea10
void C2_HOOK_FASTCALL PositionExternalCamera(tCar_spec* pCar_spec, tU32 pTime) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x0040f760
// FUNCTION: CARMA2_SW 0x0040f740
void C2_HOOK_FASTCALL CameraBugFix(tCar_spec* pCar, tU32 pTime) {

    if (gAction_replay_mode
            && (gAction_replay_camera_mode == kActionReplayCameraMode_ActionTracking || gAction_replay_camera_mode == kActionReplayCameraMode_Panning)
            && gPed_actor != NULL
            && !gProgram_state.cockpit_on) {
        IncidentCam(pCar, pTime);
    }
}

// PossibleRemoveNonCarFromWorld

// PutNonCarBackInWorld

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x0040f790
// STUB: CARMA2_SW 0x0040f770
int C2_HOOK_FASTCALL IncidentCam(tCar_spec* pCar, tU32 pTime) {

    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// MoveCamToIncident

// PanningExternalCamera

// CheckForWall

// SetUpPanningCamera

// FUNCTION: CARMA2_HW 0x00410be0
// FUNCTION: CARMA2_SW 0x00410bc0
void C2_HOOK_FASTCALL SaveCameraPosition(int pIndex) {

    if (gSave_camera[pIndex].saved == 1) {
        return;
    }
    gSave_camera[pIndex].zoom = gCamera_zoom;
    gSave_camera[pIndex].yaw = gCamera_yaw;
    gSave_camera[pIndex].saved = 1;
}

// FUNCTION: CARMA2_HW 0x00410c20
// FUNCTION: CARMA2_SW 0x00410c00
void C2_HOOK_FASTCALL RestoreCameraPosition(int pIndex) {

    if (gSave_camera[pIndex].saved == 0) {
        return;
    }
    gCamera_zoom = gSave_camera[pIndex].zoom;
    gCamera_yaw = gSave_camera[pIndex].yaw;
    gSave_camera[pIndex].saved = 0;
}

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

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x00413580
// STUB: CARMA2_SW 0x004135b0
void C2_HOOK_FASTCALL InitialiseExternalCamera(void) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00413770
void C2_HOOK_FASTCALL FreezeCamera(void) {

    gCamera_frozen = 1;
}

// FUNCTION: CARMA2_HW 0x00413780
// FUNCTION: CARMA2_SW 0x004137b0
void C2_HOOK_FASTCALL GetAverageGridPosition(tRace_info* pThe_race) {
    int i;
    br_scalar total_cars;
    tCar_spec* car;

    total_cars = 0.0f;
    BrVector3SetFloat(&gAverage_grid_position, 0.0f, 0.0f, 0.0f);
    if (pThe_race->number_of_racers <= 2) {
        BrVector3Copy(&gAverage_grid_position, &gProgram_state.current_car.pos);
    } else {
        for (i = 0; i < pThe_race->number_of_racers; i++) {
            car = pThe_race->opponent_list[i].car_spec;
            BrVector3Accumulate(&gAverage_grid_position, &car->pos);
            total_cars += 1.0f;
        }
        gAverage_grid_position.v[0] /= total_cars;
        gAverage_grid_position.v[1] /= total_cars;
        gAverage_grid_position.v[2] /= total_cars;
    }
}

// FindACar

// FindAPed

// MouseClickInActionReplay

