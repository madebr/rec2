#ifndef REC2_FLICPLAY_H
#define REC2_FLICPLAY_H

#include "c2_hooks.h"

#include "rec2_types.h"

int C2_HOOK_FASTCALL GetPanelFlicFrameIndex(int pIndex);

void C2_HOOK_FASTCALL DontLetFlicFuckWithPalettes(void);

void C2_HOOK_FASTCALL LetFlicFuckWithPalettes(void);

void C2_HOOK_FASTCALL TurnFlicTransparencyOff(void);

void C2_HOOK_FASTCALL TurnFlicTransparencyOn(void);

void C2_HOOK_FASTCALL FlicPaletteAllocate(void);

void C2_HOOK_FASTCALL AssertFlicPixelmap(tFlic_descriptor_ptr pFlic_info, br_pixelmap* pDest_pixelmap);

int C2_HOOK_FASTCALL StartFlic(char* pFile_name, int pIndex, tFlic_descriptor_ptr pFlic_info, tU32 pSize, tS8* pData_ptr, br_pixelmap* pDest_pixelmap, int pX_offset, int pY_offset, int pFrame_rate);

int C2_HOOK_FASTCALL EndFlic(tFlic_descriptor_ptr pFlic_info);

void C2_HOOK_FASTCALL InitFlics(void);

int C2_HOOK_FASTCALL LoadFlic(int pIndex);

void C2_HOOK_FASTCALL UnlockFlic(int pIndex);

int C2_HOOK_FASTCALL LoadFlicData(char* pName, tU8** pData, tU32* pData_length);

void C2_HOOK_FASTCALL FreeFlic(int pIndex);

void C2_HOOK_FASTCALL RunFlicAt(int pIndex, int pX, int pY);

void C2_HOOK_FASTCALL RunFlic(int pIndex);

void C2_HOOK_FASTCALL PreloadBunchOfFlics(int pBunch_index);

void C2_HOOK_FASTCALL UnlockBunchOfFlics(int pBunch_index);

void C2_HOOK_FASTCALL FlicPaletteAllocate(void);

int C2_HOOK_FASTCALL TranslationMode(void);

#endif
