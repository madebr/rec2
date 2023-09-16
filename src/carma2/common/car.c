#include "car.h"

#include "finteray.h"
#include "world.h"

#include "brender/brender.h"
#include "brender/br_inline_funcs.h"
#include "rec2_macros.h"
#include "rec2_types.h"

#include "c2_string.h"

C2_HOOK_VARIABLE_IMPLEMENT(int, gCar_simplification_level, 0x006793d8);

void (C2_HOOK_FASTCALL * SetUpPanningCamera_original)(tCar_spec* c);
void C2_HOOK_FASTCALL SetUpPanningCamera(tCar_spec* c) {

#if defined(C2_HOOKS_ENABLED)
    SetUpPanningCamera_original(c);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004105f0, SetUpPanningCamera, SetUpPanningCamera_original)

int (C2_HOOK_FASTCALL * CollideCamera2_original)(br_vector3* car_pos, br_vector3* cam_pos, br_vector3* old_camera_pos, int manual_move, tCollision_info *collision_info);
int C2_HOOK_FASTCALL CollideCamera2(br_vector3* car_pos, br_vector3* cam_pos, br_vector3* old_camera_pos, int manual_move, tCollision_info *collision_info) {

#if defined(C2_HOOKS_ENABLED)
    return CollideCamera2_original(car_pos, cam_pos, old_camera_pos, manual_move, collision_info);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004122b0, CollideCamera2, CollideCamera2_original)

void (C2_HOOK_FASTCALL * PanningExternalCamera_original)(tCar_spec* c, tU32 pTime);
void C2_HOOK_FASTCALL PanningExternalCamera(tCar_spec* c, tU32 pTime) {

#if defined(C2_HOOKS_ENABLED)
    PanningExternalCamera_original(c, pTime);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004104b0, PanningExternalCamera, PanningExternalCamera_original)

int (C2_HOOK_FASTCALL * IncidentCam_original)(tCar_spec* c, tU32 pTime);
int C2_HOOK_FASTCALL IncidentCam(tCar_spec* c, tU32 pTime) {

#if defined(C2_HOOKS_ENABLED)
    return IncidentCam_original(c, pTime);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0040f790, IncidentCam, IncidentCam_original)

void (C2_HOOK_FASTCALL * ResetCarSpecialVolume_original)(tCollision_info* pCar);
void C2_HOOK_FASTCALL ResetCarSpecialVolume(tCollision_info* pCollision_info) {

#if 0//defined(C2_HOOKS_ENABLED)
    ResetCarSpecialVolume_original(pCollision_info);
#else
    br_vector3 cast_v;
    br_vector3 norm;
    br_scalar t;
    int id_len;
    char* mat_id;
    tSpecial_volume* new_special_volume;
    br_material* material;

    new_special_volume = NULL;
    BrVector3Set(&cast_v, 0.f, 200.f, 0.f);
    DisablePlingMaterials();
    FindFace(&pCollision_info->actor->t.t.translate.t, &cast_v, &norm, &t, &material);
    EnablePlingMaterials();
    if (t < 100.0f && material != NULL) {
        mat_id = material->identifier;
        if (mat_id != NULL) {
            id_len = c2_strlen(mat_id);
            if (id_len != 0 && (mat_id[0] == '!' || mat_id[0] == '#')) {
                new_special_volume = GetDefaultSpecialVolumeForWater();
            }
        }
    }
    pCollision_info->auto_special_volume = new_special_volume;
    pCollision_info->water_depth_factor = 1.0f;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004ff530, ResetCarSpecialVolume, ResetCarSpecialVolume_original)

void C2_HOOK_FAKE_THISCALL ControlCar4(tCar_spec* c, undefined4 pArg2, br_scalar dt) {

    REC2_THISCALL_UNUSED(pArg2);

    if (c->keys.left) {
        if (c->turn_speed < 0.f) {
            c->turn_speed = 0.f;
        }
        if (c->collision_info->velocity_car_space.v[2] > 0.f) {
            c->turn_speed += .25f * dt;
        } else if ((c->curvature >= 0.f && c->collision_info->omega.v[1] >= -.001f) || c->turn_speed != 0.f) {
            c->turn_speed += 25.f * dt * (0.05f / (5.f + WORLD_SCALE * BrVector3Length(&c->collision_info->v) )) * .25f;
        } else {
            c->turn_speed = 25.f * dt * (.05f / (5.f + WORLD_SCALE * BrVector3Length(&c->collision_info->v)));
            if (c->collision_info->omega.v[1] < -.01f) {
                c->turn_speed -= .25f * dt * c->collision_info->omega.v[1];
            }
        }
    }
    if (c->keys.right) {
        if (c->turn_speed > 0.f) {
            c->turn_speed = 0.f;
        }
        if (c->collision_info->velocity_car_space.v[2] > 0.f) {
            c->turn_speed -= .25f * dt;
        } else if ((c->curvature <= 0.f && c->collision_info->omega.v[1] <= .001f) || c->turn_speed != 0.f) {
            c->turn_speed -= 25.f * dt * (.05f / (5.f + WORLD_SCALE * BrVector3Length(&c->collision_info->v))) * .25f;
        } else {
            c->turn_speed = -25.f * dt * (.05f / (5.f + WORLD_SCALE * BrVector3Length(&c->collision_info->v)));
            if (c->collision_info->omega.v[1] < -.01f) {
                c->turn_speed -= .25f * dt * c->collision_info->omega.v[1];
            }
        }
    }
    if (!c->keys.left && !c->keys.right) {
        c->turn_speed = 0.f;
    } else if (fabsf(c->turn_speed) < fabsf(2.f * dt * c->curvature) && c->curvature * c->turn_speed < 0.f) {
        c->turn_speed = -(2.f * dt * c->curvature);
    }
    c->curvature += c->turn_speed;
    if (c->joystick.left > 0) {
        c->curvature = (float)c->joystick.left * (float)c->joystick.left / 4294967300.f * c->maxcurve;
    } else if (c->joystick.right >= 0) {
        c->curvature = -((float)c->joystick.right * (float)c->joystick.right / 4294967300.f) * c->maxcurve;
    }
    if (c->curvature > c->maxcurve) {
        c->curvature = c->maxcurve;
    }
    if (c->curvature < -c->maxcurve) {
        c->curvature = -c->maxcurve;
    }
}
C2_HOOK_FUNCTION(0x004175e0, ControlCar4)
