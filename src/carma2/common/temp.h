#ifndef TEMP_H
#define TEMP_H

#include <brender/brender.h>

#include "c2_hooks.h"

void C2_HOOK_FASTCALL TemporaryMaterialStorageInit(void);

br_material* C2_HOOK_FASTCALL LoadTemporaryMaterial(const char* pName);

void C2_HOOK_FASTCALL EnableMaterialAdapt(void);

void C2_HOOK_FASTCALL DisableMaterialAdapt(void);

#endif /* TEMP_H */
