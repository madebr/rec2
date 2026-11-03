/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "57-drone.h"

#include "08-loading1.h"
#include "15-displays.h"
#include "28-world3.h"

#include "41-utility.h"
#include "49-physics.h"
#include "52-errors.h"
#include "56-aiworld.h"
#include "60-phil.h"
#include "c2_string.h"
#include "globvars.h"
#include "platform.h"

#include "c2_stdio.h"

#include <math.h>

// GLOBAL: CARMA2_HW 0x00705b70
tU32 gINT_00705b70;

// GLOBAL: CARMA2_HW 0x006820c0
// GLOBAL: CARMA2_SW 0x0067c800
br_vector3 gRender_bounds_centre;

// GLOBAL: CARMA2_HW 0x006820d0
// GLOBAL: CARMA2_SW 0x0067c810
int gCount_drones;

// GLOBAL: CARMA2_HW 0x00682178
// GLOBAL: CARMA2_SW 0x0067c8b8
tDrone_form gDrone_forms[64];

// GLOBAL: CARMA2_HW 0x0068450c
// GLOBAL: CARMA2_SW 0x0067ec4c
int gCount_drone_forms;

// GLOBAL: CARMA2_HW 0x00684510
// GLOBAL: CARMA2_SW 0x0067ec50
int gCount_drone_path_nodes;

// GLOBAL: CARMA2_HW 0x006844f8
// GLOBAL: CARMA2_SW 0x0067ec38
int gCurrent_selected_drone;

// GLOBAL: CARMA2_HW 0x00684504
// GLOBAL: CARMA2_SW 0x0067ec44
tDrone_spec* gDrone_specs;

// GLOBAL: CARMA2_HW 0x00684508
// GLOBAL: CARMA2_SW 0x0067ec48
tDrone_path_node* gDrone_path_nodes;

// GLOBAL: CARMA2_HW 0x00684518
// GLOBAL: CARMA2_SW 0x0067ec58
int gTraffic_disabled;

// GLOBAL: CARMA2_HW 0x00681fb0
// GLOBAL: CARMA2_SW 0x0067c6f0
int gINT_00681fb0;

// GLOBAL: CARMA2_HW 0x00684514
// GLOBAL: CARMA2_SW 0x0067ec54
int gFrame;

// GLOBAL: CARMA2_HW 0x006820d8
tDrone_crush_data gDrone_crush_data;


// GLOBAL: CARMA2_HW 0x0068452c
int gShow_drone_paths;

// GLOBAL: CARMA2_HW 0x005947c0
br_vector3 gDefault_drone_direction = {{ 0.f, 0.f, -1.f }};


/* separate 0.0f in .rdata used only here (not the shared 0x0058a988 literal) */
// GLOBAL: CARMA2_HW 0x00589dc4
float gDrone_render_zero = 0.0f;

extern void C2_HOOK_CDECL DoNotDprintf_drone(const char* pMessage, ...);
extern tDrone_state_function* gDrone_state_functions[6];
extern int C2_HOOK_FASTCALL PHILAddObject(tPhysics_object* pObject);
extern int C2_HOOK_CDECL PHILSetObjectProperty(tPhysics_object* pObject, int pProperty, ...);
extern void C2_HOOK_FASTCALL PipeSingleDroneRender(tDrone_spec* pDrone_spec, int pRender);
extern void C2_HOOK_FASTCALL DroneStateFuncReset(tDrone_spec* pDrone, tDroneStateFuncState state);
extern void C2_HOOK_FASTCALL PipeDroneMatrix(tDrone_spec* pDrone);
extern void C2_HOOK_FASTCALL MoveThisDronePlane(tDrone_spec* pDrone);
extern void C2_HOOK_FASTCALL MoveThisDroneCar(tDrone_spec* pDrone);
extern float C2_HOOK_FASTCALL ARGetReplayRate(void);
extern void C2_HOOK_FASTCALL PipeSingleDroneStraightPos(tDrone_spec* pDrone, undefined4 pSection, tU16 pSpeed, tU16 pDistance);

// DoNotDprintf

// FUNCTION: CARMA2_HW 0x00451620
void C2_HOOK_FASTCALL CrappyLittleVector3DPrintf(const char* pMessage, br_vector3* pPosition) {
    DoNotDprintf_drone("%d: %s: %3.3f, %3.3f, %3.3f", gFrame, pMessage, pPosition->v[0], pPosition->v[1], pPosition->v[2]);
}

