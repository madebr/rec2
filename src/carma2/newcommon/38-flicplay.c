#include "38-flicplay.h"

#include "rec2_types.h"

// GLOBAL: CARMA2_HW 0x0068b8b4
float gFlic_sound_delay_pre_smack;

// GLOBAL: CARMA2_HW 0x0068b8b8
float gFlic_sound_delay_post_smack;

// GLOBAL: CARMA2_HW 0x0068b8bc
float gFlic_sound_delay_not_in_demo;

// GLOBAL: CARMA2_HW 0x0068b8c0
float gFlic_sound_delay_post_demo;

// GLOBAL: CARMA2_HW 0x006861dc
int gTranslation_count;

// GLOBAL: CARMA2_HW 0x006861e8
tFlic_descriptor* gFirst_flic;

// FUNCTION: CARMA2_HW 0x00461990
int C2_HOOK_FASTCALL TranslationMode(void) {

    return gTranslation_count;
}

// DontLetFlicFuckWithPalettes

// LetFlicFuckWithPalettes

// TurnFlicTransparencyOn

// TurnFlicTransparencyOff

// PlayFlicsFromDisk

// TurnOffPanelFlics

// TurnOnPanelFlics

// STUB: CARMA2_HW 0x00461a60
void C2_HOOK_FASTCALL FlicPaletteAllocate(void) {
    NOT_IMPLEMENTED();
}

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

// STUB: CARMA2_HW 0x00462a40
void C2_HOOK_FASTCALL InitFlics(void) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00462a60
int C2_HOOK_FASTCALL LoadFlic(int pIndex) {

    return 0;
}

// STUB: CARMA2_HW 0x00462a70
void C2_HOOK_FASTCALL UnlockFlic(int pIndex) {
    NOT_IMPLEMENTED();
}

// LoadFlicData

// FUNCTION: CARMA2_HW 0x00462cc0
void C2_HOOK_FASTCALL InitFlicQueue(void) {

    gFirst_flic = NULL;
}

// FlicQueueFinished

// ProcessFlicQueue

// STUB: CARMA2_HW 0x00462dc0
void C2_HOOK_FASTCALL FlushFlicQueue(void) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x00463340
void C2_HOOK_FASTCALL LoadInterfaceStrings(void) {
    NOT_IMPLEMENTED();
}

// SuspendPendingFlic

