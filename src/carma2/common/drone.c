#include "drone.h"

#include "errors.h"
#include "loading.h"
#include "physics.h"

#include <brender/brender.h>

#include "c2_stdio.h"
#include "c2_string.h"

#include <stdarg.h>

C2_HOOK_VARIABLE_IMPLEMENT(int, gCount_drones, 0x006820d0);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tDrone_form, gDrone_forms, 64, 0x00682178);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCount_drone_forms, 0x0068450c);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(tDrone_state_function*, gDrone_state_functions, 6, 0x00594738, {
    NULL,
    DroneStateFuncReset,
    DroneStateFuncControlledMovement,
    DroneStateFuncPhysicsActive,
    DroneStateFuncStationaryPassive,
    NULL,
});
C2_HOOK_VARIABLE_IMPLEMENT(tCollision_info*, gList_collision_infos, 0x0074a5f0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gDrones_unmodified, 0x006820b8);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCount_rendered_drones, 0x00684500);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCurrent_selected_drone, 0x006844f8);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_006820d4, 0x006820d4);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_006844fc, 0x006844fc);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_00681fb0, 0x00681fb0);
C2_HOOK_VARIABLE_IMPLEMENT(tDrone_spec*, gDrone_specs, 0x00684504);
C2_HOOK_VARIABLE_IMPLEMENT(int, gShow_drone_paths, 0x0068452c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gCount_active_drones, 0x00681fb4);
C2_HOOK_VARIABLE_IMPLEMENT(int, gFrame, 0x00684514);

void C2_HOOK_CDECL DroneDebug(const char* format, ...) {
// Disabled because too noisy
#if 0
    va_list ap;

    va_start(ap, format);
    c2_vfprintf(c2_stderr, format, ap);
    c2_fputc('\n', c2_stderr);
    va_end(ap);
#endif
}
C2_HOOK_FUNCTION(0x0044cfc0, DroneDebug)

void (C2_HOOK_FASTCALL * LoadInDronePaths_original)(FILE* pF);
void C2_HOOK_FASTCALL LoadInDronePaths(FILE* pF) {

#if defined(C2_HOOKS_ENABLED)
    LoadInDronePaths_original(pF);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00450bf0, LoadInDronePaths, LoadInDronePaths_original)

void C2_HOOK_FASTCALL DoDefaultDroneStateAction(tDrone_spec* pDrone_spec) {

    if (C2V(gDrone_state_functions)[pDrone_spec->current_state] != NULL) {
        C2V(gDrone_state_functions)[pDrone_spec->current_state](pDrone_spec, eDrone_state_DEFAULT);
    }
}
C2_HOOK_FUNCTION(0x004516b0, DoDefaultDroneStateAction)

void C2_HOOK_FASTCALL NewDroneState(tDrone_spec* pDrone_spec, int pNew_state) {
    DroneDebug("NewDroneState() Drone %d, Current state %d, New state %d",
        pDrone_spec->id, pDrone_spec->current_state, pNew_state);
    if (pDrone_spec->current_state != 1) {
        if (C2V(gDrone_state_functions)[pDrone_spec->current_state] != NULL) {
            C2V(gDrone_state_functions)[pDrone_spec->current_state](pDrone_spec, eDrone_state_STOP);
        }
        pDrone_spec->time_last_munge = C2V(gTime_stamp_for_this_munging);
        pDrone_spec->prev_states[1] = pDrone_spec->prev_states[0];
        pDrone_spec->prev_states[0] = pDrone_spec->current_state;
        pDrone_spec->current_state = pNew_state;
        if (C2V(gDrone_state_functions)[pNew_state] != NULL) {
            C2V(gDrone_state_functions)[pNew_state](pDrone_spec, eDrone_state_START);
        }
    }
}

int (C2_HOOK_FASTCALL * DroneCollisionInfoCollides_original)(tCollision_info* pCollision_1, tCollision_info* pCollision_2);
int C2_HOOK_FASTCALL DroneCollisionInfoCollides(tCollision_info* pCollision_1, tCollision_info* pCollision_2) {

#if defined(C2_HOOKS_ENABLED)
    return DroneCollisionInfoCollides_original(pCollision_1, pCollision_2);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00428d40, DroneCollisionInfoCollides, DroneCollisionInfoCollides_original)

