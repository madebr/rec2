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

extern int C2_HOOK_FASTCALL TranslationMode(void);

// DontLetFlicFuckWithPalettes

// LetFlicFuckWithPalettes

// TurnFlicTransparencyOn

// TurnFlicTransparencyOff

// PlayFlicsFromDisk

// TurnOffPanelFlics

// TurnOnPanelFlics

extern void C2_HOOK_FASTCALL FlicPaletteAllocate(void);

// AssertFlicPixelmap

// StartFlic

// EndFlic

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

// SuspendPendingFlic

#endif // GUARD_38_FLICPLAY_H