// CrappyLittleDrivingStateInfoDprintf

// DroneStraightestArcForCorner2D

// FUNCTION: CARMA2_HW 0x0044f640
// FUNCTION: CARMA2_SW 0x0044f3e0
FILE* C2_HOOK_FASTCALL OpenDroneFile(char* pDrone_name) {
    tPath_name the_path;
    FILE* f;

    strcpy(the_path, gApplication_path);
    PathCat(the_path, the_path, "DRONES");
    PathCat(the_path, the_path, pDrone_name);
    PathCat(the_path, the_path, pDrone_name);
    strcat(the_path, ".TXT");
    f = DRfopen(the_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_UnableToOpenDroneFileOrFileCorrupted_S, the_path);
    }
    return f;
}

// LoadDroneTypeInfo

void C2_HOOK_FASTCALL ReallyRemoveDroneFromPHIL(tDrone_spec* pDrone) {
    PHILRemoveObject(&pDrone->collision_info);
}

void C2_HOOK_FASTCALL RemoveDroneFromPHIL(tDrone_spec* pDrone) {
    ReallyRemoveDroneFromPHIL(pDrone);
}

// InitDroneCollisionObject

// FUNCTION: CARMA2_HW 0x00451650
int C2_HOOK_FASTCALL ReallyAddDroneToPHIL(tDrone_spec* pDrone) {

    if (PHILAddObject(&pDrone->collision_info)) {
        return 0;
    }
    PHILSetObjectProperty(&pDrone->collision_info, 0, 1.0);
    PHILSetObjectProperty(&pDrone->collision_info, 3, 1);
    PHILSetObjectProperty(&pDrone->collision_info, 7, 1);
    PHILSetObjectProperty(&pDrone->collision_info, 6, 1);
    return 1;
}

