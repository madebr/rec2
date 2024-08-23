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

void C2_HOOK_FASTCALL DoColour256(tFlic_descriptor* pFlic_info, tU32 chunk_length);

void C2_HOOK_FASTCALL DoDeltaTrans(tFlic_descriptor* pFlic_info, tU32 chunk_length);

void C2_HOOK_FASTCALL DoDeltaX(tFlic_descriptor* pFlic_info, tU32 chunk_length);

void C2_HOOK_FASTCALL DoColourMap(tFlic_descriptor_ptr pFlic_info, tU32 chunk_length);

void C2_HOOK_FASTCALL DoDifferenceX(tFlic_descriptor* pFlic_info, tU32 chunk_length);

void C2_HOOK_FASTCALL DoDifferenceTrans(tFlic_descriptor* pFlic_info, tU32 chunk_length);

void C2_HOOK_FASTCALL DoBlack(tFlic_descriptor* pFlic_info, tU32 chunk_length);

void C2_HOOK_FASTCALL DoRunLengthX(tFlic_descriptor* pFlic_info, tU32 chunk_length);

void C2_HOOK_FASTCALL DoRunLengthTrans(tFlic_descriptor* pFlic_info, tU32 chunk_length);

void C2_HOOK_FASTCALL DoUncompressed(tFlic_descriptor* pFlic_info, tU32 chunk_length);

void C2_HOOK_FASTCALL DoUncompressedTrans(tFlic_descriptor* pFlic_info, tU32 chunk_length);

void C2_HOOK_FASTCALL DrawTranslations(tFlic_descriptor* pFlic_info);

int C2_HOOK_FASTCALL PlayNextFlicFrame2(tFlic_descriptor* pFlic_info, int pPanel_flic);

int C2_HOOK_FASTCALL PlayNextFlicFrame(tFlic_descriptor* pFlic_info);

int C2_HOOK_FASTCALL PlayFlic(int pIndex, tU32 pSize, tS8* pData_ptr, br_pixelmap* pDest_pixelmap, int pX_offset, int pY_offset, tPlayFlic_DoPerFrame DoPerFrame, int pInterruptable, int pFrame_rate);

void C2_HOOK_FASTCALL SwapScreen(void);

void C2_HOOK_FASTCALL InitFlics(void);

int C2_HOOK_FASTCALL LoadFlic(int pIndex);

void C2_HOOK_FASTCALL UnlockFlic(int pIndex);

int C2_HOOK_FASTCALL LoadFlicData(char* pName, tU8** pData, tU32* pData_length);

void C2_HOOK_FASTCALL FreeFlic(int pIndex);

void C2_HOOK_FASTCALL RunFlicAt(int pIndex, int pX, int pY);

void C2_HOOK_FASTCALL RunFlic(int pIndex);

void C2_HOOK_FASTCALL PreloadBunchOfFlics(int pBunch_index);

void C2_HOOK_FASTCALL UnlockBunchOfFlics(int pBunch_index);

void C2_HOOK_FASTCALL FlushAllFlics(int pBunch_index);

void C2_HOOK_FASTCALL InitFlicQueue(void);

int C2_HOOK_FASTCALL FlicQueueFinished(void);

void C2_HOOK_FASTCALL ProcessFlicQueue(tU32 pInterval);

void C2_HOOK_FASTCALL FlushFlicQueue(void);

void C2_HOOK_FASTCALL AddToFlicQueue(int pIndex, int pX, int pY, int pMust_finish);

void C2_HOOK_FASTCALL InitialiseFlicPanel(int pIndex, int pLeft, int pTop, int pWidth, int pHeight);

void C2_HOOK_FASTCALL DisposeFlicPanel(int pIndex);

void C2_HOOK_FASTCALL ServicePanelFlics(int pCopy_to_buffer);

void C2_HOOK_FASTCALL ChangePanelFlic(int pIndex, tU8* pData, tU32 pData_length);

br_pixelmap* C2_HOOK_FASTCALL GetPanelPixelmap(int pIndex);

void C2_HOOK_FASTCALL LoadInterfaceStrings(void);

void C2_HOOK_FASTCALL SuspendPendingFlic(void);

void C2_HOOK_FASTCALL ResumePendingFlic(void);

int C2_HOOK_FASTCALL TranslationMode(void);

void C2_HOOK_FASTCALL PlayFlicsFromDisk(void);

void C2_HOOK_FASTCALL PlayFlicsFromMemory(void);

void C2_HOOK_FASTCALL TurnOffPanelFlics(void);

void C2_HOOK_FASTCALL TurnOnPanelFlics(void);

#endif
