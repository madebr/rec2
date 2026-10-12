#include "60-phil.h"

#include "49-physics.h"
#include "55-volume.h"

#include <string.h>
#include <stddef.h>

// GLOBAL: CARMA2_HW 0x006923e0
tPHIL_registry_storage gPHIL_registry;

// GLOBAL: CARMA2_HW 0x0065d004
int gPHIL_disabled = 1;

// GLOBAL: CARMA2_HW 0x0074a5f0
tPhysics_object* gPHIL_list_collision_infos;

// GLOBAL: CARMA2_HW 0x0074a5e8
int gPHIL_munging_objects;

// GLOBAL: CARMA2_HW 0x0074a5f8
int gPHIL_doing_physics;

// GLOBAL: CARMA2_HW 0x0074a5ec
tU32 gPHIL_last_physics_tick;

/* REP MOVSD copies overlapping buffers in forward DWORD order. */
static void C2_HOOK_CDECL PhilCopyWords(void* pOutput, const void* pInput, int pCount) {
    int i;
    volatile tU32 word;

    for (i = 0; i < pCount; i++) {
        memcpy((void*)&word, (const char*)pInput + 4 * i, 4);
        memcpy((char*)pOutput + 4 * i, (const void*)&word, 4);
    }
}

// ScalarToFraction

// FractionToScalar

// GetOrientationFromMatrix

// GetMatrixFromOrientation

// OmegaCompScalarTo16

// OmegaComp16ToScalar

// GetOmega16FromOmega

// GetOmegaFromOmega16

// GetObjectNetworkStuff

// GetVelocityAndPosition

// SizeOfObjectNetworkStuff

// WriteObjectNetworkStuff

// GetHierarchyNetworkStuff

// FindObject

// WriteHierarchyNetworkStuff

// GetHierarchyNetworkSize

// GetNetworkDataSize

// GetSingleMatrixFromNetworkData

// FUNCTION: CARMA2_HW 0x004b5cc0
int C2_HOOK_FASTCALL PHILInit(void) {
    int i;

    for (i = 0; i < 100; i++) {
        gPhil_queued_objects.headers[i].collision_info = NULL;
    }
    gPHIL_count_list_collision_infos = 0;
    gPHIL_count_queued_objects = 0;
    gPHIL_count_queued_objects_for_removal = 0;
    gPHIL_list_collision_infos = NULL;
    gPHIL_munging_objects = 0;
    gPHIL_doing_physics = 0;
    gPHIL_object_added = 0;
    gPHIL_last_physics_tick = 0;
    gPHIL_mechanics_time_sync = 1;
    gPHIL_disabled = 0;
    return 0;
}

// FUNCTION: CARMA2_HW 0x004b5d20
void C2_HOOK_FASTCALL PHILDisable(void) {
    gPHIL_disabled = 1;
}

// PHILAddObject

