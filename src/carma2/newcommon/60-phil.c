/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "60-phil.h"

#include "49-physics.h"
#include "55-volume.h"

#include "rec2_macros.h"

#include <stdarg.h>

// GLOBAL: CARMA2_HW 0x0074a5f8
int gCrush_deferred;

// GLOBAL: CARMA2_HW 0x0065d004
// GLOBAL: CARMA2_SW 0x0065793c
int gPHIL_disabled = 1;

// GLOBAL: CARMA2_HW 0x0074a5f0
// GLOBAL: CARMA2_SW 0x007410b0
tPhysics_object* gPHIL_list_collision_infos;

// GLOBAL: CARMA2_HW 0x0074a5ec
// GLOBAL: CARMA2_SW 0x007410ac
tU32 gPHIL_last_physics_tick;


// GLOBAL: CARMA2_HW 0x006923e0
int gPHIL_count_list_collision_infos;

// GLOBAL: CARMA2_HW 0x0074a5e8
int gPHIL_munging_objects;

// GLOBAL: CARMA2_HW 0x006923e4
int gPHIL_object_added;

// GLOBAL: CARMA2_HW 0x006923e8
tPhysics_callbacks* gPHIL_callbacks;

/* explicit initializers keep gPhil_queued_objects and the following globals adjacent, in address order (PHILInit loop-end compare) */
// GLOBAL: CARMA2_HW 0x006923f0
tPHIL_queued_objects gPhil_queued_objects = { 0 };

// GLOBAL: CARMA2_HW 0x00692db8
tWorld_callback_active_passive_cbfn* gPHIL_original_activate_passive = NULL;

// GLOBAL: CARMA2_HW 0x00692dbc
int gPHIL_count_queued_objects_for_removal = 0;

// GLOBAL: CARMA2_HW 0x00692dc0
int gPHIL_count_queued_objects = 0;

// ScalarToFraction

// FractionToScalar

// FUNCTION: CARMA2_HW 0x0049be90
// FUNCTION: CARMA2_SW 0x00498010
void C2_HOOK_FASTCALL GetOrientationFromMatrix(tFixed_quat* pOrientation, const br_matrix34* pMatrix) {
    br_quat quaternion;
    br_scalar value;

    BrMatrix34ToQuat(&quaternion, pMatrix);
    value = quaternion.x;
    if (value > 1.f) {
        value = 1.f;
    }
    if (value < -1.f) {
        value = -1.f;
    }
    value = (value + 1.f) * .5f;
    value *= 65534.f;
    pOrientation->v[0] = (tU16)(value + .5f);
    value = quaternion.y;
    if (value > 1.f) {
        value = 1.f;
    }
    if (value < -1.f) {
        value = -1.f;
    }
    value = (value + 1.f) * .5f;
    value *= 65534.f;
    pOrientation->v[1] = (tU16)(value + .5f);
    value = quaternion.z;
    if (value > 1.f) {
        value = 1.f;
    }
    if (value < -1.f) {
        value = -1.f;
    }
    value = (value + 1.f) * .5f;
    value *= 65534.f;
    pOrientation->v[2] = (tU16)(value + .5f);
    value = quaternion.w;
    if (value > 1.f) {
        value = 1.f;
    }
    if (value < -1.f) {
        value = -1.f;
    }
    value = (value + 1.f) * .5f;
    value *= 65534.f;
    pOrientation->v[3] = (tU16)(value + .5f);
}

// GetMatrixFromOrientation

// OmegaCompScalarTo16

// OmegaComp16ToScalar

// GetOmega16FromOmega