// AddDroneToPHIL

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x00451070
void C2_HOOK_FASTCALL PreprocessDronePaths(void) {
    int i;
    int j;

    for (i = 0; i < gCount_drone_path_nodes; i++) {
        for (j = 0; j < gDrone_path_nodes[i].count_sections; j++) {
            BrVector3Sub(&gDrone_path_nodes[i].sections[j].field_0x04, &gDrone_path_nodes[gDrone_path_nodes[i].sections[j].node1].position, &gDrone_path_nodes[i].position);
            gDrone_path_nodes[i].sections[j].field_0x1c = BrVector3Length(&gDrone_path_nodes[i].sections[j].field_0x04);
            {
                br_scalar len;
                double scale;

                len = BrVector3Length(&gDrone_path_nodes[i].sections[j].field_0x04);
                if (len > BR_SCALAR_EPSILON * 2) {
                    scale = 1.0 / len;
                    gDrone_path_nodes[i].sections[j].field_0x10.v[0] = gDrone_path_nodes[i].sections[j].field_0x04.v[0] * scale;
                    gDrone_path_nodes[i].sections[j].field_0x10.v[1] = gDrone_path_nodes[i].sections[j].field_0x04.v[1] * scale;
                    gDrone_path_nodes[i].sections[j].field_0x10.v[2] = gDrone_path_nodes[i].sections[j].field_0x04.v[2] * scale;
                } else {
                    gDrone_path_nodes[i].sections[j].field_0x10.v[0] = 1.0f;
                    gDrone_path_nodes[i].sections[j].field_0x10.v[1] = 0.0f;
                    gDrone_path_nodes[i].sections[j].field_0x10.v[2] = 0.0f;
                }
            }
        }
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00450600
// FUNCTION: CARMA2_SW 0x00450390
int C2_HOOK_CDECL LinkyCallback(br_actor* pActor, void* pData) {
    tLinkyCallback_context* context;
    int i;

    context = pData;
    if (pActor->type != BR_ACTOR_MODEL) {
        return 0;
    }
    pActor->render_style = BR_RSTYLE_DEFAULT;
    for (i = context->model_start; i < context->model_start + context->model_end; i++) {
        if (DRStricmp(gDroneStorage.models[i]->identifier, pActor->model->identifier) == 0) {
            pActor->model = gDroneStorage.models[i];
            return 0;
        }
    }
    PDFatalError("No drone model");
    return 0;
}

// LinkDroneActorsToModelsAndSetRenderStyle

// LoadDroneActorsModels

// ReadPastBoundsShapesHeader

// ReadPastThisLine

// MatchFGType

// GetAxisFromString

// GetSpeedControlFromString

// FUNCTION: CARMA2_HW 0x00450b30
// FUNCTION: CARMA2_SW 0x004508c0
br_actor* C2_HOOK_CDECL MrFindy(br_actor* pActor, const char* pName) {

    if (DRStricmp(pActor->identifier, pName) == 0) {
        return pActor;
    } else {
        return NULL;
    }
}

// FindDroneChildActor

// GetReversenessFromString

// LoadFunksAndGrooves

// LoadDrone

// CalculateDrivingInfo

// PauseDroneState

// UnPauseDroneState

// NewDroneState

// ResetDroneCrushyModel

// ResetDroneModel

// FUNCTION: CARMA2_HW 0x0044ca90
// FUNCTION: CARMA2_SW 0x0044c840
int C2_HOOK_FASTCALL DroneCarWithinRenderingDistance(const br_vector3* pPos) {

    if ((float)fabs(gRender_bounds_centre.v[0] - pPos->v[0]) < 15.f
        && (float)fabs(gRender_bounds_centre.v[1] - pPos->v[1]) < 12.f
        && (float)fabs(gRender_bounds_centre.v[2] - pPos->v[2]) < 15.f) {
        return 1;
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x0044cae0
// FUNCTION: CARMA2_SW 0x0044c890
int C2_HOOK_FASTCALL DronePlaneWithinRenderingDistance(const br_vector3* pPos) {

    if ((float)fabs(gRender_bounds_centre.v[0] - pPos->v[0]) < 40.f
        && (float)fabs(gRender_bounds_centre.v[1] - pPos->v[1]) < 30.f
        && (float)fabs(gRender_bounds_centre.v[2] - pPos->v[2]) < 40.f) {
        return 1;
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x0044cb30
// FUNCTION: CARMA2_SW 0x0044c8e0
int C2_HOOK_FASTCALL DroneTrainWithinRenderingDistance(const br_vector3* pPos) {

    if ((float)fabs(gRender_bounds_centre.v[0] - pPos->v[0]) < 50.f
        && (float)fabs(gRender_bounds_centre.v[1] - pPos->v[1]) < 20.f
        && (float)fabs(gRender_bounds_centre.v[2] - pPos->v[2]) < 50.f) {
        return 1;
    } else {
        return 0;
    }
}

// SemiInitDroneSpec

// FUNCTION: CARMA2_HW 0x00451210
void C2_HOOK_FASTCALL InitDroneSpec(tDrone_spec* pDrone_spec, int pNode) {

    C2_HOOK_BUG_ON(sizeof(*pDrone_spec) != 0x5d8);

    DoNotDprintf_drone("InitDroneSpec() - INITIALISING DRONE SPEC");
    memset(pDrone_spec, 0, sizeof(*pDrone_spec));
    pDrone_spec->field_0xa_pathnode_id = pNode;
    pDrone_spec->field_0x8_pathnode_id = pNode;
    pDrone_spec->field_0xe = -1;
    pDrone_spec->field_0xc = -1;
    BrVector3Set(&pDrone_spec->field_0x18, 0.f, 0.f, 0.f);
    BrVector3Copy(&pDrone_spec->pos, &gDrone_path_nodes[pNode].position);
    pDrone_spec->field_0xdc = 0;
    pDrone_spec->form = &gDrone_forms[gDrone_path_nodes[pNode].type];
    pDrone_spec->field_0xf4 = -1;
    pDrone_spec->current_state = 0;
    pDrone_spec->field_0x5d4 = 0;
    pDrone_spec->field_0xe4 = 1.0f;
    pDrone_spec->field_0x48 = 1.0f;
}

// ChooseNextNode

// DroneOrientationChanged

// DroneyObjectInSensiblePlace

// FUNCTION: CARMA2_HW 0x004516b0
void C2_HOOK_FASTCALL DoDefaultDroneStateAction(tDrone_spec* pDrone_spec) {

    if (gDrone_state_functions[pDrone_spec->current_state] != NULL) {
        gDrone_state_functions[pDrone_spec->current_state](pDrone_spec, eDrone_state_DEFAULT);
    }
}

// FUNCTION: CARMA2_HW 0x004516d0
void C2_HOOK_FASTCALL StopRenderingThisDrone(tDrone_spec* pDrone_spec) {
    if (pDrone_spec->actor->render_style == BR_RSTYLE_FACES) {
        DoNotDprintf_drone("STOP RENDERING: %d", pDrone_spec->id);
        gCount_rendered_drones -= 1;
        pDrone_spec->actor->render_style = BR_RSTYLE_NONE;
        PipeSingleDroneRender(pDrone_spec, 0);
    }
}

// StopProcessingThisDrone

// FUNCTION: CARMA2_HW 0x00451810
void C2_HOOK_FASTCALL StartRenderingThisDrone(tDrone_spec* pDrone) {

    if (pDrone->actor->render_style == BR_RSTYLE_FACES) {
        return;
    }
    if (gShow_drone_paths && pDrone->actor->render_style == BR_RSTYLE_BOUNDING_FACES) {
        return;
    }
    if (gShow_drone_paths || (pDrone->field_0x44 && gCount_rendered_drones < 10)) {

        DoNotDprintf_drone("START RENDERING: %d", pDrone->id);
        gCount_rendered_drones += 1;
        pDrone->actor->render_style = BR_RSTYLE_FACES;
        PipeSingleDroneRender(pDrone, 1);
    } else {
        DoNotDprintf_drone("NewDroneState() Drone %d, Current state %d, New state %d", pDrone->id, pDrone->current_state, 1);
        if (pDrone->current_state != 1) {
            if (gDrone_state_functions[pDrone->current_state] != NULL) {
                gDrone_state_functions[pDrone->current_state](pDrone, eDrone_state_STOP);
            }
            pDrone->time_last_munge = gTime_stamp_for_this_munging;
            pDrone->prev_states[1] = pDrone->prev_states[0];
            pDrone->prev_states[0] = pDrone->current_state;
            pDrone->current_state = 1;
            if (gDrone_state_functions[1] != NULL) {
                gDrone_state_functions[1](pDrone, eDrone_state_START);
            }
        }
    }
}

// CheckDroneInSensiblePlaceBeforeStartingToProcessTheCuntingThing

// StartProcessingThisDrone

// FUNCTION: CARMA2_HW 0x0044cb80
// FUNCTION: CARMA2_SW 0x0044c930
int C2_HOOK_FASTCALL DroneCarWithinProcessingDistance(const br_vector3* pPos) {

    if ((float)fabs(gRender_bounds_centre.v[0] - pPos->v[0]) < 18.f
        && (float)fabs(gRender_bounds_centre.v[1] - pPos->v[1]) < 13.f
        && (float)fabs(gRender_bounds_centre.v[2] - pPos->v[2]) < 18.f) {
        return 1;
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x0044cbd0
// FUNCTION: CARMA2_SW 0x0044c980
int C2_HOOK_FASTCALL DronePlaneWithinProcessingDistance(const br_vector3* pPos) {

    if ((float)fabs(gRender_bounds_centre.v[0] - pPos->v[0]) < 43.f
        && (float)fabs(gRender_bounds_centre.v[1] - pPos->v[1]) < 31.f
        && (float)fabs(gRender_bounds_centre.v[2] - pPos->v[2]) < 43.f) {
        return 1;
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x0044cc20
// FUNCTION: CARMA2_SW 0x0044c9d0
int C2_HOOK_FASTCALL DroneTrainWithinProcessingDistance(const br_vector3* pPos) {

    if ((float)fabs(gRender_bounds_centre.v[0] - pPos->v[0]) < 43.f
        && (float)fabs(gRender_bounds_centre.v[1] - pPos->v[1]) < 31.f
        && (float)fabs(gRender_bounds_centre.v[2] - pPos->v[2]) < 43.f) {
        return 1;
    } else {
        return 0;
    }
}

// CalcRenderBoundsCentre

// DoDroneDistanceChecks

// DoDroneFunkyGroovyThings

// ProcessThisDrone

// InitDroneDrivingInfo

// PossiblyPipeDroneMovement

// FUNCTION: CARMA2_HW 0x0044d1d0
void C2_HOOK_FASTCALL DroneStateFuncControlledMovement(tDrone_spec* pDrone, tDroneStateFuncState state) {

    switch ((int)state) {
    case eDrone_state_START:
        DoNotDprintf_drone("DroneStateFuncControlledMovement() START");
        pDrone->field_0xdc = 0;
        pDrone->field_0xe4 = 0.f;
        pDrone->field_0x48 = 0.f;
        pDrone->field_0x4c = 0.f;
        break;
    case eDrone_state_RUN:
        DoNotDprintf_drone("DroneStateFuncControlledMovement() RUN");
        if (pDrone->field_0x44) {
            switch (pDrone->form->type) {
            case kDroneType_plane:
                MoveThisDronePlane(pDrone);
                break;
            default:
                MoveThisDroneCar(pDrone);
                break;
            }
            if (pDrone->field_0xdc == 1) {
                PipeSingleDroneStraightPos(pDrone, pDrone->field_0x10,
                    DRScalarToU16(pDrone->field_0x48, -500.f, 500.f),
                    DRScalarToU16(pDrone->field_0x4c, 0.f, 255.f));
                return;
            }
            if (pDrone->field_0xdc == 2) {
                PipeDroneMatrix(pDrone);
                return;
            }
            PDEnterDebugger("Keith");
            return;
        }
        break;
    default:
        DoNotDprintf_drone("DroneStateFuncControlledMovement() DEFAULT");
        break;
    }
}

// DoDroneHaltingStuff

// FUNCTION: CARMA2_HW 0x0044eb70
void C2_HOOK_FASTCALL DroneStateFuncPhysicsActive(tDrone_spec* pDrone, tDroneStateFuncState state) {

    switch ((int)state) {
    case eDrone_state_RUN:
        DoNotDprintf_drone("DroneStateFuncPhysicsActive() RUN");
        if (pDrone->field_0x44) {
            if (!(pDrone->form->flags & 0x4)) {
                return;
            }
            if (pDrone->field_0x45) {
                return;
            }
        }
        if (!(pDrone->form->flags & 0x2)) {
            return;
        }
        PHILRemoveObject(&pDrone->collision_info);
        pDrone->collision_info.field_0x1a0 = 0x100000;
        PHILSetObjectProperty(&pDrone->collision_info, 3, 1);
        pDrone->field_0x5d4 = 0;
        DoNotDprintf_drone("NewDroneState() Drone %d, Current state %d, New state %d", pDrone->id, pDrone->current_state, 1);
        if (pDrone->current_state != 1) {
            if (gDrone_state_functions[pDrone->current_state] != NULL) {
                gDrone_state_functions[pDrone->current_state](pDrone, eDrone_state_STOP);
            }
            pDrone->time_last_munge = gTime_stamp_for_this_munging;
            pDrone->prev_states[1] = pDrone->prev_states[0];
            pDrone->prev_states[0] = pDrone->current_state;
            pDrone->current_state = 1;
            if (gDrone_state_functions[1] != NULL) {
                gDrone_state_functions[1](pDrone, eDrone_state_START);
            }
        }
        break;
    }
}

// FUNCTION: CARMA2_HW 0x0044ec50
void C2_HOOK_FASTCALL DroneStateFuncStationaryPassive(tDrone_spec* pDrone, tDroneStateFuncState state) {

    switch ((int)state) {
    case eDrone_state_RUN:
        DoNotDprintf_drone("DroneStateFuncStationaryPassive() RUN");
        if (pDrone->field_0x44) {
            if (!(pDrone->form->flags & 0x4)) {
                return;
            }
            if (pDrone->field_0x45) {
                return;
            }
        }
        if (!(pDrone->form->flags & 0x2)) {
            return;
        }
        DoNotDprintf_drone("NewDroneState() Drone %d, Current state %d, New state %d", pDrone->id, pDrone->current_state, 1);
        if (pDrone->current_state != 1) {
            if (gDrone_state_functions[pDrone->current_state] != NULL) {
                gDrone_state_functions[pDrone->current_state](pDrone, eDrone_state_STOP);
            }
            pDrone->time_last_munge = gTime_stamp_for_this_munging;
            pDrone->prev_states[1] = pDrone->prev_states[0];
            pDrone->prev_states[0] = pDrone->current_state;
            pDrone->current_state = 1;
            if (gDrone_state_functions[1] != NULL) {
                gDrone_state_functions[1](pDrone, eDrone_state_START);
            }
        }
        break;
    }
}

// DroneStateFuncStationaryPassive

// GetNewCrushyModelForDrone

// FUNCTION: CARMA2_HW 0x0044ecf0
// FUNCTION: CARMA2_SW 0x0044ea90
void C2_HOOK_FASTCALL SetTrafficOn(int pTraffic_on) {

    gTraffic_disabled = !pTraffic_on;
}

// FUNCTION: CARMA2_HW 0x0044ed00
// FUNCTION: CARMA2_SW 0x0044eaa0
int C2_HOOK_FASTCALL OtherPerambulatoryInstasificationIsEnablificated(void) {
    return !gTraffic_disabled;
}

// STUB: CARMA2_HW 0x0044ed10
// STUB: CARMA2_SW 0x0044eab0
void C2_HOOK_FASTCALL LoadPanGameDroneInfo(void) {
    NOT_IMPLEMENTED();
}

// InitDrones

// FUNCTION: CARMA2_HW 0x0044fc10
// FUNCTION: CARMA2_SW 0x0044f9b0
void C2_HOOK_FASTCALL DisposeDronesRaceStuff(void) {
    int i;

    if (gCount_drones == 0) {
        return;
    }

    for (i = 0; i < gCount_drones; i++) {

        if (gDrone_specs[i].actor->parent != NULL) {
            BrActorRemove(gDrone_specs[i].actor);
        }
        BrActorFree(gDrone_specs[i].actor);
        RemoveDroneFromPHIL(&gDrone_specs[i]);
        if (gDrone_specs[i].field_0x46) {
            FreeShape(gDrone_specs[i].collision_info.shape);
            gDrone_specs[i].field_0x46 = 0;
        }

        if (gDrone_specs[i].funk_grooves != NULL) {
            BrMemFree(gDrone_specs[i].funk_grooves);
            gDrone_specs[i].funk_grooves = NULL;
        }
    }

    for (i = 0; i < gCount_drone_forms; i++) {

        if (gDrone_forms[i].field_0x80 != NULL) {
            int j;

            BrMemFree(gDrone_forms[i].field_0x80);
            gDrone_forms[i].field_0x80 = NULL;
            for (j = 0; j < gDrone_forms[i].count_models; j++) {
                BrModelRemove(gDrone_forms[i].models[j]);
                gDrone_forms[i].models[j]->nfaces = 0;
                gDrone_forms[i].models[j]->faces = NULL;
                BrModelFree(gDrone_forms[i].models[j]);
            }
            if (gDrone_forms[i].models != NULL) {
                BrMemFree(gDrone_forms[i].models);
                gDrone_forms[i].models = NULL;
            }
        }
    }
    ClearOutStorageSpace(&gDroneStorage);
    BrMemFree(gDrone_specs);
    gDrone_specs = NULL;
    gCount_drones = 0;
    BrMemFree(gDrone_path_nodes);
    gDrone_path_nodes = NULL;
    gCount_drone_path_nodes = 0;
}

// STUB: CARMA2_HW 0x0044fda0
// STUB: CARMA2_SW 0x0044fb40
void C2_HOOK_FASTCALL LoadPerRaceDroneStuff(void) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00450b50
tDrone_crush_data* C2_HOOK_FASTCALL GetDroneCrushData(tPhysics_object* pObject) {
    tDrone_spec* pDrone;
    int i;

    pDrone = (tDrone_spec*)pObject->owner;
    if (pDrone->form->crushability == gDrone_render_zero) {
        return NULL;
    }
    if (pDrone->field_0xf4 < 0) {
        for (i = 0; i < pDrone->form->count_models; i++) {
            if (pDrone->form->field_0x80[i] == 0) {
                pDrone->form->field_0x80[i] = 1;
                pDrone->field_0xf4 = i;
                pDrone->model_actor->model = pDrone->form->models[i];
                goto selected_model;
            }
        }
        return NULL;
    }
selected_model:
    gDrone_crush_data.model = pDrone->model_actor->model;
    gDrone_crush_data.crushability = pDrone->form->crushability;
    gDrone_crush_data.bounds = pDrone->form->field_0x54;
    return &gDrone_crush_data;
}

// AllocateAndInitDrones

// STUB: CARMA2_HW 0x00450bf0
// STUB: CARMA2_SW 0x00450980
void C2_HOOK_FASTCALL LoadInDronePaths(FILE* pF) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004512d0
// FUNCTION: CARMA2_SW 0x00451060
void C2_HOOK_FASTCALL DronePreCollisionStuff(void) {
    gINT_00681fb0 = 0;
    gFrame++;
}

// DoDronePerGameFrameStuff

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x00451e70
// STUB: CARMA2_SW 0x00451c00
void C2_HOOK_FASTCALL DoDroneFunkyGroovyThings(tDrone_spec* pDrone) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

extern int C2_HOOK_FASTCALL PHILMakeObjectActive(tPhysics_object* pObject, const br_vector3* pVelocity, const br_vector3* pOmega, int pFlag);
extern int gMutant_tail_state;

// FUNCTION: CARMA2_HW 0x00452230
int C2_HOOK_FASTCALL MyDroneHathCollideth(tPhysics_object* pObject, tPhysics_object* pOther) {
    tDrone_spec* pDrone;

    DoNotDprintf_drone("MyDroneHathCollideth(): PHIL called a bloody callback. Rejoice!");
    pDrone = (tDrone_spec*)pObject->owner;
    pObject->disable_move_rotate = 0;
    pObject->field_0xed = 1;
    pObject->field_0x1a0 = 0x200000;
    pObject->v.v[0] = pDrone->field_0x18.v[0];
    pObject->v.v[1] = pDrone->field_0x18.v[1];
    pObject->v.v[2] = pDrone->field_0x18.v[2];
    PHILMakeObjectActive(pObject, NULL, NULL, 1);
    PHILSetObjectProperty(pObject, 3, 0);
    if ((pOther != NULL && pOther->owner != NULL && pOther->flags_0x238 == 1)
            || (pOther->flags_0x238 == 0x20
                && (gMutant_tail_state == 2 || GetTotalTime() - gINT_00705b70 < 0x1d4c))) {
        ((tDrone_spec*)pObject->owner)->field_0x5d4 = (undefined4)pOther->owner;
    } else if (pOther != NULL && pOther->flags_0x238 == 0x40) {
        ((tDrone_spec*)pObject->owner)->field_0x5d4 = ((tDrone_spec*)pOther->owner)->time_last_munge;
    }
    pDrone = (tDrone_spec*)pObject->owner;
    DoNotDprintf_drone("NewDroneState() Drone %d, Current state %d, New state %d", pDrone->id, pDrone->current_state, 3);
    if (pDrone->current_state != 3) {
        if (gDrone_state_functions[pDrone->current_state] != NULL) {
            gDrone_state_functions[pDrone->current_state](pDrone, eDrone_state_STOP);
        }
        pDrone->time_last_munge = gTime_stamp_for_this_munging;
        pDrone->prev_states[1] = pDrone->prev_states[0];
        pDrone->prev_states[0] = pDrone->current_state;
        pDrone->current_state = 3;
        if (gDrone_state_functions[3] != NULL) {
            gDrone_state_functions[3](pDrone, eDrone_state_START);
        }
    }
    return 0;
}

// MyDroneHathHalteth

// MoveMyDroneBaby

// DroneNoOp

// FUNCTION: CARMA2_HW 0x004525d0
// FUNCTION: CARMA2_SW 0x00452360
void C2_HOOK_FASTCALL SelectNextDrone(void) {
    char s[64];

    gCurrent_selected_drone++;
    if (gCurrent_selected_drone >= gCount_drones) {
        gCurrent_selected_drone = 0;
    }
    sprintf(s, "Drone %d", gCurrent_selected_drone);
    NewTextHeadupSlot(4, 0, 3000, -1, s);
}

// FUNCTION: CARMA2_HW 0x00452680
// FUNCTION: CARMA2_SW 0x00452410
int C2_HOOK_FASTCALL OKToViewDrones(void) {
    return 0;
}

// FUNCTION: CARMA2_HW 0x00452690
// FUNCTION: CARMA2_SW 0x00452420
br_matrix34* C2_HOOK_FASTCALL GetCurrentViewDroneMat(void) {
    return &gDrone_specs[gCurrent_selected_drone].actor->t.t.mat;
}

// GetCurrentViewDroneDirection

// STUB: CARMA2_HW 0x00452810
void C2_HOOK_FASTCALL ApplyDroneRender2(int pIndex, int pRender) { NOT_IMPLEMENTED(); }

// FUNCTION: CARMA2_HW 0x00452880
// FUNCTION: CARMA2_SW 0x00452610
void C2_HOOK_FASTCALL ApplyDroneStraightPos2(int pDrone_id, tDrone_path_node_section* pSection, tU16 pSpeed, tU16 pDistance) {
    br_vector3 direction;
    tDrone_spec* drone;

    drone = &gDrone_specs[pDrone_id];
    drone->field_0xe4 = DRU16ToScalar(pSpeed, -500.0f, 500.0f);
    drone->field_0xe0 = 0.0f;
    BrVector3Scale(&direction, &pSection->field_0x10, pSection->field_0x1c - DRU16ToScalar(pDistance, 0.0f, 255.0f));
    BrVector3Negate(&direction, &direction);
    BrVector3Add(&drone->actor->t.t.translate.t, &gDrone_path_nodes[pSection->node1].position, &direction);
    BrVector3Copy(&direction, &pSection->field_0x10);
    if (drone->form->flags & 0x10) {
        direction.v[1] = 0.0f;
    }
    PointActorAlongThisBloodyVector(drone->actor, &direction);
    DoDroneFunkyGroovyThings(drone);
}

// FUNCTION: CARMA2_HW 0x004529d0
// FUNCTION: CARMA2_SW 0x00452760
void C2_HOOK_FASTCALL ApplyDroneCornerPos2(int pDrone_id, tU16 pSpeed, tU16 pField_0x74, tU16 pRadius, const tCompressed_matrix34* pMatrix) {
    tDrone_spec* drone;

    drone = &gDrone_specs[pDrone_id];
    drone->field_0xe4 = DRU16ToScalar(pSpeed, -500.0f, 500.0f);
    drone->field_0xe0 = DRU16ToScalar(pField_0x74, 0.0f, 1.0f);
    drone->field_0xe8 = DRU16ToScalar(pRadius, 0.0f, 100.0f);
    BrVector3Set((br_vector3*)drone->actor->t.t.mat.m[0],
        DRU16ToScalar(pMatrix->m[0].v[0], -1.0f, 1.0f),
        DRU16ToScalar(pMatrix->m[0].v[1], -1.0f, 1.0f),
        DRU16ToScalar(pMatrix->m[0].v[2], -1.0f, 1.0f));
    BrVector3Set((br_vector3*)drone->actor->t.t.mat.m[1],
        DRU16ToScalar(pMatrix->m[1].v[0], -1.0f, 1.0f),
        DRU16ToScalar(pMatrix->m[1].v[1], -1.0f, 1.0f),
        DRU16ToScalar(pMatrix->m[1].v[2], -1.0f, 1.0f));
    BrVector3Set((br_vector3*)drone->actor->t.t.mat.m[2],
        DRU16ToScalar(pMatrix->m[2].v[0], -1.0f, 1.0f),
        DRU16ToScalar(pMatrix->m[2].v[1], -1.0f, 1.0f),
        DRU16ToScalar(pMatrix->m[2].v[2], -1.0f, 1.0f));
    BrVector3Set((br_vector3*)drone->actor->t.t.mat.m[3],
        DRU16ToScalar(pMatrix->m[3].v[0], -1000.0f, 1000.0f),
        DRU16ToScalar(pMatrix->m[3].v[1], -1000.0f, 1000.0f),
        DRU16ToScalar(pMatrix->m[3].v[2], -1000.0f, 1000.0f));
    DoDroneFunkyGroovyThings(drone);
}

// ReallocModelFacesAndVertices

// CalcNFacesAndVerticesInEditModel

// InitialiseEditModelsEtc

// UpdateEditModels

// WriteOutDronePaths

// GLOBAL: CARMA2_HW 0x00684500
int gCount_rendered_drones;

// GLOBAL: CARMA2_HW 0x00594738
tDrone_state_function* gDrone_state_functions[6];

// GLOBAL: CARMA2_HW 0x00594780
const char* gGroove_funk_type_names[4];

// STUB: CARMA2_HW 0x0044e540
void C2_HOOK_FASTCALL MoveThisDroneCar(tDrone_spec* pDrone) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x0044d2a0
void C2_HOOK_FASTCALL MoveThisDronePlane(tDrone_spec* pDrone) { NOT_IMPLEMENTED(); }

// STUB: CARMA2_HW 0x0044cfd0
void C2_HOOK_FASTCALL PipeDroneMatrix(tDrone_spec* pDrone) { NOT_IMPLEMENTED(); }