// FUNCTION: CARMA2_HW 0x004b5ea0
int C2_HOOK_FASTCALL PHILRemoveObject(tPhysics_object* pObject) {
    int i;
    int index;
    tPHIL_queued_header* info;
    tPhysics_object* previous;

    if (gPHIL_disabled) {
        return 0;
    }
    if (pObject->field_0x239 == 2) {
        info = pObject->field_0x240;
        info->collision_info = NULL;
        pObject->field_0x240 = NULL;
        pObject->field_0x239 = 0;
        return 0;
    }
    if (gPHIL_munging_objects && !gPHIL_object_added) {
        index = gPHIL_count_queued_objects_for_removal;
        memcpy((char*)&gPHIL_registry.first_removal + (tU32)index * 4u, &pObject, 4);
        gPHIL_count_queued_objects_for_removal = (int)((tU32)index + 1u);
        return 0;
    }
    for (i = 0; i < 100; i++) {
        if (gPhil_queued_objects.headers[i].collision_info == pObject) {
            break;
        }
    }
    if (i == 100 || pObject->field_0x240 == NULL) {
        return 3;
    }
    info = pObject->field_0x240;
    info->collision_info = NULL;
    pObject->field_0x240 = NULL;
    gPHIL_count_list_collision_infos = (int)((tU32)gPHIL_count_list_collision_infos - 1u);
    if (pObject == gPHIL_list_collision_infos) {
        gPHIL_list_collision_infos = pObject->next;
        if (pObject->next != NULL) {
            pObject->next->prev = NULL;
        }
    }
    for (i = 0; i < 100; i++) {
        previous = gPhil_queued_objects.headers[i].collision_info;
        if (previous != NULL && previous->next == pObject) {
            previous->next = pObject->next;
            if (pObject->next != NULL) {
                pObject->next->prev = gPhil_queued_objects.headers[i].collision_info;
            }
            return 0;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004b5fd0
tPhysics_object* C2_HOOK_FASTCALL PHILGetFirstObject(void) {
    return gPHIL_disabled == 0 ? gPHIL_list_collision_infos : NULL;
}

// FUNCTION: CARMA2_HW 0x004b5ff0
tPhysics_object* C2_HOOK_FASTCALL PHILGetNextObject(tPhysics_object* pObject) {
    return gPHIL_disabled == 0 ? pObject->next : NULL;
}

// FUNCTION: CARMA2_HW 0x004b6010
int C2_HOOK_FASTCALL PHILReturnObjectStatus(tPhysics_object* pObject) {
    tPHIL_queued_header* info;

    if (gPHIL_disabled) {
        return 0;
    }
    info = pObject->field_0x240;
    if (pObject->field_0x239 == 2 || info != NULL) {
        return (int)info->field_0x8;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x004b6040
int C2_HOOK_FASTCALL PHILMakeObjectPassive(tPhysics_object* pObject) {
    tPHIL_queued_header* info;

    if (gPHIL_disabled) {
        return 0;
    }
    info = pObject->field_0x240;
    if (pObject->field_0x239 == 2) {
        info->field_0x8 = 1;
        return 0;
    }
    if (info == NULL) {
        return 3;
    }
    info->field_0x8 = 1;
    pObject->flags |= 0x20;
    return 0;
}

// FUNCTION: CARMA2_HW 0x004b6090
int C2_HOOK_FASTCALL PHILMakeObjectActive(tPhysics_object* pObject, const br_vector3* pVelocity, const br_vector3* pOmega, int pFlag) {
    tPHIL_queued_object_info* info;

    if (gPHIL_disabled) {
        return 0;
    }
    info = pObject->field_0x240;
    if (pObject->field_0x239 == 2) {
        info->field_0x8 = 2;
        if (pVelocity != NULL) {
            PhilCopyWords(&info->field_0x18, pVelocity, 3);
            info->flags |= 1;
        }
        if (pOmega != NULL) {
            PhilCopyWords(&info->field_0x24, pOmega, 3);
            info->flags |= 2;
        }
        if (pFlag) {
            info->field_0x4 |= 2;
        } else {
            info->field_0x4 &= ~2u;
        }
        return 0;
    }
    if (info == NULL) {
        return 3;
    }
    info->field_0x8 = 2;
    pObject->flags &= ~0x20;
    if (pFlag) {
        info->field_0x4 |= 2;
    } else {
        info->field_0x4 &= ~2u;
    }
    if (pVelocity != NULL) {
        PhilCopyWords(&pObject->v, pVelocity, 3);
    }
    if (pOmega != NULL) {
        PhilCopyWords(&pObject->omega, pOmega, 3);
    }
    ResetCarSpecialVolume(pObject);
    return 0;
}

// FUNCTION: CARMA2_HW 0x004b61a0
int C2_HOOK_FASTCALL PHILSetPassiveObjectsMatrix(tPhysics_object* pObject, const br_matrix34* pMatrix) {
    tPHIL_queued_object_info* info;

    if (gPHIL_disabled) {
        return 0;
    }
    info = pObject->field_0x240;
    if (pObject->field_0x239 == 2) {
        PhilCopyWords(&info->field_0x30, pMatrix, 12);
        info->flags |= 4;
        return 0;
    }
    if (info == NULL) {
        return 3;
    }
    if (info->field_0x8 != 1) {
        return 4;
    }
    PhilCopyWords(&info->collision_info->actor->t.t.mat, pMatrix, 12);
    return 0;
}

// PHILAddActiveObject

// PHILAddObjectImmediately

// PHILAddActiveObjectImmediately

// PHILSetObjectProperty

// FUNCTION: CARMA2_HW 0x004b6530
double C2_HOOK_FASTCALL PHILGetObjectProperty(tPhysics_object* pObject, int pProperty) {
    tPHIL_queued_header* info;

    if (gPHIL_disabled) {
        return 0.0;
    }
    info = pObject->field_0x240;
    if (info == NULL) {
        return 0.0;
    }
    switch (pProperty) {
    case 0:
        return (double)info->field_0x10;
    case 1:
        return info->field_0x4 & 1;
    case 2:
        return (double)info->field_0xc;
    case 3:
        return ((tU32)pObject->flags >> 6) & 1;
    case 4:
        return ((tU32)pObject->flags >> 3) & 1;
    case 5:
        return ((tU32)pObject->flags >> 4) & 1;
    case 6:
        return (info->field_0x4 >> 2) & 1;
    case 7:
        return (info->field_0x4 >> 3) & 1;
    case 8:
        return (double)info->field_0x14;
    default:
        return 5.0;
    }
}

// FlushQueuedAddsAndRemoves

// ChangedObjectsCallbacks

// ProcessDrag2

// ProcessDrag

// MarkObjectAndChildrenAsPassive

// SetStandardGravity

// ProcessGravity

// LevelOutOnSurface

// PHILMungeObjects

// PHILActivatePassive

// PHILInterpolateObjects

// PHILDoPhysics

// PHILApplyPHILObject

