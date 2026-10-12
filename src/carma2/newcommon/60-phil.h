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

extern int C2_HOOK_FASTCALL PHILAddObject(tPhysics_object* object);

extern int C2_HOOK_FASTCALL PHILRemoveObject(tPhysics_object* pObject);

extern tPhysics_object* C2_HOOK_FASTCALL PHILGetFirstObject(void);

extern tPhysics_object* C2_HOOK_FASTCALL PHILGetNextObject(tPhysics_object* pObject);

extern int C2_HOOK_FASTCALL PHILReturnObjectStatus(tPhysics_object* pObject);

extern int C2_HOOK_FASTCALL PHILMakeObjectPassive(tPhysics_object* pObject);

extern int C2_HOOK_FASTCALL PHILMakeObjectActive(tPhysics_object* pObject, const br_vector3* pVelocity, const br_vector3* pOmega, int pFlag);

extern int C2_HOOK_FASTCALL PHILSetPassiveObjectsMatrix(tPhysics_object* pObject, const br_matrix34* pMatrix);

extern int C2_HOOK_FASTCALL PHILAddActiveObject(tPhysics_object* object, const br_matrix34* matrix, const br_vector3* velocity, const br_vector3* omega);

extern int C2_HOOK_FASTCALL FUN_004b62b0(tPhysics_object* object);

extern int C2_HOOK_FASTCALL FUN_004b62e0(tPhysics_object* object, const br_matrix34* matrix, const br_vector3* velocity, const br_vector3* omega);

extern int C2_HOOK_CDECL PHILSetObjectProperty(tPhysics_object* pObject, int pProperty, ...);

extern double C2_HOOK_FASTCALL PHILGetObjectProperty(tPhysics_object* pObject, int pProperty);

// FlushQueuedAddsAndRemoves

// ChangedObjectsCallbacks

// ProcessDrag

// MarkObjectAndChildrenAsPassive

// SetStandardGravity

void C2_HOOK_FASTCALL LevelOutOnSurface(tPhysics_object* object);

// PHILMungeObjects

void C2_HOOK_FASTCALL PHILActivatePassive(tPhysics_object* object, undefined4 captured_edx);

// PHILInterpolateObjects

// PHILDoPhysics

// PHILApplyPHILObject

void C2_HOOK_FASTCALL ProcessGravity(tPHIL_queued_header* header, tPhysics_object* object, br_scalar gravity);
void C2_HOOK_FASTCALL ProcessDrag2(tPHIL_queued_header* header, tPhysics_object* object, br_scalar drag, int flags, tSpecial_volume* volume);

#endif // GUARD_60_PHIL_H
