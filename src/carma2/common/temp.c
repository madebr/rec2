#include "temp.h"

C2_HOOK_VARIABLE_IMPLEMENT(int, gMaterials_to_adapt_count, 0x006a6d38);

void C2_HOOK_FASTCALL TemporaryMaterialStorageInit(void) {

    C2V(gMaterials_to_adapt_count) = 0;
}
C2_HOOK_FUNCTION(0x004ea760, TemporaryMaterialStorageInit)
