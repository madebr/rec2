#include "matsupt.h"

#include "dbsetup.h"

#include "core/fw/resource.h"

#include "c2_string.h"

#include <stddef.h>

br_material* C2_HOOK_CDECL BrMaterialAllocate(const char* name) {
    br_material* m;

    C2_HOOK_BUG_ON(sizeof(br_material) != 156);
    C2_HOOK_ASSERT((uintptr_t)&C2V(v1db).default_material==(uintptr_t)0x0079f49c);

    m = BrResAllocate(C2V(v1db).res, sizeof(br_material), BR_MEMORY_MATERIAL);
    c2_memcpy(m, C2V(v1db).default_material, sizeof(br_material));
    m->stored = NULL;
    if (name != NULL) {
        m->identifier = BrResStrDup(m, name);
    } else {
        m->identifier = NULL;
    }
    return m;
}
C2_HOOK_FUNCTION(0x0051f240, BrMaterialAllocate)

void C2_HOOK_CDECL BrMaterialFree(br_material* m) {

    BrResFree(m);
}
C2_HOOK_FUNCTION(0x0051f2a0, BrMaterialFree)
