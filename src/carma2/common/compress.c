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

br_scalar C2_HOOK_FASTCALL DRU16ToScalar(tU16 pValue, float pMin, float pMax) {

    return pMin + (float)pValue * (pMax - pMin) / 65535.f;
}
C2_HOOK_FUNCTION(0x00516460, DRU16ToScalar)

void C2_HOOK_FASTCALL CompressVector3(tCompressed_vector3* pDest, const br_vector3* pSrc, float pMin, float pMax) {

    pDest->v[0] = DRScalarToU16(pSrc->v[0], pMin, pMax);
    pDest->v[1] = DRScalarToU16(pSrc->v[1], pMin, pMax);
    pDest->v[2] = DRScalarToU16(pSrc->v[2], pMin, pMax);
}
C2_HOOK_FUNCTION(0x00516490, CompressVector3)
