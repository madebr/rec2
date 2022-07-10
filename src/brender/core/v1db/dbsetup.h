#ifndef REC2_DBSETUP_H
#define REC2_DBSETUP_H

#include "c2_hooks.h"

#include "brender/br_types.h"

C2_HOOK_VARIABLE_DECLARE(br_v1db_state, v1db);

br_error C2_HOOK_CDECL BrV1dbBegin(void);

br_error C2_HOOK_CDECL BrV1dbEnd(void);

br_uint_32 C2_HOOK_CDECL updateTable(br_pixelmap* item, void* arg);

br_uint_32 C2_HOOK_CDECL updateMap(br_pixelmap* item, void* arg);

br_uint_32 C2_HOOK_CDECL updateMaterial(br_material* item, void* arg);

br_uint_32 C2_HOOK_CDECL updateModel(br_model* item, void* arg);

br_uint_32 C2_HOOK_CDECL clearTable(br_pixelmap* item, void* arg);

br_uint_32 C2_HOOK_CDECL clearMap(br_pixelmap* item, void* arg);

br_uint_32 C2_HOOK_CDECL clearMaterial(br_material* item, void* arg);

br_uint_32 C2_HOOK_CDECL clearModel(br_model* item, void* arg);

br_error C2_HOOK_CDECL BrV1dbRendererBegin(br_device_pixelmap* destination, br_renderer* renderer);

br_renderer* C2_HOOK_CDECL BrV1dbRendererQuery(void);

br_error C2_HOOK_CDECL BrV1dbRendererEnd(void);

void C2_HOOK_CDECL BrZbBegin(br_uint_8 colour_type, br_uint_8 depth_type);

void C2_HOOK_CDECL BrZsBegin(br_uint_8 colour_type, void* primitive, br_uint_32 size);

void C2_HOOK_CDECL BrZbEnd(void);

void C2_HOOK_CDECL BrZsEnd(void);

void C2_HOOK_CDECL BrV1dbBeginWrapper_Float(void);

void C2_HOOK_CDECL BrV1dbEndWrapper_Float
(void);

#endif // REC2_DBSETUP_H