// GetOmegaFromOmega16

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x0049c0d0
// STUB: CARMA2_SW 0x00498250
int C2_HOOK_FASTCALL GetObjectNetworkStuff(tPhysics_object* pObject, tU8* pBuffer, tU32 pSize) {

    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// GetVelocityAndPosition

int C2_HOOK_FASTCALL SizeOfObjectNetworkStuff(tPhysics_joint_type pType) {

    // FIXME: add structs and use sizeof(struct) + add static assertion making sure sizeof(struct) == {0x28,0xc,0x10}
    switch (pType & 0xf) {
    case eJoint_none:
        return 0x28;
    case eJoint_hinge:
    case eJoint_quick_hinge:
        return 0xc;
    case eJoint_universal:
    case eJoint_ball_n_socket:
        return 0x10;
    default:
        PhysicsError(9, "netphys got messed up");
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x0049c5e0
// FUNCTION: CARMA2_SW 0x00498760
int C2_HOOK_STDCALL FloatToFixedFraction(float pValue, float pMin, float pMax) {
    if (pValue > pMax) {
        pValue = pMax;
    }
    if (pValue < pMin) {
        pValue = pMin;
    }
    pValue = ((pValue - pMin) / (pMax - pMin)) * 65534.f;
    return (int)(pValue + .5f);
}

// WriteObjectNetworkStuff

// FUNCTION: CARMA2_HW 0x0049cd00
// FUNCTION: CARMA2_SW 0x00498e80
int C2_HOOK_FASTCALL GetHierarchyNetworkStuff(tPhysics_object* pObject, tU8* pBuffer, int pSize) {
    int size;
    int child_size;
    tPhysics_object* child;
    tU8* child_buffer;

    size = GetObjectNetworkStuff(pObject, pBuffer, pSize);
    if (size < 0) {
        return -1;
    }
    child = pObject->child;
    pSize -= size;
    if (child != NULL) {
        *pBuffer |= 0x80;
        for (; child != NULL; child = child->next) {
            child_buffer = pBuffer + size;
            child_size = GetHierarchyNetworkStuff(child, child_buffer, pSize);
            if (child_size < 0) {
                return -1;
            }
            pSize -= child_size;
            size += child_size;
            *child_buffer |= 0x40;
        }
        *child_buffer &= ~0x40;
    }
    return size;
}

// FindObject

// WriteHierarchyNetworkStuff

// FUNCTION: CARMA2_HW 0x0049ce60
// FUNCTION: CARMA2_SW 0x00498fe0
int C2_HOOK_FASTCALL GetHierarchyNetworkSize(tPhysics_object* pObject) {
    int size;
    tPhysics_object* child;

    size = SizeOfObjectNetworkStuff(pObject->physics_joint1 != NULL ? pObject->physics_joint1->type : eJoint_none);
    for (child = pObject->child; child != NULL; child = child->next) {
        size += GetHierarchyNetworkSize(child);
    }
    return size;
}

// FUNCTION: CARMA2_HW 0x0049cee0
int C2_HOOK_FASTCALL GetNetworkDataSize(tU8* pNet_data) {
    int size;
    tU8* next;
    tU8 more;

    switch (*pNet_data & 0xf) {
    case 0:
        size = 0x28;
        break;
    case 1:
    case 4:
        size = 0xc;
        break;
    case 2:
    case 3:
        size = 0x10;
        break;
    default:
        PhysicsError((tPhysicsError)9, "netphys got messed up");
        size = 0;
    }
    more = *pNet_data & 0x80;
    while (more != 0) {
        next = pNet_data + size;
        size += GetNetworkDataSize(next);
        more = *next & 0x40;
    }
    return size;
}

// GetSingleMatrixFromNetworkData

// FUNCTION: CARMA2_HW 0x004b5cc0
int C2_HOOK_FASTCALL PHILInit(void) {
    int i;

    C2_HOOK_BUG_ON(sizeof(gPhil_queued_objects.headers[0]) != 0x18);

    for (i = 0; i < (int)REC2_ASIZE(gPhil_queued_objects.headers); i++) {
        gPhil_queued_objects.headers[i].collision_info = NULL;
    }
    gPHIL_count_list_collision_infos = 0;
    gPHIL_count_queued_objects = 0;
    gPHIL_count_queued_objects_for_removal = 0;
    gPHIL_list_collision_infos = NULL;
    gPHIL_munging_objects = 0;
    gCrush_deferred = 0;
    gPHIL_object_added = 0;
    gPHIL_last_physics_tick = 0;
    gPHIL_mechanics_time_sync = 1;
    gPHIL_disabled = 0;
    return 0;
}

// FUNCTION: CARMA2_HW 0x004b5d20
// FUNCTION: CARMA2_SW 0x004b2440
void C2_HOOK_FASTCALL PHILDisable(void) {
    gPHIL_disabled = 1;
}

// STUB: CARMA2_HW 0x004b5ea0
// STUB: CARMA2_SW 0x004b25c0
int C2_HOOK_FASTCALL PHILRemoveObject(tPhysics_object* pObject) {

    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x004b5fd0
// FUNCTION: CARMA2_SW 0x004b26f0
tPhysics_object* C2_HOOK_FASTCALL PHILGetFirstObject(void) {
    if (gPHIL_disabled) {
        return NULL;
    } else {
        return gPHIL_list_collision_infos;
    }
}

// FUNCTION: CARMA2_HW 0x004b5ff0
// FUNCTION: CARMA2_SW 0x004b2710
tPhysics_object* C2_HOOK_FASTCALL PHILGetNextObject(tPhysics_object* pObject) {
    if (gPHIL_disabled) {
        return NULL;
    } else {
        return pObject->next;
    }
}

// FUNCTION: CARMA2_HW 0x004b6010
tU32 C2_HOOK_FASTCALL PHILReturnObjectStatus(tPhysics_object* pObject) {
    tCollision_info_owner* owner;

    if (!gPHIL_disabled) {
        if (pObject->field_0x239 == 2) {
            return ((tCollision_info_owner*)pObject->field_0x240)->field_0x08;
        }
        owner = pObject->field_0x240;
        if (owner != NULL) {
            return owner->field_0x08;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004b6040
// FUNCTION: CARMA2_SW 0x004b2760
int C2_HOOK_FASTCALL PHILMakeObjectPassive(tPhysics_object* pObject) {
    tPHIL_queued_header* info;

    if (gPHIL_disabled) {
        return 0;
    }
    if (pObject->field_0x239 == 2) {
        info = pObject->field_0x240;
        info->field_0x8 = 1;
        return 0;
    }
    info = pObject->field_0x240;
    if (info != NULL) {
        info->field_0x8 = 1;
        pObject->flags |= 0x20;
        return 0;
    } else {
        return 3;
    }
}

// FUNCTION: CARMA2_HW 0x004b6090
// FUNCTION: CARMA2_SW 0x004b27b0
int C2_HOOK_FASTCALL PHILMakeObjectActive(tPhysics_object* pObject, const br_vector3* pVelocity, const br_vector3* pOmega, int pFlag) {
    tPHIL_queued_object_info* info;

    if (gPHIL_disabled) {
        return 0;
    }
    if (pObject->field_0x239 == 2) {
        info = pObject->field_0x240;
        info->field_0x8 = 2;
        if (pVelocity != NULL) {
            info->field_0x18 = *pVelocity;
            info->flags |= 0x1;
        }
        if (pOmega != NULL) {
            info->field_0x24 = *pOmega;
            info->flags |= 0x2;
        }
        if (pFlag) {
            info->field_0x4 |= 0x2;
        } else {
            info->field_0x4 &= ~0x2;
        }
        return 0;
    }
    info = pObject->field_0x240;
    if (info != NULL) {
        info->field_0x8 = 2;
        pObject->flags &= ~0x20;
        if (pFlag) {
            info->field_0x4 |= 0x2;
        } else {
            info->field_0x4 &= ~0x2;
        }
        if (pVelocity != NULL) {
            pObject->v = *pVelocity;
        }
        if (pOmega != NULL) {
            pObject->omega = *pOmega;
        }
        ResetCarSpecialVolume(pObject);
        return 0;
    } else {
        return 3;
    }
}

// FUNCTION: CARMA2_HW 0x004b61a0
// FUNCTION: CARMA2_SW 0x004b28c0
int C2_HOOK_FASTCALL PHILSetPassiveObjectsMatrix(tPhysics_object* pObject, const br_matrix34* pMatrix) {
    tPHIL_queued_object_info* queued;
    tPHIL_queued_header* info;

    if (gPHIL_disabled) {
        return 0;
    }
    if (pObject->field_0x239 == 2) {
        queued = pObject->field_0x240;
        queued->field_0x30 = *pMatrix;
        queued->flags |= 0x4;
        return 0;
    }
    info = pObject->field_0x240;
    if (info != NULL) {
        if (info->field_0x8 != 1) {
            return 4;
        }
        info->collision_info->actor->t.t.mat = *pMatrix;
        return 0;
    } else {
        return 3;
    }
}

extern void C2_HOOK_FASTCALL PipeSinglePHILObject(tPhysics_object* pObject);
extern int C2_HOOK_FASTCALL PHILAddObject(tPhysics_object* pObject);

/* the body after the gPHIL_disabled test; inlined (its edi save appears after that test in the original); name invented */
static int C2_HOOK_FASTCALL PHILAddActiveObjectInner(tPhysics_object* pInfo, br_matrix34* pMatrix, const br_vector3* pArg3, const br_vector3* pArg4) {
    int result;

    result = PHILAddObject(pInfo);
    if (result != 0) {
        return result;
    }
    if (pMatrix != NULL && !gPHIL_disabled) {
        if (pInfo->field_0x239 == 2) {
            tQueued_object_info* object_info = pInfo->field_0x240;
            object_info->field_0x30 = *pMatrix;
            object_info->flags |= 0x4;
        } else {
            tQueued_object_info* object_info = pInfo->field_0x240;
            if (object_info != NULL) {
                if (object_info->field_0x8 != 1) {
                    return 4;
                }
                object_info->object->actor->t.t.mat = *pMatrix;
            } else {
                return 3;
            }
        }
    }
    return PHILMakeObjectActive(pInfo, pArg3, pArg4, 0);
}

// FUNCTION: CARMA2_HW 0x004b6210
int C2_HOOK_FASTCALL PHILAddActiveObject(tPhysics_object* pInfo, br_matrix34* pMatrix, const br_vector3* pArg3, const br_vector3* pArg4) {

    if (gPHIL_disabled) {
        return 0;
    }
    return PHILAddActiveObjectInner(pInfo, pMatrix, pArg3, pArg4);
}

// FUNCTION: CARMA2_HW 0x004b62b0
int C2_HOOK_FASTCALL PHILAddObjectImmediately(tPhysics_object* pObject) {
    int result;

    if (gPHIL_disabled) {
        return 0;
    }
    if (gPHIL_munging_objects) {
        gPHIL_object_added = 1;
    }
    result = PHILAddObject(pObject);
    gPHIL_object_added = 0;
    return result;
}


// FUNCTION: CARMA2_HW 0x004b62e0
int C2_HOOK_FASTCALL PHILAddActiveObjectImmediately(tPhysics_object* pInfo, br_matrix34* pMatrix, const br_vector3* pArg3, const br_vector3* pArg4) {
    int result;

    if (gPHIL_disabled) {
        return 0;
    }
    if (gPHIL_munging_objects) {
        gPHIL_object_added = 1;
    }
    result = PHILAddActiveObject(pInfo, pMatrix, pArg3, pArg4);
    gPHIL_object_added = 0;
    return result;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
extern int gFace_num__car;
extern void C2_HOOK_FASTCALL PositionChildren(tPhysics_object* pCollision_info);

// FUNCTION: CARMA2_HW 0x004b63b0
int C2_HOOK_CDECL PHILSetObjectProperty(tPhysics_object* pObject, int pProperty, ...) {
    va_list va;
    tPHIL_queued_header* owner;
    int value;

    if (gPHIL_disabled) {
        return 0;
    }
    if (pObject->field_0x239 != 2) {
        owner = pObject->field_0x240;
    } else {
        owner = pObject->field_0x240;
    }
    if (owner != NULL) {
    if (pProperty < 0 || pProperty >= 9) {
        return 5;
    }
    va_start(va, pProperty);
    switch (pProperty) {
    case 0:
        owner->field_0x10 = (float)va_arg(va, double);
        break;
    case 1:
        if (va_arg(va, int)) {
            owner->field_0x4 |= 1;
            PositionChildren(pObject);
        } else {
            owner->field_0x4 &= 1;
        }
        break;
    case 2:
        owner->field_0xc = (float)va_arg(va, double);
        break;
    case 3:
        value = (va_arg(va, int) != 0) & 1;
        pObject->flags = (pObject->flags & ~0x40) | (value << 6);
        if (!(pObject->flags & 0x40)) {
            pObject->box_face_ref = (tU16)((tU32)gFace_num__car - 2u);
        }
        break;
    case 4:
        value = (va_arg(va, int) != 0) & 1;
        pObject->flags = (pObject->flags & ~8) | (value << 3);
        break;
    case 5:
        value = (va_arg(va, int) != 0) & 1;
        pObject->flags = (pObject->flags & ~0x10) | (value << 4);
        break;
    case 6:
        if (va_arg(va, int)) {
            owner->field_0x4 |= 4;
        } else {
            owner->field_0x4 &= 4;
        }
        break;
    case 7:
        if (va_arg(va, int)) {
            owner->field_0x4 |= 8;
            pObject->water_d = 10000.f;
        } else {
            owner->field_0x4 &= 8;
        }
        break;
    case 8:
        owner->field_0x14 = (float)va_arg(va, double);
        break;
    }
    va_end(va);
    return 0;
    }
    return 3;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// STUB: CARMA2_HW 0x004b6530
// STUB: CARMA2_SW 0x004b2c50
float C2_HOOK_FASTCALL PHILGetObjectProperty(tPhysics_object* pObject, int pProperty) {
    NOT_IMPLEMENTED();
}

// FlushQueuedAddsAndRemoves

// FUNCTION: CARMA2_HW 0x004b6be0
void C2_HOOK_FASTCALL ChangedObjectsCallbacks(tPhysics_object* pObjects, tPhysics_callbacks* pCallbacks, tU32 pPeriod) {
    tPhysics_object* pObject;
    tCollision_info_owner* owner;
    tPhysics_object* child;
    tU32 val;

    for (pObject = pObjects; pObject != NULL; pObject = pObject->next) {
        if (!gPHIL_disabled) {
            if (pObject->field_0x239 == 2) {
                val = ((tCollision_info_owner*)pObject->field_0x240)->field_0x08;
            } else if (pObject->field_0x240 != NULL) {
                val = ((tCollision_info_owner*)pObject->field_0x240)->field_0x08;
            } else {
                val = 0;
            }
        } else {
            val = 0;
        }
        if (val != 2) {
            continue;
        }
        if (pObject->disable_move_rotate) {
            owner = pObject->field_0x240;
            if (owner == NULL) {
                continue;
            }
            if (!(owner->field_0x04 & 0x2)) {
                continue;
            }
            if (pCallbacks->active_halted != NULL && !pCallbacks->active_halted(pObject)) {
                continue;
            }
            if (gPHIL_disabled) {
                continue;
            }
            if (pObject->field_0x239 == 2) {
                ((tCollision_info_owner*)pObject->field_0x240)->field_0x08 = 1;
            } else {
                owner = pObject->field_0x240;
                if (owner == NULL) {
                    continue;
                }
                owner->field_0x08 = 1;
                pObject->flags |= 0x20;
            }
        } else {
            if (pCallbacks->changed_objects != NULL) {
                pCallbacks->changed_objects(pObject, pPeriod);
            }
            owner = pObject->field_0x240;
            if (owner == NULL) {
                continue;
            }
            if (!(owner->field_0x04 & 0x4)) {
                continue;
            }
            for (child = pObject; child != NULL; child = child->child) {
                PipeSinglePHILObject(child);
            }
        }
    }
}

// ProcessDrag2

// ProcessDrag

// FUNCTION: CARMA2_HW 0x004b6e90
void C2_HOOK_FASTCALL MarkObjectAndChildrenAsPassive(tPhysics_object* pObject) {
    tPhysics_object* child;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, field_0xf0, 0xf0);

    pObject->field_0xf0 = 2;
    for (child = pObject->child; child != NULL; child = child->next) {
        MarkObjectAndChildrenAsPassive(child);
    }
}

// SetStandardGravity

// ProcessGravity

// LevelOutOnSurface

// PHILMungeObjects

// PHILActivatePassive

// STUB: CARMA2_HW 0x004b5d40
int C2_HOOK_FASTCALL PHILAddObject(tPhysics_object* pObject) { NOT_IMPLEMENTED(); }