int C2_HOOK_FASTCALL DroneHasCollided(tDrone_spec* pDrone_spec) {
    tCollision_info* collision = C2V(gList_collision_infos);
    for (collision = C2V(gList_collision_infos); collision != NULL; collision = collision->next) {
        if (&pDrone_spec->collision_info != collision && DroneCollisionInfoCollides(&pDrone_spec->collision_info, collision)) {
            return 1;
        }
    }
    return 0;
}

void (C2_HOOK_FASTCALL * InitDroneCollisionInfo_original)(tDrone_spec *pDrone_spec);
void C2_HOOK_FASTCALL InitDroneCollisionInfo(tDrone_spec *pDrone_spec) {

#if defined(C2_HOOKS_ENABLED)
    InitDroneCollisionInfo_original(pDrone_spec);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0044f980, InitDroneCollisionInfo, InitDroneCollisionInfo_original)

void C2_HOOK_FASTCALL DroneDebugPosition(const char* pMessage, br_vector3* pPosition) {
    DroneDebug("%d: %s: %3.3f, %3.3f, %3.3f", C2V(gFrame), pMessage, pPosition->v[0], pPosition->v[1], pPosition->v[2]);
}
C2_HOOK_FUNCTION(0x00451620, DroneDebugPosition)


void C2_HOOK_FASTCALL MakeDroneActive(tDrone_spec* pDrone_spec) {

    BrMatrix34Copy(&pDrone_spec->collision_info.transform_matrix, &pDrone_spec->actor->t.t.mat);
    pDrone_spec->field_0x44 = 1;
    if ((pDrone_spec->form->flags & 0x4) == 0) {
         C2V(gCount_active_drones) += 1;
    }
    if (C2V(gDrone_state_functions)[pDrone_spec->current_state] != NULL) {
        C2V(gDrone_state_functions)[pDrone_spec->current_state](pDrone_spec, eDrone_state_DEFAULT);
    }
    DroneDebug("PROCESSING ON: Frame %d, Drone %d, state %d", C2V(gFrame), pDrone_spec->id, pDrone_spec->current_state);
    DroneDebugPosition("    Pos", &pDrone_spec->actor->t.t.translate.t);
}

void C2_HOOK_FASTCALL InitDrones(void) {
    int i;

    C2_HOOK_BUG_ON(sizeof(tDrone_spec) != 1496);

    C2V(gDrones_unmodified) = 1;
    C2V(gCount_rendered_drones) = 0;
    C2V(gCurrent_selected_drone) = 0;
    C2V(gINT_006820d4) = 0;
    C2V(gINT_006844fc) = 0;
    C2V(gINT_00681fb0) = 0;

    for (i = 0; i < C2V(gCount_drones); i++) {
        tDrone_spec* drone = &C2V(gDrone_specs)[i];
        tDrone_form* form = drone->form;

        NewDroneState(drone, 1);
        if (form->type == kDroneType_plane && !C2V(gShow_drone_paths) && drone->field_0x44 == 0 && ((form->flags & 0x4) != 0 || C2V(gCount_active_drones) < 12))  {
            int collision_free;

            if (C2V(gTime_stamp_for_this_munging) > drone->last_collide_check + 1000) {
                drone->last_collide_check = C2V(gTime_stamp_for_this_munging);
                InitDroneCollisionInfo(drone);

                collision_free = !DroneHasCollided(drone);
            } else {
                collision_free = 0;
            }
            if (collision_free && MarkCollisionInfoAsProcessed(&drone->collision_info) == 0) {
                SetCollisionInfoParam(&drone->collision_info, 0, 0, 1.875);
                SetCollisionInfoParam(&drone->collision_info, 3, 1);
                SetCollisionInfoParam(&drone->collision_info, 7, 1);
                MakeDroneActive(drone);
            }
        }
    }
}
C2_HOOK_FUNCTION(0x0044f700, InitDrones)

void C2_HOOK_FASTCALL FreeThingForm(void* pData) {

    BrMemFree(pData);
}
C2_HOOK_FUNCTION(0x004c5e70, FreeThingForm)
