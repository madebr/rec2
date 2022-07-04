#ifndef REC2_DBSETUP_H
#define ReC2_DBSETUP_H

#include "c2_hooks.h"

//#include "brender/br_types.h"
//
//extern br_v1db_state v1db;
//
//br_error BrV1dbBegin();
//
//br_error BrV1dbEnd();
//
//br_uint_32 updateTable(br_pixelmap* item, void* arg);
//
//br_uint_32 updateMap(br_pixelmap* item, void* arg);
//
//br_uint_32 updateMaterial(br_material* item, void* arg);
//
//br_uint_32 updateModel(br_model* item, void* arg);
//
//br_uint_32 clearTable(br_pixelmap* item, void* arg);
//
//br_uint_32 clearMap(br_pixelmap* item, void* arg);
//
//br_uint_32 clearMaterial(br_material* item, void* arg);
//
//br_uint_32 clearModel(br_model* item, void* arg);
//
//br_error BrV1dbRendererBegin(br_device_pixelmap* destination, br_renderer* renderer);
//
//br_renderer* BrV1dbRendererQuery();
//
//br_error BrV1dbRendererEnd();
//
//void BrZbBegin(br_uint_8 colour_type, br_uint_8 depth_type);
//
//void BrZsBegin(br_uint_8 colour_type, void* primitive, br_uint_32 size);
//
//void BrZbEnd();
//
//void BrZsEnd();
//
void C2_HOOK_CDECL BrV1dbBeginWrapper_Float(void);
//
//void BrV1dbEndWrapper();

#endif // REC2_DBSETUP_H
