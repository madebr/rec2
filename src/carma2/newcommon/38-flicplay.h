#ifndef GUARD_38_FLICPLAY_H
#define GUARD_38_FLICPLAY_H

#include "c2_hooks.h"

// TranslationMode

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

// LoadFlic

// UnlockFlic

// LoadFlicData

// InitFlicQueue

// FlicQueueFinished

// ProcessFlicQueue

// FlushFlicQueue

extern void C2_HOOK_FASTCALL LoadInterfaceStrings(void);

// SuspendPendingFlic

#endif // GUARD_38_FLICPLAY_H
