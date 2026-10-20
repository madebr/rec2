#include "46-win95sys.h"

#include "platform.h"

// FUN_100bf5a4

// AsmDoDeltaX

// DoDeltaX

// FUN_100bf768

// DoRunLengthX

// main

// CentreRect

// GetCentredDialog

// CentredStopAlert

// CentredCautionAlert

// GetDRect

// BoldDefault

// BoldDefault1

// PDFatalError

// PrefsError

// InitializePreferences

// MungePrefs

// WriteOutPrefs

// EncryptPassword

// GoDecideOnPassword

// AskForPassword

// DoPassword

// PDInitialiseSystem

// PDShutdownSystem

// PDSaveOriginalPalette

// SafeSetEntries

// PDRevertPalette

// PDInitScreenVars

// PDInitScreen

// PDAllocateScreenAndBack

// DoubleToScreen

// PDScreenBufferSwap

// DoWarningMessage

// DoErrorMessage

// PDInstallErrorHandlers

// strinsert

// HCalcPathName

// PDSetFileVariables

// PDBuildAppPath

// PDForEveryFile

// PDSetPaletteEntries

// PDSetPalette

// KeyDown2

// PDSetKeyArray

// PDGetASCIIFromKey

// PDMouseButtons

// PDMouseDoubleClickedAt

// PDGetMousePosition

// PDGetTotalTime

// PDInitTimers

// PDGetMicroseconds

// PDServiceSystem

// PDEnterDebugger

// FUNCTION: CARMA2_HW 0x0051b9a0
void C2_HOOK_FASTCALL PDAllocateActionReplayBuffer(tU8** pBuffer, tU32* pSize) {

    PDReallyAllocateActionReplayBuffer();
    if (pBuffer != NULL) {
        *pBuffer = gPDActionReplayBuffer;
    }
    if (pSize != NULL) {
        *pSize = gPDActionReplayBufferSize;
    }
}

// PDDisposeActionReplayBuffer

// PDInitJoysticks

// PDReadJoySticks

// PDGetJoy1X

// PDGetJoy1Y

// PDGetJoy2X

// PDGetJoy2Y

// PDGetJoy1Button1

// PDGetJoy1Button2

// PDGetJoy1Button3

// PDGetJoy1Button4

// PDGetJoy2Button1

// PDGetJoy2Button2

// PDGetJoy2Button3

// PDGetJoy2Button4

// PDFileUnlock

// PDCheckDriveExists2

// PDDoWeLeadAnAustereExistance

// NextSmackerFrame

// SetUpDeviousSmack

// FreeDeviousSmack

// MacPlaySmackerFile

// PDFileDelete

// TwatPIX16

// PDGetCurrentTime

// PDGetCurrentDate

// PDPhysicalMemorySize

// iMacKeyboard

// ShowError

// PDNetInitialise

// MapperNotifier

