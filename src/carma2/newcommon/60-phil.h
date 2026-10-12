#ifndef GUARD_60_PHIL_H
#define GUARD_60_PHIL_H

#include "c2_hooks.h"
#include "rec2_types.h"

/* Deferred removals, deferred adds and permanent headers overlap in the EXE. */
typedef struct {
    int count_list;
    int object_added;
    tPhysics_callbacks* callbacks;
    tPhysics_object* first_removal;
    tPHIL_queued_objects queued;
    tWorld_callback_active_passive_cbfn* original_activate_passive;
    int count_removals;
    int count_adds;
} tPHIL_registry_storage;

extern tPHIL_registry_storage gPHIL_registry;
extern int gPHIL_disabled;
extern tPhysics_object* gPHIL_list_collision_infos;
extern int gPHIL_munging_objects;
extern int gPHIL_doing_physics;
extern tU32 gPHIL_last_physics_tick;

#define gPHIL_count_list_collision_infos (gPHIL_registry.count_list)
#define gPHIL_object_added (gPHIL_registry.object_added)
#define gPHIL_callbacks (gPHIL_registry.callbacks)
#define gPhil_queued_objects (gPHIL_registry.queued)
#define gPHIL_original_activate_passive (gPHIL_registry.original_activate_passive)
#define gPHIL_count_queued_objects_for_removal (gPHIL_registry.count_removals)
#define gPHIL_count_queued_objects (gPHIL_registry.count_adds)

// ScalarToFraction

// FractionToScalar

void C2_HOOK_FASTCALL GetOrientationFromMatrix(tU16* pOrientation, const br_matrix34* pMatrix);

void C2_HOOK_FASTCALL GetMatrixFromOrientation(br_matrix34* pMatrix, const tU16* pOrientation);

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

extern int C2_HOOK_FASTCALL PHILInit(void);

extern void C2_HOOK_FASTCALL PHILDisable(void);

// PHILAddObject

extern int C2_HOOK_FASTCALL PHILRemoveObject(tPhysics_object* pObject);

extern tPhysics_object* C2_HOOK_FASTCALL PHILGetFirstObject(void);

extern tPhysics_object* C2_HOOK_FASTCALL PHILGetNextObject(tPhysics_object* pObject);

extern int C2_HOOK_FASTCALL PHILReturnObjectStatus(tPhysics_object* pObject);

extern int C2_HOOK_FASTCALL PHILMakeObjectPassive(tPhysics_object* pObject);

extern int C2_HOOK_FASTCALL PHILMakeObjectActive(tPhysics_object* pObject, const br_vector3* pVelocity, const br_vector3* pOmega, int pFlag);

extern int C2_HOOK_FASTCALL PHILSetPassiveObjectsMatrix(tPhysics_object* pObject, const br_matrix34* pMatrix);

// PHILAddActiveObject

// PHILAddObjectImmediately

// PHILAddActiveObjectImmediately

extern int C2_HOOK_CDECL PHILSetObjectProperty(tPhysics_object* pObject, int pProperty, ...);

extern double C2_HOOK_FASTCALL PHILGetObjectProperty(tPhysics_object* pObject, int pProperty);

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

#endif // GUARD_60_PHIL_H
