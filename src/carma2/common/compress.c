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

void C2_HOOK_FASTCALL ExpandVector3(br_vector3* pDest, const tCompressed_vector3 *pSrc, float pMin, float pMax) {

   pDest->v[0] = DRU16ToScalar(pSrc->v[0], pMin, pMax);
   pDest->v[1] = DRU16ToScalar(pSrc->v[1], pMin, pMax);
   pDest->v[2] = DRU16ToScalar(pSrc->v[2], pMin, pMax);
}
C2_HOOK_FUNCTION(0x00516570, ExpandVector3)

void C2_HOOK_FASTCALL CompressMatrix34(tCompressed_matrix3* pCompressed_matrix3, int* pInactive, const br_matrix34* pMatrix) {
    br_vector3 pos;

    if (pMatrix->m[3][0] < 500.f) {
        *pInactive = 0;
        BrVector3Copy(&pos, (const br_vector3*)pMatrix->m[3]);
    } else {
        *pInactive = 1;
        BrVector3Set(&pos,
            pMatrix->m[3][0] - 1000.f,
            pMatrix->m[3][1] - 1000.f,
            pMatrix->m[3][2] - 1000.f);
    }
    CompressVector3(&pCompressed_matrix3->m0, (const br_vector3*)pMatrix->m[0], -1.1f, 1.1f);
    CompressVector3(&pCompressed_matrix3->m1, (const br_vector3*)pMatrix->m[1], -1.1f, 1.1f);
    CompressVector3(&pCompressed_matrix3->p, &pos, -300.f, 300.f);
}
C2_HOOK_FUNCTION(0x005165e0, CompressMatrix34)

void C2_HOOK_FASTCALL ExpandMatrix34(br_matrix34* pMatrix, const tCompressed_matrix3* pCompressed, int pInactive) {

    ExpandVector3((br_vector3*)pMatrix->m[0], &pCompressed->m0, -1.1f, 1.1f);
    BrVector3Normalise((br_vector3*)pMatrix->m[0], (br_vector3*)pMatrix->m[0]);
    ExpandVector3((br_vector3*)pMatrix->m[1], &pCompressed->m1, -1.1f, 1.1f);
    BrVector3Normalise((br_vector3*)pMatrix->m[1], (br_vector3*)pMatrix->m[1]);
    BrVector3Cross((br_vector3*)pMatrix->m[2], (br_vector3*)pMatrix->m[0], (br_vector3*)pMatrix->m[1]);
    BrVector3Normalise((br_vector3*)pMatrix->m[2], (br_vector3*)pMatrix->m[2]);
    ExpandVector3((br_vector3*)pMatrix->m[3], &pCompressed->p, 300.f, 300.f);
    if (pInactive) {
        BrVector3Set((br_vector3*)pMatrix->m[3],
            pMatrix->m[3][0] + 1000.f,
            pMatrix->m[3][1] + 1000.f,
            pMatrix->m[3][2] + 1000.f);
    }
}
C2_HOOK_FUNCTION(0x00516910, ExpandMatrix34)
