#ifndef GUARD_38_FLICPLAY_H
#define GUARD_38_FLICPLAY_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern int gPending_flic;
extern int gPending_pending_flic;
extern int gPanel_flic_disable;
extern int gPlay_from_disk;

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

extern int C2_HOOK_FASTCALL TranslationMode(void);

// DontLetFlicFuckWithPalettes

// LetFlicFuckWithPalettes

// TurnFlicTransparencyOn

// TurnFlicTransparencyOff

void C2_HOOK_FASTCALL PlayFlicsFromDisk(void);

void C2_HOOK_FASTCALL PlayFlicsFromMemory(void);

void C2_HOOK_FASTCALL TurnOffPanelFlics(void);

void C2_HOOK_FASTCALL TurnOnPanelFlics(void);

extern void C2_HOOK_FASTCALL FlicPaletteAllocate(void);

// AssertFlicPixelmap

// StartFlic

// EndFlic

extern void C2_HOOK_FASTCALL DoColourMap(tFlic_descriptor_ptr pFlic_info, tU32 chunk_length);

extern void C2_HOOK_FASTCALL DoDifferenceX(tFlic_descriptor* pFlic_info, tU32 chunk_length);

extern void C2_HOOK_FASTCALL DoDifferenceTrans(tFlic_descriptor* pFlic_info, tU32 chunk_length);

extern void C2_HOOK_FASTCALL DoColour256(tFlic_descriptor* pFlic_info, tU32 chunk_length);

extern void C2_HOOK_FASTCALL DoDeltaTrans(tFlic_descriptor* pFlic_info, tU32 chunk_length);

extern void C2_HOOK_FASTCALL DoBlack(tFlic_descriptor* pFlic_info, tU32 chunk_length);

extern void C2_HOOK_FASTCALL DoRunLengthTrans(tFlic_descriptor* pFlic_info, tU32 chunk_length);

extern void C2_HOOK_FASTCALL DoUncompressed(tFlic_descriptor* pFlic_info, tU32 chunk_length);

extern void C2_HOOK_FASTCALL DoUncompressedTrans(tFlic_descriptor* pFlic_info, tU32 chunk_length);

// DoMini

// DrawTranslations

// PlayNextFlicFrame

extern void C2_HOOK_FASTCALL InitFlics(void);

extern int C2_HOOK_FASTCALL LoadFlic(int pIndex);

extern void C2_HOOK_FASTCALL UnlockFlic(int pIndex);

// LoadFlicData

extern void C2_HOOK_FASTCALL InitFlicQueue(void);

// FlicQueueFinished

// ProcessFlicQueue

extern void C2_HOOK_FASTCALL FlushFlicQueue(void);

extern void C2_HOOK_FASTCALL LoadInterfaceStrings(void);

void C2_HOOK_FASTCALL SuspendPendingFlic(void);

void C2_HOOK_FASTCALL ResumePendingFlic(void);

#endif // GUARD_38_FLICPLAY_H
