/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_38_FLICPLAY_H
#define GUARD_38_FLICPLAY_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern int gFlic_bunch0[29];
extern int gFlic_bunch1[31];
extern int gFlic_bunch2[8];
extern int gFlic_bunch3[13];
extern int gFlic_bunch4[22];
extern int gFlic_bunch5[5];
extern int gFlic_bunch6[51];
extern int gFlic_bunch7[7];
extern int gFlic_bunch8[16];
extern tFlic_bunch gFlic_bunch[9];

extern float gFlic_sound_delay_pre_smack;
extern float gFlic_sound_delay_post_smack;
extern float gFlic_sound_delay_not_in_demo;
extern float gFlic_sound_delay_post_demo;

extern void* gPalette_pixels;
extern br_pixelmap* gPalette;
extern tTranslation_record* gTranslations;
extern tFlic_descriptor gPanel_flic[2];
extern tDR_font* gTrans_fonts[2];
extern int gTranslation_count;
extern tFlic_spec gMain_flic_list[372];
extern int gPalette_fuck_prevention;
extern int gDark_mode;
extern int gTransparency_on;
extern int gPlay_from_disk;
extern int gPanel_flic_disable;
extern int gPending_flic;
extern int gPending_pending_flic;

extern int C2_HOOK_FASTCALL TranslationMode(void);

extern void C2_HOOK_FASTCALL DontLetFlicFuckWithPalettes(void);

extern void C2_HOOK_FASTCALL LetFlicFuckWithPalettes(void);

extern void C2_HOOK_STDCALL PlayFlicsInDarkness(void);

extern void C2_HOOK_FASTCALL TurnFlicTransparencyOn(void);

extern void C2_HOOK_FASTCALL TurnFlicTransparencyOff(void);

extern void C2_HOOK_FASTCALL PlayFlicsFromDisk(void);

extern void C2_HOOK_FASTCALL PlayFlicsFromMemory(void);

extern void C2_HOOK_FASTCALL TurnOffPanelFlics(void);

extern void C2_HOOK_FASTCALL TurnOnPanelFlics(void);

extern int C2_HOOK_FASTCALL GetPanelFlicFrameIndex(int pIndex);

extern void C2_HOOK_FASTCALL FlicPaletteAllocate(void);

// AssertFlicPixelmap

// StartFlic

extern int C2_HOOK_FASTCALL EndFlic(tFlic_descriptor_ptr pFlic_info);

// DoColourMap

extern void C2_HOOK_FASTCALL DoDifferenceX(tFlic_descriptor* pFlic_info, tU32 pChunk_length);

extern void C2_HOOK_FASTCALL DoDifferenceTrans(tFlic_descriptor* pFlic_info, tU32 pChunk_length);

// DoColour256

// DoDeltaTrans

extern void C2_HOOK_FASTCALL DoBlack(tFlic_descriptor* pFlic_info, tU32 pChunk_length);

extern void C2_HOOK_FASTCALL DoRunLengthTrans(tFlic_descriptor* pFlic_info, tU32 pChunk_length);

extern void C2_HOOK_FASTCALL DoUncompressed(tFlic_descriptor* pFlic_info, tU32 pChunk_length);

extern void C2_HOOK_FASTCALL DoUncompressedTrans(tFlic_descriptor* pFlic_info, tU32 pChunk_length);

// DoMini

// DrawTranslations

extern int C2_HOOK_FASTCALL PlayNextFlicFrame(tFlic_descriptor* pFlic_info);

extern void C2_HOOK_FASTCALL InitFlics(void);

extern int C2_HOOK_FASTCALL LoadFlic(int pIndex);

extern void C2_HOOK_FASTCALL UnlockFlic(int pIndex);

extern int C2_HOOK_FASTCALL LoadFlicData(char* pName, tU8** pData, tU32* pData_length);

extern void C2_HOOK_FASTCALL FreeFlic(int pIndex);

extern void C2_HOOK_FASTCALL PreloadBunchOfFlics(int pBunch_index);

extern void C2_HOOK_FASTCALL UnlockBunchOfFlics(int pBunch_index);

extern void C2_HOOK_FASTCALL InitFlicQueue(void);

extern int C2_HOOK_FASTCALL FlicQueueFinished(void);

extern void C2_HOOK_FASTCALL ProcessFlicQueue(tU32 pInterval);

extern void C2_HOOK_FASTCALL FlushFlicQueue(void);

extern void C2_HOOK_FASTCALL LoadInterfaceStrings(void);

extern void C2_HOOK_FASTCALL SuspendPendingFlic(void);

extern void C2_HOOK_FASTCALL ResumePendingFlic(void);

#endif // GUARD_38_FLICPLAY_H
