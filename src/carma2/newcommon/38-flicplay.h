#ifndef GUARD_38_FLICPLAY_H
#define GUARD_38_FLICPLAY_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern float gFlic_sound_delay_pre_smack;
extern float gFlic_sound_delay_post_smack;
extern float gFlic_sound_delay_not_in_demo;
extern float gFlic_sound_delay_post_demo;

extern void* gPalette_pixels;
extern br_pixelmap* gPalette;
extern tTranslation_record* gTranslations;
extern tDR_font* gTrans_fonts[2];
extern int gTranslation_count;
extern tFlic_spec gMain_flic_list[372];
extern int gPalette_fuck_prevention;
extern int gTransparency_on;
extern int gPlay_from_disk;

extern int C2_HOOK_FASTCALL TranslationMode(void);

extern void C2_HOOK_FASTCALL DontLetFlicFuckWithPalettes(void);

extern void C2_HOOK_FASTCALL LetFlicFuckWithPalettes(void);

extern void C2_HOOK_FASTCALL TurnFlicTransparencyOn(void);

extern void C2_HOOK_FASTCALL TurnFlicTransparencyOff(void);

extern void C2_HOOK_FASTCALL PlayFlicsFromDisk(void);

// TurnOffPanelFlics

// TurnOnPanelFlics

extern void C2_HOOK_FASTCALL FlicPaletteAllocate(void);

// AssertFlicPixelmap

// StartFlic

extern int C2_HOOK_FASTCALL EndFlic(tFlic_descriptor_ptr pFlic_info);

// DoColourMap

// DoDifferenceX

// DoDifferenceTrans

// DoColour256

// DoDeltaTrans

// DoBlack

// DoRunLengthTrans

// DoUncompressed

// DoUncompressedTrans

// DoMini

// DrawTranslations

extern int C2_HOOK_FASTCALL PlayNextFlicFrame(tFlic_descriptor* pFlic_info);

extern void C2_HOOK_FASTCALL InitFlics(void);

extern int C2_HOOK_FASTCALL LoadFlic(int pIndex);

extern void C2_HOOK_FASTCALL UnlockFlic(int pIndex);

// LoadFlicData

extern void C2_HOOK_FASTCALL InitFlicQueue(void);

extern int C2_HOOK_FASTCALL FlicQueueFinished(void);

extern void C2_HOOK_FASTCALL ProcessFlicQueue(tU32 pInterval);

extern void C2_HOOK_FASTCALL FlushFlicQueue(void);

extern void C2_HOOK_FASTCALL LoadInterfaceStrings(void);

// SuspendPendingFlic

#endif // GUARD_38_FLICPLAY_H
