#ifndef REC2_FLICPLAY_H
#define REC2_FLICPLAY_H

#include "c2_hooks.h"

#include "rec2_types.h"

int C2_HOOK_FASTCALL StartFlic(char* pFile_name, int pIndex, tFlic_descriptor_ptr pFlic_info, tU32 pSize, tS8* pData_ptr, br_pixelmap* pDest_pixelmap, int pX_offset, int pY_offset, int pFrame_rate);

#endif //REC2_FLICPLAY_H
