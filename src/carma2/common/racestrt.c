#include "racestrt.h"

#include "rec2_types.h"

int C2_HOOK_CDECL SortGridFunction(const void* pFirst_one, const void* pSecond_one) {

    return ((tOpp_spec*)pFirst_one)->ranking - ((tOpp_spec*)pSecond_one)->ranking;
}
C2_HOOK_FUNCTION(0x004e2b50, SortGridFunction)
