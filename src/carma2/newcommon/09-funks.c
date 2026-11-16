/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "09-funks.h"

#include "16-graphics1.h"
#include "17-world2.h"
#include "23-piping.h"
#include "36-trig.h"
#include "41-utility.h"
#include "52-errors.h"
#include "globvars.h"
#include "rec2_macros.h"

// GLOBAL: CARMA2_HW 0x0068b840
// GLOBAL: CARMA2_SW 0x00685da0
br_scalar gSight_distance_squared;

// GLOBAL: CARMA2_HW 0x0068b84c
// GLOBAL: CARMA2_SW 0x00685dac
tFunkotronic_spec* gFunkotronics_array;

// GLOBAL: CARMA2_HW 0x0068b850
// GLOBAL: CARMA2_SW 0x00685db0
tGroovidelic_spec* gGroovidelics_array;

// GLOBAL: CARMA2_HW 0x0068b848
// GLOBAL: CARMA2_SW 0x00685da8
int gGroovidelics_array_size;

// GLOBAL: CARMA2_HW 0x0068b838
// GLOBAL: CARMA2_SW 0x00685d98
tU32 gPrevious_groove_times[2];

// GLOBAL: CARMA2_HW 0x00688b30
tGroove_funk_binding gGroove_funk_bindings[1440];

// GLOBAL: CARMA2_HW 0x0068b844
// GLOBAL: CARMA2_SW 0x00685da4
int gFunkotronics_array_size;

inline int C2_HOOK_FASTCALL CalculateTextureBitsValue(tFunk_texturebits* pTexturebits) {
    int i;
    int value;

    value = 0;
    for (i = 0; i < pTexturebits->count; i++) {
        if (pTexturebits->car->field_0x18cc & (1 << pTexturebits->bits[i])) {
            value += 1 << i;
        }
    }
    return value;
}

inline float C2_HOOK_FASTCALL MapSawToTriangle(float pNumber) {

    if (pNumber < 0.5) {
        return pNumber * 4.0f - 1.0f;
    } else {
        return 3.0f - pNumber * 4.0f;
    }
}

// FUNCTION: CARMA2_HW 0x00474880
// FUNCTION: CARMA2_SW 0x00473de0
void C2_HOOK_STDCALL SetSightDistance(br_scalar pYon) {

    gSight_distance_squared = (br_scalar)REC2_SQR(pYon * 1.02);
}

// AddFunkGrooveBinding

// FUNCTION: CARMA2_HW 0x004748a0
void C2_HOOK_FASTCALL ControlBoundFunkGroove(int pSlot_number, float pValue) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGroove_funk_binding, field_0x4, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tGroove_funk_binding, field_0x4, 0x1);
    C2_HOOK_BUG_ON(sizeof(tGroove_funk_binding) != 0x8);
    C2_HOOK_BUG_ON(REC2_ASIZE(gGroove_funk_bindings) != 1440);

    if (pSlot_number < 0) {
        return;
    }
    if (pSlot_number >= (int)REC2_ASIZE(gGroove_funk_bindings)) {
        FatalError(kFatalError_UsedRefNumOfControlledGroovidelicFunkotronicIsOutOfRange);
    }
    if (!gGroove_funk_bindings[pSlot_number].field_0x4) {
        *gGroove_funk_bindings[pSlot_number].v = pValue;
    }
}

// ControlBoundFunkGroovePlus

// ShiftBoundGrooveFunks

// AddNewFunkotronic

// STUB: CARMA2_HW 0x00474950
// STUB: CARMA2_SW 0x00473eb0
void C2_HOOK_FASTCALL DisposeFunkotronics(int pOwner) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004761e0
// FUNCTION: CARMA2_SW 0x00475740
br_uint_32 C2_HOOK_FASTCALL CalcProximities(br_actor* pActor, br_material* pMat, void* pData) {
    tFunkotronic_spec* the_funk;
    br_face* the_face;
    int i;

    (void)pMat;
    the_funk = pData;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, material, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tFunkotronic_spec, proximity_count, 0x150);

    if (pActor->model != NULL) {
        for (i = 0, the_face = pActor->model->faces; i < pActor->model->nfaces; i++, the_face++) {
            if (the_face->material == the_funk->material) {
                the_funk->proximity_count++;
            }
        }
    }
    return 0;
}

// AddProximities

// TryThisFunkLink

// FindSmashableMaterial

