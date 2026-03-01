#include "compress.h"

tU16 C2_HOOK_FASTCALL DRScalarToU16(float pValue, float pMin, float pMax) {

    if (pValue < pMin) {
        pValue = pMin;
    } else if (pValue > pMax) {
        pValue = pMax;
    }
    return (tU16)(((pValue - pMin) * 65535.f) / (pMax - pMin) + 0.5f);
}
C2_HOOK_FUNCTION(0x00516410, DRScalarToU16)
