#include "53-controls.h"

#include "15-displays.h"
#include "41-utility.h"
#include "70-packfile.h"
#include "globvars.h"
#include "platform.h"
#include "rec2_macros.h"

#include "c2_string.h"

#include <stdio.h>

// GLOBAL: CARMA2_HW 0x00659b2c
float gMap_render_x = 80.f;

// GLOBAL: CARMA2_HW 0x00659b30
float gMap_render_y = 6.f;

// GLOBAL: CARMA2_HW 0x00659b34
float gMap_render_width = 128.f;

// GLOBAL: CARMA2_HW 0x00659b38
float gMap_render_height = 80.f;

// GLOBAL: CARMA2_HW 0x0068d8d4
int gMap_trans;

// GLOBAL: CARMA2_HW 0x0074abac
int gHeadup_map_x;

// GLOBAL: CARMA2_HW 0x0074abb0
int gHeadup_map_y;

// GLOBAL: CARMA2_HW 0x0074aba8
int gHeadup_map_w;

// GLOBAL: CARMA2_HW 0x0074abb4
int gHeadup_map_h;

// GLOBAL: CARMA2_HW 0x0067c400
char gString[84];

// GLOBAL: CARMA2_HW 0x0067c3c8
char* gAbuse_text[10];

// CompleteRace

// AbortRace

// F4Key

// SetFlag

// FinishRace

// FUNCTION: CARMA2_HW 0x004415e0
void C2_HOOK_FASTCALL EnsureSpecialVolumesHidden(void) {

}

// FUNCTION: CARMA2_HW 0x004415f0
void C2_HOOK_FASTCALL ShowSpecialVolumesIfRequ(void) {

}

// DoEditModeKey

// F5Key

// F6Key

// F7Key

// F8Key

// F10Key

// F11Key

// FUNCTION: CARMA2_HW 0x00441900
void C2_HOOK_FASTCALL F12Key(void) {
}

// NumberKey0

// NumberKey1

// NumberKey2

// NumberKey3

// NumberKey4

// NumberKey5

// NumberKey6

// NumberKey7

// NumberKey8

// NumberKey9

// LookLeft

// LookForward

// LookRight

// FUNCTION: CARMA2_HW 0x00441fd0
// FUNCTION: CARMA2_SW 0x004420b0
void C2_HOOK_FASTCALL DamageTest(void) {

}

// ToggleMiniMap

// CheckToggles

// CarWorldOffFallenCheckThingy

// HasCarFallenOffWorld

// CheckForBeingOutOfThisWorld

// CheckHornLocal

// CheckHorn3D

// CheckHorns

// SetQuickRecovery

// SetRecovery

// RecoverCar

// CheckMapRenderMove

// ExplodeCar

// CheckRecoveryOfCars

// TradeInAPO

// CheckOtherRacingKeys

// CheckRecoverCost

// SortOutRecover

// SetFlipUpCar

// AlignChildren

// FlipUpCar

// GetPowerup

// CheckSystemKeys

// CheckForCheatingGits

// CheckKevKeys

// BrakeInstantly

// PollCarControls

// PollCameraControls

// SetFlag2

// ToggleFlying

// CycleInvulnerability

// FUNCTION: CARMA2_HW 0x00444590
void C2_HOOK_FASTCALL ToggleTimerFreeze(void) {

    gFreeze_timer = !gFreeze_timer;
    if (gFreeze_timer) {
        NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_timer_frozen));
    } else {
        NewTextHeadupSlot(4, 0, 1000, -4, "Timer thawed out");
    }
}

// EarnDosh

// LoseDosh

// FUNCTION: CARMA2_HW 0x00444600
void C2_HOOK_FASTCALL ToggleHeadupMap(int pArg) {

}

// HackyMapFixForMac3dfx

// ToggleMap

// GetRecoverVoucherCount

// AddVouchers

// ResetRecoveryVouchers

// CycleYonFactor

// CycleSoundDetailLevel

// CycleCarSimplificationLevel

// ToggleAccessoryRendering

// UserSendMessage

// EnterUserMessage

// DisplayUserMessage

// FUNCTION: CARMA2_HW 0x00444d70
void C2_HOOK_FASTCALL InitAbuseomatic(void) {
    char path[256];
    char s[256];
    FILE* f;
    int i;
    int len;

    gString[20] = '\0';
    PDBuildAppPath(path);
    strcat(path, "ABUSE.TXT");
    for (i = 0; i < REC2_ASIZE(gAbuse_text); i++) {
        gAbuse_text[i] = NULL;
    }
    f = PFfopen(path, "rt");
    if (f == NULL) {
        return;
    }
    for (i = 0; i < (int)REC2_ASIZE(gAbuse_text); i++) {
        if (PFfgets(s, REC2_ASIZE(s) - 1, f) == NULL) {
            break;
        }
        // len = strlen(s);
        if (strlen(s) > 63) {
            s[63] = '\0';
        }
        len = strlen(s);
        while (len != 0 && s[len - 1] < ' ') {
            s[--len] = '\0';
        }
        gAbuse_text[i] = BrMemAllocate(strlen(s) + 1, kMem_abuse_text);
        strcpy(gAbuse_text[i], s);
    }
    PFfclose(f);
}

// FUNCTION: CARMA2_HW 0x00444ea0
void C2_HOOK_FASTCALL DisposeAbuseomatic(void) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gAbuse_text); i++) {
        if (gAbuse_text[i] != NULL) {
            BrMemFree(gAbuse_text[i]);
        }
    }
}

// ChangeCameraTypeInGame

// SteelBollock

// RichardsSphere

// ToggleHeadupLevel

