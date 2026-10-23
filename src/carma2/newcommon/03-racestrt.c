#include "03-racestrt.h"

#include "rec2_types.h"

// FUNCTION: CARMA2_HW 0x004e2b50
// FUNCTION: CARMA2_SW 0x004deed0
int C2_HOOK_CDECL SortGridFunction(const void* pFirst_one, const void* pSecond_one) {

    return ((tOpp_spec*)pFirst_one)->ranking - ((tOpp_spec*)pSecond_one)->ranking;
}

// SortOpponents

// DoGridPosition

// ReceivedViewCar

