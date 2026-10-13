#ifndef GUARD_38_FLICPLAY_H
#define GUARD_38_FLICPLAY_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern int gPending_flic;
extern int gPending_pending_flic;
extern int gTransparency_on;
extern int gPanel_flic_disable;
extern int gPlay_from_disk;
extern char gLast_flic_name[14];

extern float gFlic_sound_delay_pre_smack;
extern float gFlic_sound_delay_post_smack;
extern float gFlic_sound_delay_not_in_demo;
extern float gFlic_sound_delay_post_demo;

extern void* gPalette_pixels;
extern int gPalette_fuck_prevention;
extern br_pixelmap* gPalette;
extern tTranslation_record* gTranslations;
extern tDR_font* gTrans_fonts[2];
extern int gTranslation_count;
extern tFlic_spec gMain_flic_list[372];
extern tFlic_descriptor* gFirst_flic;

extern int C2_HOOK_FASTCALL TranslationMode(void);

extern void C2_HOOK_FASTCALL DontLetFlicFuckWithPalettes(void);

extern void C2_HOOK_FASTCALL LetFlicFuckWithPalettes(void);

extern void C2_HOOK_FASTCALL TurnFlicTransparencyOn(void);

extern void C2_HOOK_FASTCALL TurnFlicTransparencyOff(void);

void C2_HOOK_FASTCALL PlayFlicsFromDisk(void);

void C2_HOOK_FASTCALL PlayFlicsFromMemory(void);

void C2_HOOK_FASTCALL TurnOffPanelFlics(void);

void C2_HOOK_FASTCALL TurnOnPanelFlics(void);

extern void C2_HOOK_FASTCALL FlicPaletteAllocate(void);

extern void C2_HOOK_FASTCALL AssertFlicPixelmap(tFlic_descriptor_ptr pFlic_info, br_pixelmap* pDest_pixelmap);

extern int C2_HOOK_FASTCALL StartFlic(char* pFile_name, int pIndex, tFlic_descriptor_ptr pFlic_info, tU32 pSize, tS8* pData_ptr, br_pixelmap* pDest_pixelmap, int pX_offset, int pY_offset, int pFrame_rate);

extern int C2_HOOK_FASTCALL EndFlic(tFlic_descriptor_ptr pFlic_info);

extern void C2_HOOK_FASTCALL DoColourMap(tFlic_descriptor_ptr pFlic_info, tU32 chunk_length);

extern void C2_HOOK_FASTCALL DoDifferenceX(tFlic_descriptor* pFlic_info, tU32 chunk_length);

extern void C2_HOOK_FASTCALL DoDifferenceTrans(tFlic_descriptor* pFlic_info, tU32 chunk_length);

extern void C2_HOOK_FASTCALL DoColour256(tFlic_descriptor* pFlic_info, tU32 chunk_length);

extern void C2_HOOK_FASTCALL DoDeltaTrans(tFlic_descriptor* pFlic_info, tU32 chunk_length);

extern void C2_HOOK_FASTCALL DoBlack(tFlic_descriptor* pFlic_info, tU32 chunk_length);

extern void C2_HOOK_FASTCALL DoRunLengthTrans(tFlic_descriptor* pFlic_info, tU32 chunk_length);

extern void C2_HOOK_FASTCALL DoUncompressed(tFlic_descriptor* pFlic_info, tU32 chunk_length);

extern void C2_HOOK_FASTCALL DoUncompressedTrans(tFlic_descriptor* pFlic_info, tU32 chunk_length);

extern void C2_HOOK_FASTCALL DoMini(tFlic_descriptor* pFlic_info, tU32 chunk_length);

extern void C2_HOOK_FASTCALL DrawTranslations(tFlic_descriptor* pFlic_info);

extern int C2_HOOK_FASTCALL PlayNextFlicFrame(tFlic_descriptor* pFlic_info);

extern void C2_HOOK_FASTCALL InitFlics(void);

extern int C2_HOOK_FASTCALL LoadFlic(int pIndex);

extern void C2_HOOK_FASTCALL UnlockFlic(int pIndex);

extern int C2_HOOK_FASTCALL LoadFlicData(char* pName, tU8** pData, tU32* pData_length);

extern void C2_HOOK_FASTCALL InitFlicQueue(void);

extern int C2_HOOK_FASTCALL FlicQueueFinished(void);

extern void C2_HOOK_FASTCALL ProcessFlicQueue(tU32 pInterval);

extern void C2_HOOK_FASTCALL FlushFlicQueue(void);

extern void C2_HOOK_FASTCALL LoadInterfaceStrings(void);

void C2_HOOK_FASTCALL SuspendPendingFlic(void);

void C2_HOOK_FASTCALL ResumePendingFlic(void);

#endif // GUARD_38_FLICPLAY_H
