#ifndef REC2_PREPMATL_H
#define REC2_PREPMATL_H

#include "brender/br_types.h"

void C2_HOOK_CDECL BrMaterialUpdate(br_material* mat, br_uint_16 flags);

void C2_HOOK_STDCALL BrMaterialClear(br_material* mat);

#endif // REC2_PREPMATL_H
