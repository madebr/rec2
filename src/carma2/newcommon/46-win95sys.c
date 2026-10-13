#include "46-win95sys.h"

#include "08-loading1.h"

#include "c2_string.h"

// FUN_100bf5a4

// AsmDoDeltaX

void C2_HOOK_FASTCALL DoDeltaX(tFlic_descriptor* pFlic_info, tU32 chunk_length) {
    int i;
    int j;
    int k;
    int line_count;
    int number_of_packets;
    int skip_count;
    int size_count;
    int the_row_bytes;
    tU8* pixel_ptr;
    tU8* line_pixel_ptr;
    tU8 pair[2];

    (void)chunk_length;
    line_count = MemReadU16(&pFlic_info->data);
    the_row_bytes = pFlic_info->the_pixelmap->row_bytes;
    pixel_ptr = pFlic_info->first_pixel;
    for (i = 0; i < line_count;) {
        number_of_packets = MemReadS16(&pFlic_info->data);
        if (number_of_packets < 0) {
            pixel_ptr += the_row_bytes * -number_of_packets;
        } else {
            line_pixel_ptr = pixel_ptr;
            for (j = 0; j < number_of_packets; j++) {
                skip_count = MemReadU8(&pFlic_info->data);
                size_count = MemReadS8(&pFlic_info->data);
                line_pixel_ptr += (skip_count / 2) * 2;
                if (size_count < 0) {
                    memcpy(pair, pFlic_info->data, sizeof(pair));
                    pFlic_info->data += sizeof(pair);
                    for (k = 0; k < -size_count; k++) {
                        memcpy(line_pixel_ptr, pair, sizeof(pair));
                        line_pixel_ptr += sizeof(pair);
                    }
                } else {
                    for (k = 0; k < size_count; k++) {
                        memcpy(pair, pFlic_info->data, sizeof(pair));
                        memcpy(line_pixel_ptr, pair, sizeof(pair));
                        pFlic_info->data += sizeof(pair);
                        line_pixel_ptr += sizeof(pair);
                    }
                }
            }
            pixel_ptr += the_row_bytes;
            i++;
        }
    }
}

// FUN_100bf768

// FUNCTION: CARMA2_HW 0x004625d0
void C2_HOOK_FASTCALL DoRunLengthX(tFlic_descriptor* pFlic_info, tU32 chunk_length) {
    int i;
    int j;
    int k;
    int number_of_packets;
    int size_count;
    int the_row_bytes;
    tU8* pixel_ptr;
    tU8* line_pixel_ptr;
    tU8 the_byte;

    (void)chunk_length;
    the_row_bytes = pFlic_info->the_pixelmap->row_bytes;
    pixel_ptr = pFlic_info->first_pixel;
    for (i = 0; i < pFlic_info->height; i++) {
        line_pixel_ptr = pixel_ptr;
        number_of_packets = MemReadU8(&pFlic_info->data);
        for (j = 0; j < number_of_packets; j++) {
            size_count = MemReadS8(&pFlic_info->data);
            if (size_count >= 0) {
                the_byte = MemReadU8(&pFlic_info->data);
                for (k = 0; k < size_count; k++) {
                    *line_pixel_ptr = the_byte;
                    line_pixel_ptr++;
                }
            } else {
                for (k = 0; k < -size_count; k++) {
                    *line_pixel_ptr = MemReadU8(&pFlic_info->data);
                    line_pixel_ptr++;
                }
            }
        }
        pixel_ptr += the_row_bytes;
    }
}

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

// PDAllocateActionReplayBuffer

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