// AddFunkotronics

// FUNCTION: CARMA2_HW 0x00476430
// FUNCTION: CARMA2_SW 0x00475990
void C2_HOOK_FASTCALL DisposeGroovidelics(int pOwner) {
    tGroovidelic_spec* the_groove;
    int i;

    if (gGroovidelics_array != NULL) {
        for (i = 0, the_groove = gGroovidelics_array; i < gGroovidelics_array_size; i++, the_groove++) {
            PossibleService();
            if (the_groove->owner == pOwner) {
                the_groove->owner = -999;
            }
        }
    }
}

// AddNewGroovidelic

// AddGroovidelics

// FUNCTION: CARMA2_HW 0x00477160
// FUNCTION: CARMA2_SW 0x004766c0
void C2_HOOK_FASTCALL KillGroovadelic(int pOwner) {
    int i;
    tGroovidelic_spec* the_groove;

    if (gGroovidelics_array != NULL) {
        for (i = 0, the_groove = gGroovidelics_array; i < gGroovidelics_array_size; i++) {
            if (the_groove[i].owner == pOwner
                && the_groove[i].path_mode != eMove_controlled
                && the_groove[i].path_mode != eMove_absolute
                && the_groove[i].object_mode != eMove_controlled
                && the_groove[i].object_mode != eMove_absolute) {
                the_groove[i].owner = -999;
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x004771c0
// FUNCTION: CARMA2_SW 0x00476720
void C2_HOOK_FASTCALL KillFunkotronic(int pOwner) {
    int i;
    tFunkotronic_spec* the_funk;

    if (gFunkotronics_array != NULL) {
        for (i = 0, the_funk = gFunkotronics_array; i < gFunkotronics_array_size; i++) {
            if (the_funk[i].owner == pOwner
                && the_funk[i].matrix_mode != eMove_controlled
                && the_funk[i].matrix_mode != eMove_absolute
                && the_funk[i].lighting_animation_type != eMove_controlled
                && the_funk[i].lighting_animation_type != eMove_absolute
                && ((the_funk[i].texture_animation_data.frames_info.mode != eMove_controlled
                        && the_funk[i].texture_animation_data.frames_info.mode != eMove_texturebits)
                    || the_funk[i].texture_animation_type != eTexture_animation_frames)) {
                the_funk[i].owner = -999;
            }
        }
    }
}

// NormaliseDegreeAngle

// FunkThoseTronics

#define SAW(T, PERIOD) fmod((T), PERIOD) / (PERIOD)

#define GROOVE_PATH_SIN_COS(TRIG, TIME, PERIOD, AMPLITUDE) \
    ((PERIOD) != 0.0f \
            ? (br_scalar)(TRIG(BrAngleToRadian(BrDegreeToAngle( \
                              SAW(TIME, PERIOD) * 360.0f))) \
                  * (AMPLITUDE)) \
            : 0.0f)

#define GROOVE_PATH_FLOAT(MODE, TIME, PERIOD, TEXTUREBITS, AMPLITUDE) \
    ((MODE) == eMove_continuous \
            ? ((PERIOD) != 0.0f \
                      ? SAW(TIME, PERIOD) * (AMPLITUDE) \
                      : 0.0) \
            : ((MODE) == eMove_controlled \
                      ? PERIOD * AMPLITUDE \
                      : ((MODE) == eMove_absolute \
                                ? (PERIOD) \
                                : ((MODE) == eMove_linear \
                                          ? ((PERIOD) != 0.0f \
                                                    ? MapSawToTriangle((float)(SAW(TIME, PERIOD))) \
                                                        * (AMPLITUDE) \
                                                    : 0.0f) \
                                          : ((MODE) == eMove_flash \
                                                    ? (fmod((TIME), (PERIOD)) * 2.0 > (PERIOD) \
                                                              ? -(AMPLITUDE) \
                                                              : (AMPLITUDE)) \
                                                    : ((MODE) == eMove_texturebits \
                                                              ? CalculateTextureBitsValue(TEXTUREBITS) \
                                                              : GROOVE_PATH_SIN_COS(sinf, TIME, PERIOD, AMPLITUDE)))))))

// FUNCTION: CARMA2_HW 0x00478e30
// FUNCTION: CARMA2_SW 0x00478390
void C2_HOOK_FASTCALL PathGrooveBastard(tGroovidelic_spec* pGroove, tU32 pTime, br_matrix34* pMat, int pInterrupt_it) {
    br_scalar pos;

    switch (pGroove->path_type) {
    case eGroove_path_straight:
        if (pGroove->path_data.straight_info.x_delta != 0.0f) {
            pos = pGroove->path_data.straight_info.centre.v[0]
                + (br_scalar)(GROOVE_PATH_FLOAT(pGroove->path_mode, pTime,
                    pGroove->path_data.straight_info.period.value, pGroove->path_data.straight_info.texture_info.data,
                    pGroove->path_data.straight_info.x_delta));
            if (pInterrupt_it) {
                pGroove->path_resumption_value = pos;
                if (pos < pMat->m[3][0]) {
                    pGroove->path_interrupt_status = eInterrupt_less_than;
                } else {
                    pGroove->path_interrupt_status = eInterrupt_greater_than;
                }
                pGroove->actor->t.t.translate.t = pGroove->path_data.straight_info.centre;
                pMat->m[3][0] = pos;
            } else if (pGroove->path_interrupt_status == eInterrupt_none) {
                pGroove->actor->t.t.translate.t = pGroove->path_data.straight_info.centre;
                pMat->m[3][0] = pos;
            } else if (pGroove->path_interrupt_status == eInterrupt_less_than) {
                if (pGroove->path_resumption_value > pos) {
                    pGroove->path_interrupt_status = eInterrupt_none;
                    pGroove->actor->t.t.translate.t = pGroove->path_data.straight_info.centre;
                    pMat->m[3][0] = pos;
                }
            } else if (pGroove->path_resumption_value < pos) {
                pGroove->path_interrupt_status = eInterrupt_none;
                pGroove->actor->t.t.translate.t = pGroove->path_data.straight_info.centre;
                pMat->m[3][0] = pos;
            }
        }

        if (pGroove->path_data.straight_info.y_delta != 0.0f) {
            pos = pGroove->path_data.straight_info.centre.v[1]
                + (br_scalar)(GROOVE_PATH_FLOAT(pGroove->path_mode, pTime,
                    pGroove->path_data.straight_info.period.value, pGroove->path_data.straight_info.texture_info.data,
                    pGroove->path_data.straight_info.y_delta));
            if (pInterrupt_it) {
                pGroove->path_resumption_value = pos;
                if (pos < pMat->m[3][1]) {
                    pGroove->path_interrupt_status = eInterrupt_less_than;
                } else {
                    pGroove->path_interrupt_status = eInterrupt_greater_than;
                }
                pGroove->actor->t.t.translate.t = pGroove->path_data.straight_info.centre;
                pMat->m[3][1] = pos;
            } else if (pGroove->path_interrupt_status == eInterrupt_none) {
                pGroove->actor->t.t.translate.t = pGroove->path_data.straight_info.centre;
                pMat->m[3][1] = pos;
            } else if (pGroove->path_interrupt_status == eInterrupt_less_than) {
                if (pGroove->path_resumption_value > pos) {
                    pGroove->path_interrupt_status = eInterrupt_none;
                    pGroove->actor->t.t.translate.t = pGroove->path_data.straight_info.centre;
                    pMat->m[3][1] = pos;
                }
            } else if (pGroove->path_resumption_value < pos) {
                pGroove->path_interrupt_status = eInterrupt_none;
                pGroove->actor->t.t.translate.t = pGroove->path_data.straight_info.centre;
                pMat->m[3][1] = pos;
            }
        }

        if (pGroove->path_data.straight_info.z_delta != 0.0f) {
            pos = pGroove->path_data.straight_info.centre.v[2]
                + (br_scalar)(GROOVE_PATH_FLOAT(pGroove->path_mode, pTime,
                    pGroove->path_data.straight_info.period.value, pGroove->path_data.straight_info.texture_info.data,
                    pGroove->path_data.straight_info.z_delta));
            if (pInterrupt_it) {
                pGroove->path_resumption_value = pos;
                if (pos < pMat->m[3][2]) {
                    pGroove->path_interrupt_status = eInterrupt_less_than;
                } else {
                    pGroove->path_interrupt_status = eInterrupt_greater_than;
                }
                pGroove->actor->t.t.translate.t = pGroove->path_data.straight_info.centre;
                pMat->m[3][2] = pos;
            } else if (pGroove->path_interrupt_status == eInterrupt_none) {
                pGroove->actor->t.t.translate.t = pGroove->path_data.straight_info.centre;
                pMat->m[3][2] = pos;
            } else if (pGroove->path_interrupt_status == eInterrupt_less_than) {
                if (pGroove->path_resumption_value > pos) {
                    pGroove->path_interrupt_status = eInterrupt_none;
                    pGroove->actor->t.t.translate.t = pGroove->path_data.straight_info.centre;
                    pMat->m[3][2] = pos;
                }
            } else if (pGroove->path_resumption_value < pos) {
                pGroove->path_interrupt_status = eInterrupt_none;
                pGroove->actor->t.t.translate.t = pGroove->path_data.straight_info.centre;
                pMat->m[3][2] = pos;
            }
        }
        pGroove->object_position = pGroove->actor->t.t.translate.t;
        break;
    case eGroove_path_circular:
        pGroove->actor->t.t.translate.t = pGroove->path_data.circular_info.centre;
        if (pGroove->path_data.circular_info.axis == eAxis_y) {
            pMat->m[3][0] = pGroove->path_data.circular_info.centre.v[0]
                + GROOVE_PATH_SIN_COS(cosf, pTime,
                    pGroove->path_data.circular_info.period.value,
                    pGroove->path_data.circular_info.radius);
        } else if (pGroove->path_data.circular_info.axis == eAxis_z) {
            pMat->m[3][0] = pGroove->path_data.circular_info.centre.v[0]
                + GROOVE_PATH_SIN_COS(sinf, pTime,
                    pGroove->path_data.circular_info.period.value,
                    pGroove->path_data.circular_info.radius);
        }

        if (pGroove->path_data.circular_info.axis == eAxis_x) {
            pMat->m[3][1] = pGroove->path_data.circular_info.centre.v[1]
                + GROOVE_PATH_SIN_COS(sinf, pTime,
                    pGroove->path_data.circular_info.period.value,
                    pGroove->path_data.circular_info.radius);
        } else if (pGroove->path_data.circular_info.axis == eAxis_z) {
            pMat->m[3][1] = pGroove->path_data.circular_info.centre.v[1]
                + GROOVE_PATH_SIN_COS(cosf, pTime,
                    pGroove->path_data.circular_info.period.value,
                    pGroove->path_data.circular_info.radius);
        }

        if (pGroove->path_data.circular_info.axis == eAxis_x) {
            pMat->m[3][2] = pGroove->path_data.circular_info.centre.v[2]
                + GROOVE_PATH_SIN_COS(cosf, pTime,
                    pGroove->path_data.circular_info.period.value,
                    pGroove->path_data.circular_info.radius);
        } else if (pGroove->path_data.circular_info.axis == eAxis_y) {
            pMat->m[3][2] = pGroove->path_data.circular_info.centre.v[2]
                + GROOVE_PATH_SIN_COS(sinf, pTime,
                    pGroove->path_data.circular_info.period.value,
                    pGroove->path_data.circular_info.radius);
        }
        pGroove->object_position = pGroove->actor->t.t.translate.t;
        break;
    default:
        break;
    }
}

#define GROOVE_ANGLE(MODE, TIME, PERIOD, TEXTUREBITS) \
    BrDegreeToAngle(((MODE) == eMove_continuous \
            ? ((PERIOD) != 0.0f ? SAW(TIME, PERIOD) * 360.0 : 0.0) \
            : ((MODE) == eMove_controlled \
                      ? (PERIOD) * 360.0 \
                      : ((MODE) == eMove_absolute \
                                ? (PERIOD) \
                                : ((MODE) == eMove_linear \
                                          ? ((PERIOD) != 0.0f ? MapSawToTriangle((float)(SAW(TIME, PERIOD))) * 360.0 : 0.0) \
                                          : ((MODE) == eMove_flash \
                                                    ? (fmod((TIME), (PERIOD)) * 2.0 > (PERIOD) ? -360.0 : 360.0) \
                                                    : ((MODE) == eMove_texturebits \
                                                              ? CalculateTextureBitsValue(TEXTUREBITS) \
                                                              : ((PERIOD) != 0.0f ? sin(BrAngleToRadian(BrDegreeToAngle(SAW(TIME, PERIOD) * 360.0))) * 360.0 : 0.0))))))))

#define GROOVE_FLOAT(MODE, TIME, PERIOD, TEXTUREBITS, AMPLITUDE) \
    ((MODE) == eMove_continuous \
            ? ((PERIOD) != 0.0f ? SAW(TIME, PERIOD) * (AMPLITUDE) : 0.0) \
            : ((MODE) == eMove_controlled \
                      ? (PERIOD) * (AMPLITUDE) \
                      : ((MODE) == eMove_absolute \
                                ? (PERIOD) \
                                : ((MODE) == eMove_linear \
                                          ? ((PERIOD) != 0.0f ? MapSawToTriangle((float)(SAW(TIME, PERIOD))) * (AMPLITUDE) : 0.0f) \
                                          : ((MODE) == eMove_flash \
                                                    ? (fmod((TIME), (PERIOD)) * 2.0 > (PERIOD) ? -(AMPLITUDE) : (AMPLITUDE)) \
                                                    : ((MODE) == eMove_texturebits \
                                                              ? CalculateTextureBitsValue(TEXTUREBITS) \
                                                              : ((PERIOD) != 0.0f ? BR_SIN(BrDegreeToAngle(SAW(TIME, PERIOD) * 360.0)) * (AMPLITUDE) : 0.0f)))))))

// FUNCTION: CARMA2_HW 0x00479890
// FUNCTION: CARMA2_SW 0x00478df0
void C2_HOOK_FASTCALL ObjectGrooveBastard(tGroovidelic_spec* pGroove, tU32 pTime, br_matrix34* pMat, int pInterrupt_it) {
    int rock_it;
    br_scalar x_size;
    br_scalar y_size;
    br_scalar z_size;
    br_scalar pos;
    br_bounds* bounds;

    switch (pGroove->object_type) {
    case eGroove_object_spin:
        switch (pGroove->object_data.spin_info.axis) {
        case eAxis_x:
            DRMatrix34PostRotateX(pMat, GROOVE_ANGLE(pGroove->object_mode, pTime, pGroove->object_data.spin_info.period.value, pGroove->object_data.spin_info.texture_info.data));
            break;
        case eAxis_y:
            DRMatrix34PostRotateY(pMat, GROOVE_ANGLE(pGroove->object_mode, pTime, pGroove->object_data.spin_info.period.value, pGroove->object_data.spin_info.texture_info.data));
            break;
        case eAxis_z:
            DRMatrix34PostRotateZ(pMat, GROOVE_ANGLE(pGroove->object_mode, pTime, pGroove->object_data.spin_info.period.value, pGroove->object_data.spin_info.texture_info.data));
            break;
        }
        break;
    case eGroove_object_rock:
        rock_it = 1;
        pos = (br_scalar)(GROOVE_FLOAT(pGroove->object_mode, pTime,
            pGroove->object_data.rock_info.period.value, pGroove->object_data.rock_info.texture_info.data, pGroove->object_data.rock_info.max_angle));

        if (pInterrupt_it) {
            pGroove->object_resumption_value = pos;
            if (pGroove->object_data.rock_info.current_angle > pos) {
                pGroove->object_interrupt_status = eInterrupt_less_than;
            } else {
                pGroove->object_interrupt_status = eInterrupt_greater_than;
            }
        } else if (pGroove->object_interrupt_status == eInterrupt_less_than) {
            if (pGroove->object_resumption_value > pos && !gAction_replay_mode) {
                pGroove->object_interrupt_status = eInterrupt_none;
            } else {
                rock_it = 0;
            }
        } else if (pGroove->object_interrupt_status == eInterrupt_greater_than) {
            if (pGroove->object_resumption_value < pos && !gAction_replay_mode) {
                pGroove->object_interrupt_status = eInterrupt_none;
            } else {
                rock_it = 0;
            }
        }
        if (rock_it) {
            pGroove->object_data.rock_info.current_angle = pos;
        }

        switch (pGroove->object_data.rock_info.axis) {
        case eAxis_x:
            DRMatrix34PostRotateX(pMat, BrDegreeToAngle(pGroove->object_data.rock_info.current_angle));
            break;
        case eAxis_y:
            DRMatrix34PostRotateY(pMat, BrDegreeToAngle(pGroove->object_data.rock_info.current_angle));
            break;
        case eAxis_z:
            DRMatrix34PostRotateZ(pMat, BrDegreeToAngle(pGroove->object_data.rock_info.current_angle));
            break;
        }
        break;
    case eGroove_object_throb:
        BrMatrix34PostScale(pMat,
            (br_scalar)(GROOVE_FLOAT(pGroove->object_mode, pTime,
                    pGroove->object_data.throb_info.x_period.value, pGroove->object_data.throb_info.x_texture_info.data, pGroove->object_data.throb_info.x_magnitude)
                + 1.0),
            (br_scalar)(GROOVE_FLOAT(pGroove->object_mode, pTime,
                    pGroove->object_data.throb_info.y_period.value, pGroove->object_data.throb_info.y_texture_info.data, pGroove->object_data.throb_info.y_magnitude)
                + 1.0),
            (br_scalar)(GROOVE_FLOAT(pGroove->object_mode, pTime,
                    pGroove->object_data.throb_info.z_period.value, pGroove->object_data.throb_info.z_texture_info.data, pGroove->object_data.throb_info.z_magnitude)
                + 1.0));
        /* FALLTHROUGH */
    case eGroove_object_shear:
        bounds = &pGroove->actor->model->bounds;
        x_size = bounds->max.v[0] - bounds->min.v[0];
        y_size = bounds->max.v[1] - bounds->min.v[1];
        z_size = bounds->max.v[2] - bounds->min.v[2];
        if (pGroove->object_data.shear_info.x_magnitude == 0.0f) {
            BrMatrix34PostShearX(pMat,
                (br_scalar)(GROOVE_FLOAT(pGroove->object_mode, pTime,
                    pGroove->object_data.shear_info.y_period.value, pGroove->object_data.shear_info.y_texture_info.data, pGroove->object_data.shear_info.y_magnitude)
                    / y_size),
                (br_scalar)(GROOVE_FLOAT(pGroove->object_mode, pTime,
                    pGroove->object_data.shear_info.z_period.value, pGroove->object_data.shear_info.z_texture_info.data, pGroove->object_data.shear_info.z_magnitude)
                    / z_size));
        } else if (pGroove->object_data.shear_info.y_magnitude == 0.0f) {
            BrMatrix34PostShearY(pMat,
                (br_scalar)(GROOVE_FLOAT(pGroove->object_mode, pTime,
                    pGroove->object_data.shear_info.x_period.value, pGroove->object_data.shear_info.x_texture_info.data, pGroove->object_data.shear_info.x_magnitude)
                    / x_size),
                (br_scalar)(GROOVE_FLOAT(pGroove->object_mode, pTime,
                    pGroove->object_data.shear_info.z_period.value, pGroove->object_data.shear_info.z_texture_info.data, pGroove->object_data.shear_info.z_magnitude)
                    / z_size));
        } else {
            BrMatrix34PostShearZ(pMat,
                (br_scalar)(GROOVE_FLOAT(pGroove->object_mode, pTime,
                    pGroove->object_data.shear_info.x_period.value, pGroove->object_data.shear_info.x_texture_info.data, pGroove->object_data.shear_info.x_magnitude)
                    / x_size),
                (br_scalar)(GROOVE_FLOAT(pGroove->object_mode, pTime,
                    pGroove->object_data.shear_info.y_period.value, pGroove->object_data.shear_info.y_texture_info.data, pGroove->object_data.shear_info.y_magnitude)
                    / y_size));
        }
        break;
    default:
        break;
    }
}

// FUNCTION: CARMA2_HW 0x00478c80
// FUNCTION: CARMA2_SW 0x004781e0
void C2_HOOK_FASTCALL GrooveThisDelic(tGroovidelic_spec* pGroove, tU32 pTime, int pInterrupt_it) {
    br_actor* the_actor;
    br_vector3 actor_pos;
    br_matrix34* the_mat;
    tInterrupt_status old_path_interrupt;
    tInterrupt_status old_object_interrupt;

    old_path_interrupt = pGroove->path_interrupt_status;
    old_object_interrupt = pGroove->object_interrupt_status;
    the_actor = pGroove->actor;
    pGroove->done_this_frame = 1;
    CalcActorGlobalPos(&actor_pos, the_actor);
    if (PointOutOfSight(&actor_pos, pGroove->mode == eGroove_mode_distance ? gYon_squared : 36.f)) {
        return;
    }

    the_mat = &the_actor->t.t.mat;
    if ((!gAction_replay_mode
            || !ARReplayIsReallyPaused()
            || pGroove->path_mode == eMove_controlled
            || pGroove->path_mode == eMove_absolute)
        && !(pGroove->block_flags & 0x1)) {
        PathGrooveBastard(pGroove, pTime, the_mat, pInterrupt_it);
    }
    if ((pGroove->object_type != eGroove_object_none || pGroove->lollipop_mode != eLollipop_none)
        && (!gAction_replay_mode
            || !ARReplayIsReallyPaused()
            || pGroove->object_mode == eMove_controlled
            || pGroove->object_mode == eMove_absolute)
        && !(pGroove->block_flags & 0x2)) {
        the_mat->m[0][0] = 1.0f;
        the_mat->m[0][1] = 0.0f;
        the_mat->m[0][2] = 0.0f;
        the_mat->m[1][0] = 0.0f;
        the_mat->m[1][1] = 1.0f;
        the_mat->m[1][2] = 0.0f;
        the_mat->m[2][0] = 0.0f;
        the_mat->m[2][1] = 0.0f;
        the_mat->m[2][2] = 1.0f;
        the_mat->m[3][0] = -pGroove->object_centre.v[0];
        the_mat->m[3][1] = -pGroove->object_centre.v[1];
        the_mat->m[3][2] = -pGroove->object_centre.v[2];
        ObjectGrooveBastard(pGroove, pTime, the_mat, pInterrupt_it);
        the_mat->m[3][0] = pGroove->object_position.v[0] + the_mat->m[3][0] + pGroove->object_centre.v[0];
        the_mat->m[3][1] = pGroove->object_position.v[1] + the_mat->m[3][1] + pGroove->object_centre.v[1];
        the_mat->m[3][2] = pGroove->object_position.v[2] + the_mat->m[3][2] + pGroove->object_centre.v[2];
        if (pGroove->lollipop_mode != eLollipop_none) {
            LollipopizeActor(pGroove->actor, &gCamera_to_world, pGroove->lollipop_mode);
        }
    }
    if (old_path_interrupt != pGroove->path_interrupt_status || old_object_interrupt != pGroove->object_interrupt_status) {
        PipeSingleGrooveStop(pGroove - gGroovidelics_array, the_mat,
            pGroove->path_interrupt_status, pGroove->object_interrupt_status,
            pGroove->path_resumption_value, pGroove->object_resumption_value);
    }
}

// FUNCTION: CARMA2_HW 0x00478c00
// FUNCTION: CARMA2_SW 0x00478160
void C2_HOOK_FASTCALL GrooveThoseDelics(void) {
    tGroovidelic_spec* the_groove;
    float f_the_time;
    int i;

    if (gGroovidelics_array != NULL) {
        f_the_time = (float)GetTotalTime();
        gPrevious_groove_times[1] = gPrevious_groove_times[0];
        gPrevious_groove_times[0] = (tU32)f_the_time;
        for (i = 0, the_groove = gGroovidelics_array; i < gGroovidelics_array_size; i++, the_groove++) {
            if (the_groove->owner != -999 && !the_groove->done_this_frame) {
                GrooveThisDelic(the_groove, (tU32)f_the_time, 0);
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x0047b0e0
// FUNCTION: CARMA2_SW 0x0047a640
void C2_HOOK_FASTCALL StopGroovidelic(br_actor* pActor) {
    tGroovidelic_spec* the_groove;
    int i;

    for (i = 0, the_groove = gGroovidelics_array; i < gGroovidelics_array_size; i++, the_groove++) {
        if (the_groove->actor == pActor) {
            if (the_groove->path_interrupt_status == eInterrupt_none
                && the_groove->object_interrupt_status == eInterrupt_none) {
                GrooveThisDelic(the_groove, gPrevious_groove_times[1], 1);
            }
            return;
        }
    }
}

// FUNCTION: CARMA2_HW 0x0047b130
// FUNCTION: CARMA2_SW 0x0047a690
void C2_HOOK_FASTCALL DisableGroovidelic(br_actor* pActor, tU32 pBlock_flags) {
    tGroovidelic_spec* the_groove;
    int i;

    the_groove = gGroovidelics_array;
    for (i = 0; i < gGroovidelics_array_size; i++, the_groove++) {
        if (the_groove->actor == pActor) {
            the_groove->block_flags |= pBlock_flags;
            return;
        }
    }
}

// FUNCTION: CARMA2_HW 0x0047b160
// FUNCTION: CARMA2_SW 0x0047a6c0
void C2_HOOK_FASTCALL EnableGroovidelic(br_actor* pActor, tU32 pBlock_flags) {
    tGroovidelic_spec* the_groove;
    int i;

    the_groove = gGroovidelics_array;
    for (i = 0; i < gGroovidelics_array_size; i++, the_groove++) {
        if (the_groove->actor == pActor) {
            the_groove->block_flags &= ~pBlock_flags;
            return;
        }
    }
}

// FUNCTION: CARMA2_HW 0x0047b1a0
// FUNCTION: CARMA2_SW 0x0047a700
void C2_HOOK_FASTCALL SetGrooveInterrupt(int pIndex, const br_matrix34* pMatrix, tInterrupt_status pPath_interrupt_status, tInterrupt_status pObject_interrupt_status, float pPath_resumption_value, float pObject_resumption_value) {
    tGroovidelic_spec* the_groove;

    the_groove = &gGroovidelics_array[pIndex];
    the_groove->path_interrupt_status = pPath_interrupt_status;
    the_groove->object_interrupt_status = pObject_interrupt_status;
    the_groove->path_resumption_value = pPath_resumption_value;
    the_groove->object_resumption_value = pObject_resumption_value;
    BrMatrix34Copy(&the_groove->actor->t.t.mat, pMatrix);
}

// FUNCTION: CARMA2_HW 0x0047b1e0
// FUNCTION: CARMA2_SW 0x0047a740
void C2_HOOK_FASTCALL ResetGrooveFlags(void) {
    int i;
    tGroovidelic_spec* the_groove;

    i = 0;
    if (gGroovidelics_array_size > 0) {
        the_groove = gGroovidelics_array;
        do {
            the_groove->done_this_frame = 0;
            i++;
            the_groove++;
        } while (i < gGroovidelics_array_size);
    }
}

// FUNCTION: CARMA2_HW 0x0047b210
// FUNCTION: CARMA2_SW 0x0047a770
intptr_t C2_HOOK_CDECL DelGrooveRef(br_actor* pActor, void* pArg) {
    tGroovidelic_spec* the_groove;
    int i;

    for (i = 0, the_groove = gGroovidelics_array; i < gGroovidelics_array_size; i++, the_groove++) {
        if (pActor == the_groove->actor) {
            the_groove->owner = -999;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0047b250
// FUNCTION: CARMA2_SW 0x0047a7b0
void C2_HOOK_FASTCALL DisableFunkotronic(int pIndex) {
    gFunkotronics_array[pIndex].flags |= 0x1;
    PipeSingleFunkEnable(pIndex, 0);
}

// FUNCTION: CARMA2_HW 0x0047b280
// FUNCTION: CARMA2_SW 0x0047a7e0
void C2_HOOK_FASTCALL EnableFunkotronic(int pIndex) {
    gFunkotronics_array[pIndex].flags &= ~0x1;
    PipeSingleFunkEnable(pIndex, 1);
}

// FUNCTION: CARMA2_HW 0x0047b2b0
// FUNCTION: CARMA2_SW 0x0047a810
void C2_HOOK_FASTCALL MasterDisableFunkotronic(int pIndex) {
    gFunkotronics_array[pIndex].flags |= 0x2;
}

// FUNCTION: CARMA2_HW 0x0047b2e0
// FUNCTION: CARMA2_SW 0x0047a840
void C2_HOOK_FASTCALL MasterEnableFunkotronic(int pIndex) {
    gFunkotronics_array[pIndex].flags &= ~0x2;
}

// FUNCTION: CARMA2_HW 0x0047b310
// FUNCTION: CARMA2_SW 0x0047a870
void C2_HOOK_FASTCALL AdjustFunkEnable(int pIndex, tS16 pFlags) {
    if (pFlags != 0) {
        EnableFunkotronic(pIndex);
    } else {
        DisableFunkotronic(pIndex);
    }
}

// FUNCTION: CARMA2_HW 0x0047b360
// FUNCTION: CARMA2_SW 0x0047a8c0
tGroovidelic_spec* C2_HOOK_FASTCALL ActorsGroove(br_actor* pActor) {
    tGroovidelic_spec* groove;
    int count;
    int i;

    groove = gGroovidelics_array;
    count = gGroovidelics_array_size;
    for (i = 0; i < count; i++, groove++) {
        if (groove->actor == pActor) {
            return groove;
        }
    }
    return NULL;
}
