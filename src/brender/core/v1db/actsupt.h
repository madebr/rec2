#ifndef REC2_ACTSUPT_H
#define REC2_ACTSUPT_H

#include "c2_hooks.h"

#include "brender/br_types.h"

br_uint_32 C2_HOOK_CDECL BrActorEnum(br_actor* parent, br_actor_enum_cbfn* callback, void* arg);

br_uint_32 C2_HOOK_CDECL BrActorSearchMany(br_actor* root, char* pattern, br_actor** actors, int max);

br_actor* C2_HOOK_CDECL BrActorSearch(br_actor* root, char* pattern);

void C2_HOOK_STDCALL RenumberActor(br_actor* a, int d);

br_actor* C2_HOOK_CDECL BrActorAdd(br_actor* parent, br_actor* a);

br_actor* C2_HOOK_CDECL BrActorRemove(br_actor* a);

void C2_HOOK_CDECL BrActorRelink(br_actor* parent, br_actor* a);

br_actor* C2_HOOK_CDECL BrActorAllocate(br_uint_8 type, void* type_data);

void C2_HOOK_STDCALL InternalActorFree(br_actor* a);

void C2_HOOK_CDECL BrActorFree(br_actor* a);

br_boolean C2_HOOK_STDCALL ActorToRoot(br_actor* a, br_actor* world, br_matrix34* m);

br_boolean C2_HOOK_STDCALL ActorToRootTyped(br_actor* a, br_actor* world, br_matrix34* m, br_int_32* type);

void C2_HOOK_STDCALL Matrix4PerspectiveNew(br_matrix4* mat, br_angle field_of_view, br_scalar aspect, br_scalar hither, br_scalar yon, br_scalar origin_x, br_scalar origin_y);

br_token C2_HOOK_STDCALL CameraToScreenMatrix4(br_matrix4* mat, br_actor* camera);

br_uint_16 C2_HOOK_CDECL BrActorToActorMatrix34(br_matrix34* m, br_actor* a, br_actor* b);

void C2_HOOK_CDECL BrActorToScreenMatrix4(br_matrix4* m, br_actor* a, br_actor* camera);

void C2_HOOK_CDECL BrMatrix34ApplyBounds(br_bounds* d, br_bounds* s, br_matrix34* m);

void C2_HOOK_STDCALL ActorToBounds(br_bounds* dest, br_actor* ap, br_model* model);

br_bounds* C2_HOOK_CDECL BrActorToBounds(br_bounds* b, br_actor* ap);

#endif // REC2_ACTSUPT_H
