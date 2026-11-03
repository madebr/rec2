/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "55-volume.h"

#include "00-car.h"
#include "08-loading1.h"
#include "34-finteray.h"
#include "41-utility.h"
#include "69-sound.h"
#include "70-packfile.h"
#include "globvars.h"

#include "c2_string.h"

#include "rec2_macros.h"

// GLOBAL: CARMA2_HW 0x00761b80
// GLOBAL: CARMA2_SW 0x007580e0
tSpecial_volume gDefault_default_water_spec_vol;

// GLOBAL: CARMA2_HW 0x00761f44
// GLOBAL: CARMA2_SW 0x007584a4
tSpecial_volume* gDefault_water_spec_vol_real;

// GLOBAL: CARMA2_HW 0x00660268
// GLOBAL: CARMA2_SW 0x0065ab48
const char* gSoundType_choices[2] = {
    "SATURATED",
    "SCATTERED"
};

// FUNCTION: CARMA2_HW 0x004ff110
tSpecial_volume* C2_HOOK_FASTCALL GetDefaultSpecialVolumeForWater(void) {
    return gDefault_water_spec_vol_real;
}

// FindSpecialVolume

// FUNCTION: CARMA2_HW 0x004ff410
void C2_HOOK_FASTCALL MungeSpecialVolume(tPhysics_object* pObject) {
    extern tSpecial_volume* C2_HOOK_FASTCALL FindSpecialVolume(br_vector3* pP, tSpecial_volume* pLast_vol, int pArg3);
    extern tS3_sound_tag C2_HOOK_FASTCALL DRS3StartSound(tS3_outlet* pOutlet, tS3_sound_id pSound);
    extern int C2_HOOK_FASTCALL IsCarSmoking(tCar_spec* pCar);
    extern void C2_HOOK_FASTCALL StopCarSmoking(tCar_spec* pCar);
    extern tS3_outlet* gCar_outlet;
    tSpecial_volume* new_vol;
    tCar_spec* sv;

    new_vol = FindSpecialVolume(&pObject->pos, pObject->last_special_volume, 0);

    sv = NULL;
    if (pObject != NULL && pObject->owner != NULL && pObject->flags_0x238 == 1) {
        sv = (tCar_spec*)pObject->owner;
    } else {
        sv = NULL;
    }

    if (pObject->auto_special_volume != NULL) {
        if (new_vol == NULL || new_vol->gravity_multiplier == 1.0f) {
            if (pObject->water_d == 10000.0f && pObject->water_depth_factor != 1.0f) {
                pObject->auto_special_volume = NULL;
            } else {
                new_vol = pObject->auto_special_volume;
            }
        }
    }

    if (new_vol != pObject->last_special_volume && sv != NULL && sv->driver == eDriver_local_human) {
        if (pObject->last_special_volume != NULL && pObject->last_special_volume->exit_noise >= 0
                && (new_vol == NULL || new_vol->exit_noise != pObject->last_special_volume->exit_noise)) {
            DRS3StartSound(gCar_outlet, pObject->last_special_volume->exit_noise);
        }
        if (new_vol != NULL && new_vol->entry_noise >= 0
                && (pObject->last_special_volume == NULL || pObject->last_special_volume->entry_noise != new_vol->entry_noise)) {
            DRS3StartSound(gCar_outlet, new_vol->entry_noise);
        }
    }

    pObject->last_special_volume = new_vol;

    if (sv != NULL && new_vol != NULL) {
        br_scalar gravity_multiplier = new_vol->gravity_multiplier;
        if (gravity_multiplier < 1.0f) {
            if (IsCarSmoking(sv) != 0) {
                StopCarSmoking(sv);
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004ff530
// FUNCTION: CARMA2_SW 0x004fae10
void C2_HOOK_FASTCALL ResetCarSpecialVolume(tPhysics_object* pObject) {
    br_vector3 direction;
    br_vector3 normal;
    br_scalar distance;
    br_material* material;
    tSpecial_volume* volume;
    char* mat_id;

    volume = NULL;
    BrVector3Set(&direction, 0.f, 200.f, 0.f);
    DisablePlingMaterials();
    FindFace(&pObject->actor->t.t.translate.t, &direction, &normal, &distance, &material);
    EnablePlingMaterials();
    if (distance < 100.f && material != NULL) {
        mat_id = material->identifier;
        if (mat_id != NULL) {
            if (strlen(mat_id) != 0 && (mat_id[0] == '!' || mat_id[0] == '#')) {
                volume = GetDefaultSpecialVolumeForWater();
            }
        }
    }
    pObject->auto_special_volume = volume;
    pObject->water_depth_factor = 1.f;
}

// FUNCTION: CARMA2_HW 0x004ff5d0
void C2_HOOK_FASTCALL TestAutoSpecialVolume(tPhysics_object* pObject) {
    br_scalar t1;
    br_scalar t2;
    tSpecial_volume* def_vol;
    br_vector3 edge;
    br_vector3 low;
    br_vector3 high;
    br_vector3 dir;
    br_scalar d;
    br_scalar d_neg;
    br_scalar d_pos;
    int k;

    d = 0.0f;
    for (k = 0; k < 3; k++) {
        d += BR_MAC3(pObject->transform_matrix.m[k][1], pObject->water_normal.v[1],
                         pObject->transform_matrix.m[k][0], pObject->water_normal.v[0],
                         pObject->transform_matrix.m[k][2], pObject->water_normal.v[2]) * pObject->bb2.min.v[k];
    }
    d += BR_MAC3(pObject->transform_matrix.m[3][0], pObject->water_normal.v[0],
                 pObject->transform_matrix.m[3][1], pObject->water_normal.v[1],
                 pObject->transform_matrix.m[3][2], pObject->water_normal.v[2]);
    d_pos = d;
    d_neg = d;
    for (k = 0; k < 3; k++) {
        t1 = BR_MAC3(pObject->transform_matrix.m[k][1], pObject->water_normal.v[1],
                     pObject->transform_matrix.m[k][2], pObject->water_normal.v[2],
                     pObject->transform_matrix.m[k][0], pObject->water_normal.v[0]) * (pObject->bb2.max.v[k] - pObject->bb2.min.v[k]);
        if (t1 < 0.0f) {
            d_neg += t1;
        } else {
            d_pos += t1;
        }
    }
    if (d_neg >= pObject->water_d) {
        pObject->auto_special_volume = NULL;
        pObject->water_depth_factor = 1.0f;
        return;
    }
    if (d_pos - 0.001 > pObject->water_d) {
        pObject->water_depth_factor = (pObject->water_d - d_neg) / (d_pos - d_neg);
    } else {
        pObject->water_depth_factor = 1.0f;
    }
    if (pObject->auto_special_volume != NULL) {
        return;
    }
    def_vol = gDefault_water_spec_vol_real;
    if (def_vol != NULL) {
        BrMatrix34ApplyP(&low, &pObject->bb2.min, &pObject->transform_matrix);
        high.v[0] = low.v[0];
        high.v[1] = low.v[1];
        high.v[2] = low.v[2];
        for (k = 0; k < 3; k++) {
            edge.v[0] = (pObject->bb2.max.v[k] - pObject->bb2.min.v[k]) * pObject->transform_matrix.m[k][0];
            edge.v[2] = (pObject->bb2.max.v[k] - pObject->bb2.min.v[k]) * pObject->transform_matrix.m[k][2];
            edge.v[1] = (pObject->bb2.max.v[k] - pObject->bb2.min.v[k]) * pObject->transform_matrix.m[k][1];
            if (BrVector3Dot(&pObject->water_normal, &edge) > 0.0f) {
                high.v[0] += edge.v[0];
                high.v[1] += edge.v[1];
                high.v[2] += edge.v[2];
            } else {
                low.v[0] += edge.v[0];
                low.v[1] += edge.v[1];
                low.v[2] += edge.v[2];
            }
        }
        dir.v[0] = low.v[0] - high.v[0];
        dir.v[1] = low.v[1] - high.v[1];
        dir.v[2] = low.v[2] - high.v[2];
        DisablePlingMaterials();
        RayCastInBox(&high, &dir, &edge, &t1, pObject);
        EnablePlingMaterials();
        RayCastInBox(&high, &dir, &edge, &t2, pObject);
        if (t1 < t2) {
            pObject->auto_special_volume = def_vol;
            return;
        }
    }
    pObject->water_depth_factor = 1.0f;
    pObject->auto_special_volume = NULL;
}

// FUNCTION: CARMA2_HW 0x004ff8d0
// FUNCTION: CARMA2_SW 0x004fb1b0
void C2_HOOK_FASTCALL ParseSpecialVolume(FILE* pF, tSpecial_volume* pSpec, char* pScreen_name_str, int soundfx) {
    char s[256];

    /* gravity multiplier */
    pSpec->gravity_multiplier = GetAFloat(pF);
    if (pSpec->gravity_multiplier < 0 && !gRussels_fannies) {
        pSpec->gravity_multiplier = 1.f;
    }
    /* viscosity multiplier */
    pSpec->viscosity_multiplier = GetAFloat(pF);
    /* Car damage per millisecond */
    pSpec->car_damage_per_ms = GetAScalar(pF);
    /* Pedestrian damage per millisecond */
    pSpec->ped_damage_per_ms = GetAScalar(pF);
    /* camera effect index */
    pSpec->camera_special_effect_index = GetAnInt(pF);
    /* Sky colour */
    pSpec->sky_col = GetAnInt(pF);

    /* Windscreen material to use */
    GetAString(pF, s);
    if (pScreen_name_str != NULL) {
        strcpy(pScreen_name_str, s);
    } else {
        pSpec->screen_pixelmap = BrMapFind(s);
    }

    /* Sound ID of entry noise */
    pSpec->entry_noise = GetAnInt(pF);
    /* Sound ID of exit noise */
    pSpec->exit_noise = GetAnInt(pF);
    /* Engine noise index */
    pSpec->engine_noise_index = GetAnInt(pF);
    /* material index */
    pSpec->material_modifier_index = GetAnInt(pF);
    if (soundfx) {
        pSpec->soundfx_type = GetALineAndInterpretCommand(pF, gSoundType_choices, REC2_ASIZE(gSoundType_choices));
        if (pSpec->soundfx_type != kSoundFx_None) {
            ReadSoundSpec(pF, &pSpec->soundfx_data);
        }
    } else {
        pSpec->soundfx_type = kSoundFx_None;
    }
}

// FUNCTION: CARMA2_HW 0x004ffa20
// FUNCTION: CARMA2_SW 0x004fb300
void C2_HOOK_FASTCALL SaveSpecialVolumes(void) {
    tPath_name the_path;
    FILE* f;
    int i;
    tSpecial_volume* v;
    const char* screen_name;

    PathCat(the_path, gApplication_path, "SPECSAVE.TXT");
    f = DRfopen(the_path, "wt");
    if (f != NULL) {
        fprintf(f, "// SPECIAL EFFECTS VOLUMES\n\n");
        fprintf(f, "%d\t\t\t\t// # special effects volumes\n\n", gProgram_state.special_volume_count);
        for (i = 0, v = gProgram_state.special_volumes; i < gProgram_state.special_volume_count; i++, v++) {
            switch (v->boundary_type) {
            case eFx_boundary_box:
                fprintf(f, "BOX\n");
                fprintf(f, "%.3f, %.3f, %.3f\n", v->boundary.box.mat.m[0][0], v->boundary.box.mat.m[0][1], v->boundary.box.mat.m[0][2]);
                fprintf(f, "%.3f, %.3f, %.3f\n", v->boundary.box.mat.m[1][0], v->boundary.box.mat.m[1][1], v->boundary.box.mat.m[1][2]);
                fprintf(f, "%.3f, %.3f, %.3f\n", v->boundary.box.mat.m[2][0], v->boundary.box.mat.m[2][1], v->boundary.box.mat.m[2][2]);
                fprintf(f, "%.3f, %.3f, %.3f\n", v->boundary.box.mat.m[3][0], v->boundary.box.mat.m[3][1], v->boundary.box.mat.m[3][2]);
                break;
            case eFx_boundary_plane:
                fprintf(f, "%.3f, %.3f, %.3f, %.3f\n", v->boundary.plane.v[0], v->boundary.plane.v[1], v->boundary.plane.v[2], v->boundary.plane.v[3]);
                break;
            case eFx_boundary_default:
                fprintf(f, "DEFAULT\n");
                break;
            default:
                break;
            }
            fprintf(f, "%.2f\t\t\t\t// gravity multiplier\n", v->gravity_multiplier);
            fprintf(f, "%.2f\t\t\t\t// viscosity multiplier\n", v->viscosity_multiplier);
            fprintf(f, "%.2f\t\t\t\t// Car damage per millisecond\n", v->car_damage_per_ms);
            fprintf(f, "%.2f\t\t\t\t// Pedestrian damage per millisecond\n", v->ped_damage_per_ms);
            fprintf(f, "%d\t\t\t\t\t// camera effect index\n", v->camera_special_effect_index);
            fprintf(f, "%d\t\t\t\t\t// sky colour\n", v->sky_col);
            if (v->screen_pixelmap != NULL) {
                screen_name = v->screen_pixelmap->identifier;
            } else {
                screen_name = "NONE";
            }
            fprintf(f, "%s\t\t\t\t// Windscreen texture to use\n", screen_name);
            fprintf(f, "%d\t\t\t\t\t// Sound ID of entry noise\n", v->entry_noise);
            fprintf(f, "%d\t\t\t\t\t// Sound ID of exit noise\n", v->exit_noise);
            fprintf(f, "%d\t\t\t\t\t// Engine noise index\n", v->engine_noise_index);
            fprintf(f, "%d\t\t\t\t\t// material index\n", v->material_modifier_index);
            if (v->boundary_type == eFx_boundary_box || v->boundary_type == eFx_boundary_plane) {
                if (v->soundfx_type == kSoundFx_None) {
                    fprintf(f, "NONE\t\t\t\t\t// sound type\n");
                } else {
                    fprintf(f, "%s\t\t\t\t\t// sound type\n", gSoundType_choices[v->soundfx_type]);
                    WriteOutSoundSpec(f, &v->soundfx_data);
                }
            }
            fprintf(f, "\n");
        }
        PFfclose(f);
    }
}

// FUNCTION: CARMA2_HW 0x004ffd20
// FUNCTION: CARMA2_SW 0x004fb600
void C2_HOOK_FASTCALL FindInverseAndWorldBox(tSpecial_volume* pSpec) {
    br_bounds bnds;

    BrVector3Set(&bnds.min, -1.f, -1.f, -1.f);
    BrVector3Set(&bnds.max, 1.f, 1.f, 1.f);
    GetNewBoundingBox(&pSpec->boundary.box.bounds, &bnds, &pSpec->boundary.box.mat);
    BrMatrix34Inverse(&pSpec->boundary.box.inv_mat, &pSpec->boundary.box.mat);
}

// ReadSpecialVolumes

// STUB: CARMA2_HW 0x004ff120
tSpecial_volume* C2_HOOK_FASTCALL FindSpecialVolume(br_vector3* pP, tSpecial_volume* pLast_vol, int pArg3) {
    NOT_IMPLEMENTED();
    return 0;
}
