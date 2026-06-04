#include "38-flicplay.h"

// GLOBAL: CARMA2_HW 0x0068b8b4
float gFlic_sound_delay_pre_smack;

// GLOBAL: CARMA2_HW 0x0068b8b8
float gFlic_sound_delay_post_smack;

// GLOBAL: CARMA2_HW 0x0068b8bc
float gFlic_sound_delay_not_in_demo;

// GLOBAL: CARMA2_HW 0x0068b8c0
float gFlic_sound_delay_post_demo;

// TranslationMode

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

// LoadFlic

// UnlockFlic

// LoadFlicData

// InitFlicQueue

// FlicQueueFinished

// ProcessFlicQueue

// FlushFlicQueue

// STUB: CARMA2_HW 0x00463340
void C2_HOOK_FASTCALL LoadInterfaceStrings(void) {
    NOT_IMPLEMENTED();
}

// SuspendPendingFlic

