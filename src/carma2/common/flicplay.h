#ifndef REC2_FLICPLAY_H
#define REC2_FLICPLAY_H

#include "c2_hooks.h"

#include "rec2_types.h"

int C2_HOOK_FASTCALL StartFlic(char* pFile_name, int pIndex, tFlic_descriptor_ptr pFlic_info, tU32 pSize, tS8* pData_ptr, br_pixelmap* pDest_pixelmap, int pX_offset, int pY_offset, int pFrame_rate);

int C2_HOOK_FASTCALL EndFlic(tFlic_descriptor_ptr pFlic_info);

void C2_HOOK_FASTCALL InitFlics(void);

int C2_HOOK_FASTCALL LoadFlic(int pIndex);

void C2_HOOK_FASTCALL UnlockFlic(int pIndex);

int C2_HOOK_FASTCALL LoadFlicData(char* pName, tU8** pData, tU32* pData_length);

void C2_HOOK_FASTCALL FreeFlic(int pIndex);

#endif //REC2_FLICPLAY_H
