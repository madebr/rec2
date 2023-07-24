#include "controls.h"

#include "sound.h"

#include "brender/brender.h"

#include "rec2_macros.h"

#define DEBUG_CONTROLS

#if defined(DEBUG_CONTROLS)
#include "c2_stdio.h"
#define CONTROLS_START() c2_fprintf(c2_stderr, "%s: start\n", __FUNCTION__); C2_HOOK_DEBUGF("")
#else
#define CONTROLS_START()
#endif

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(char*, gAbuse_text, 10, 0x0067c3c8);

void C2_HOOK_FASTCALL SetSoundDetailLevel(int pLevel) {

    C2V(gSound_detail_level) = pLevel;
}
C2_HOOK_FUNCTION(0x00456950, SetSoundDetailLevel)

void (C2_HOOK_FASTCALL * ReallySetSoundDetailLevel_original)(int pLevel);
void C2_HOOK_FASTCALL ReallySetSoundDetailLevel(int pLevel) {

#if defined(C2_HOOKS_ENABLED)
    ReallySetSoundDetailLevel_original(pLevel);
#else
    DRS3StopAllOutletSounds();
    DisposeSoundSources();
    C2V(gSound_detail_level) = pLevel;
    InitSound();
    InitSoundSources();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00456960, ReallySetSoundDetailLevel, ReallySetSoundDetailLevel_original)

int C2_HOOK_FASTCALL GetSoundDetailLevel(void) {

    return C2V(gSound_detail_level);
}
C2_HOOK_FUNCTION(0x004569e0, GetSoundDetailLevel)

// Key: 'm'
void (C2_HOOK_FASTCALL * ToggleMiniMap_original)(void);
void C2_HOOK_FASTCALL ToggleMiniMap(void) {
    CONTROLS_START();
#if defined(C2_HOOKS_ENABLED)
    ToggleMiniMap_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004420e0, ToggleMiniMap, ToggleMiniMap_original)

// Key: 'Enter'
void (C2_HOOK_FASTCALL * ToggleDoors_original)(void);
void C2_HOOK_FASTCALL ToggleDoors(void) {
    CONTROLS_START();
#if defined(C2_HOOKS_ENABLED)
    ToggleDoors_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0042dd50, ToggleDoors, ToggleDoors_original)

// Key: 'tab'
void (C2_HOOK_FASTCALL * ToggleMap_original)(void);
void C2_HOOK_FASTCALL ToggleMap(void) {
    CONTROLS_START();
#if defined(C2_HOOKS_ENABLED)
    ToggleMap_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00444610, ToggleMap, ToggleMap_original)

void C2_HOOK_FASTCALL FUN00444600(void) {
    CONTROLS_START();
    // Function has empty implementation
    // FIXME: check with 'carmshit'
}
C2_HOOK_FUNCTION(0x00444600, FUN00444600)

// Key: 'ctrl+tab'
void (C2_HOOK_FASTCALL * ToggleCheckpointFinder_original)(void);
void C2_HOOK_FASTCALL ToggleCheckpointFinder(void) {
    CONTROLS_START();
#if defined(C2_HOOKS_ENABLED)
    ToggleCheckpointFinder_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00494840, ToggleCheckpointFinder, ToggleCheckpointFinder_original)

// key: 'alt+tab' ('alt' in map mode)
void (C2_HOOK_FASTCALL * ToggleMapTrans_original)(void);
void C2_HOOK_FASTCALL ToggleMapTrans(void) {
    CONTROLS_START();
#if defined(C2_HOOKS_ENABLED)
    ToggleMapTrans_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00494880, ToggleMapTrans, ToggleMapTrans_original)

// Key: 'r'
void (C2_HOOK_FASTCALL * SetRecovery_original)(void);
void C2_HOOK_FASTCALL SetRecovery(void) {
    CONTROLS_START();
#if defined(C2_HOOKS_ENABLED)
    SetRecovery_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00442300, SetRecovery, SetRecovery_original)

// Key: 'ctrl+a'
void (C2_HOOK_FASTCALL * AbortRace_original)(void);
void C2_HOOK_FASTCALL AbortRace(void) {
    CONTROLS_START();
#if defined(C2_HOOKS_ENABLED)
    AbortRace_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00441490, AbortRace, AbortRace_original)

// Key: 's'
void (C2_HOOK_FASTCALL * ToggleSoundEnable_original)(void);
void C2_HOOK_FASTCALL ToggleSoundEnable(void) {
    CONTROLS_START();
#if defined(C2_HOOKS_ENABLED)
    ToggleSoundEnable_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00455a50, ToggleSoundEnable, ToggleSoundEnable_original)

// Key: 'shift+d'
void (C2_HOOK_FASTCALL * WriteScreenShotBMP_original)(void);
void C2_HOOK_FASTCALL WriteScreenShotBMP(void) {
    CONTROLS_START();
#if defined(C2_HOOKS_ENABLED)
    WriteScreenShotBMP_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00518780, WriteScreenShotBMP, WriteScreenShotBMP_original)

// Key: 'f1'
void (C2_HOOK_FASTCALL * ToggleHUD_original)(void);
void C2_HOOK_FASTCALL ToggleHUD(void) {
    CONTROLS_START();
#if defined(C2_HOOKS_ENABLED)
    ToggleHUD_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00444f40, ToggleHUD, ToggleHUD_original)

// Key: 'f4'
void (C2_HOOK_FASTCALL * F4Key_original)(void);
void C2_HOOK_FASTCALL F4Key(void) {
    CONTROLS_START();
#if defined(C2_HOOKS_ENABLED)
    F4Key_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004414b0, F4Key, F4Key_original)

// Key: 'f5'
void (C2_HOOK_FASTCALL * F5Key_original)(void);
void C2_HOOK_FASTCALL F5Key(void) {
    CONTROLS_START();
#if defined(C2_HOOKS_ENABLED)
    F5Key_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00441600, F5Key, F5Key_original)

// Key: 'f6'
void (C2_HOOK_FASTCALL * F6Key_original)(void);
void C2_HOOK_FASTCALL F6Key(void) {
    CONTROLS_START();
#if defined(C2_HOOKS_ENABLED)
    F6Key_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00441680, F6Key, F6Key_original)

// Key: 'f7'
void (C2_HOOK_FASTCALL * F7Key_original)(void);
void C2_HOOK_FASTCALL F7Key(void) {
    CONTROLS_START();
#if defined(C2_HOOKS_ENABLED)
    F7Key_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00441700, F7Key, F7Key_original)

// Key: 'f8'
void (C2_HOOK_FASTCALL * F8Key_original)(void);
void C2_HOOK_FASTCALL F8Key(void) {
    CONTROLS_START();
#if defined(C2_HOOKS_ENABLED)
    F8Key_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00441780, F8Key, F8Key_original)

// Key: 'f10'
void (C2_HOOK_FASTCALL * F10Key_original)(void);
void C2_HOOK_FASTCALL F10Key(void) {
    CONTROLS_START();
#if defined(C2_HOOKS_ENABLED)
    F10Key_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00441800, F10Key, F10Key_original)

// Key: 'f11'
void (C2_HOOK_FASTCALL * F11Key_original)(void);
void C2_HOOK_FASTCALL F11Key(void) {
    CONTROLS_START();
#if defined(C2_HOOKS_ENABLED)
    F11Key_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00441880, F11Key, F11Key_original)

// Key: 'f12'
void (C2_HOOK_FASTCALL * F12Key_original)(void);
void C2_HOOK_FASTCALL F12Key(void) {
    CONTROLS_START();
#if defined(C2_HOOKS_ENABLED)
    F12Key_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00441900, F12Key, F12Key_original)

// Key: '0'
void (C2_HOOK_FASTCALL * NumberKey0_original)(void);
void C2_HOOK_FASTCALL NumberKey0(void) {
    CONTROLS_START();
#if defined(C2_HOOKS_ENABLED)
    NumberKey0_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00441910, NumberKey0, NumberKey0_original)

// Key: '1'
void (C2_HOOK_FASTCALL * NumberKey1_original)(void);
void C2_HOOK_FASTCALL NumberKey1(void) {
    CONTROLS_START();
#if defined(C2_HOOKS_ENABLED)
    NumberKey1_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00441990, NumberKey1, NumberKey1_original)

// Key: '2'
void (C2_HOOK_FASTCALL * NumberKey2_original)(void);
void C2_HOOK_FASTCALL NumberKey2(void) {
    CONTROLS_START();
#if defined(C2_HOOKS_ENABLED)
    NumberKey2_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00441a10, NumberKey2, NumberKey2_original)

// Key: '3'
void (C2_HOOK_FASTCALL * NumberKey3_original)(void);
void C2_HOOK_FASTCALL NumberKey3(void) {
    CONTROLS_START();
#if defined(C2_HOOKS_ENABLED)
    NumberKey3_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00441a90, NumberKey3, NumberKey3_original)

// Key: '4'
void (C2_HOOK_FASTCALL * NumberKey4_original)(void);
void C2_HOOK_FASTCALL NumberKey4(void) {
    CONTROLS_START();
#if defined(C2_HOOKS_ENABLED)
    NumberKey4_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00441b10, NumberKey4, NumberKey4_original)

// Key: '5'
void (C2_HOOK_FASTCALL * NumberKey5_original)(void);
void C2_HOOK_FASTCALL NumberKey5(void) {
    CONTROLS_START();
#if defined(C2_HOOKS_ENABLED)
    NumberKey5_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00441b90, NumberKey5, NumberKey5_original)

// Key: '6'
void (C2_HOOK_FASTCALL * NumberKey6_original)(void);
void C2_HOOK_FASTCALL NumberKey6(void) {
    CONTROLS_START();
#if defined(C2_HOOKS_ENABLED)
    NumberKey6_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00441c10, NumberKey6, NumberKey6_original)

// Key: '7'
void (C2_HOOK_FASTCALL * NumberKey7_original)(void);
void C2_HOOK_FASTCALL NumberKey7(void) {
    CONTROLS_START();
#if defined(C2_HOOKS_ENABLED)
    NumberKey7_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00441c90, NumberKey7, NumberKey7_original)

// Key: '8'
void (C2_HOOK_FASTCALL * NumberKey8_original)(void);
void C2_HOOK_FASTCALL NumberKey8(void) {
    CONTROLS_START();
#if defined(C2_HOOKS_ENABLED)
    NumberKey8_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00441d10, NumberKey8, NumberKey8_original)

// Key: '9'
void (C2_HOOK_FASTCALL * NumberKey9_original)(void);
void C2_HOOK_FASTCALL NumberKey9(void) {
    CONTROLS_START();
#if defined(C2_HOOKS_ENABLED)
    NumberKey9_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00441d90, NumberKey9, NumberKey9_original)

// Key: 'kp_1'
void (C2_HOOK_FASTCALL * ToggleFlying_original)(void);
void C2_HOOK_FASTCALL ToggleFlying(void) {
    CONTROLS_START();
#if defined(C2_HOOKS_ENABLED)
    ToggleFlying_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004443c0, ToggleFlying, ToggleFlying_original)

// Key: '-'
void (C2_HOOK_FASTCALL * ScreenSmaller_original)(void);
void C2_HOOK_FASTCALL ScreenSmaller(void) {
    CONTROLS_START();
#if defined(C2_HOOKS_ENABLED)
    ScreenSmaller_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004e4cd0, ScreenSmaller, ScreenSmaller_original)

// Key: '='
void (C2_HOOK_FASTCALL * ScreenLarger_original)(void);
void C2_HOOK_FASTCALL ScreenLarger(void) {
    CONTROLS_START();
#if defined(C2_HOOKS_ENABLED)
    ScreenLarger_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004e4d00, ScreenLarger, ScreenLarger_original)

// Key: 'Del'
void (C2_HOOK_FASTCALL * BuyArmour_original)(void);
void C2_HOOK_FASTCALL BuyArmour(void) {
    CONTROLS_START();
#if defined(C2_HOOKS_ENABLED)
    BuyArmour_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00502e50, BuyArmour, BuyArmour_original)

// Key: 'End'
void (C2_HOOK_FASTCALL * BuyPower_original)(void);
void C2_HOOK_FASTCALL BuyPower(void) {
    CONTROLS_START();
#if defined(C2_HOOKS_ENABLED)
    BuyPower_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00502fd0, BuyPower, BuyPower_original)

// Key: 'PgDn'
void (C2_HOOK_FASTCALL * BuyOffense_original)(void);
void C2_HOOK_FASTCALL BuyOffense(void) {
    CONTROLS_START();
#if defined(C2_HOOKS_ENABLED)
    BuyOffense_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00503030, BuyOffense, BuyOffense_original)

// Key: ' '
void (C2_HOOK_FASTCALL * FUN0040e430_original)(void);
void C2_HOOK_FASTCALL FUN0040e430(void) {
    CONTROLS_START();
#if defined(C2_HOOKS_ENABLED)
    int* a = (int*)0x006792bc;
    c2_printf("*%p=0x%x\n", a, *a);
    int* b = (int*)0x006792bc;
    c2_printf("*%p=0x%x\n", b, *b);
    FUN0040e430_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0040e430, FUN0040e430, FUN0040e430_original)

// Key: '`'
void (C2_HOOK_FASTCALL * UserSendMessage_original)(void);
void C2_HOOK_FASTCALL UserSendMessage(void) {
    CONTROLS_START();
#if defined(C2_HOOKS_ENABLED)
    UserSendMessage_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004448f0, UserSendMessage, UserSendMessage_original)

// Key: 't'
void (C2_HOOK_FASTCALL * ToggleTargetLock_original)(void);
void C2_HOOK_FASTCALL ToggleTargetLock(void) {
    CONTROLS_START();
#if defined(C2_HOOKS_ENABLED)
    ToggleTargetLock_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004945f0, ToggleTargetLock, ToggleTargetLock_original)

// Key: 'y'
void (C2_HOOK_FASTCALL * ToggleTarget_original)(void);
void C2_HOOK_FASTCALL ToggleTarget(void) {
    CONTROLS_START();
#if defined(C2_HOOKS_ENABLED)
    ToggleTarget_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00494700, ToggleTarget, ToggleTarget_original)

// Key: ';'
void (C2_HOOK_FASTCALL * PowerupInventoryToggle_original)(void);
void C2_HOOK_FASTCALL PowerupInventoryToggle(void) {
    CONTROLS_START();
#if defined(C2_HOOKS_ENABLED)
    PowerupInventoryToggle_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004da9d0, PowerupInventoryToggle, PowerupInventoryToggle_original)

// Key: '['
void (C2_HOOK_FASTCALL * PowerupInventoryNext_original)(void);
void C2_HOOK_FASTCALL PowerupInventoryNext(void) {
    CONTROLS_START();
#if defined(C2_HOOKS_ENABLED)
    PowerupInventoryNext_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004dab80, PowerupInventoryNext, PowerupInventoryNext_original)

// Key: ']'
void (C2_HOOK_FASTCALL * PowerupInventoryPrevious_original)(void);
void C2_HOOK_FASTCALL PowerupInventoryPrevious(void) {
    CONTROLS_START();
#if defined(C2_HOOKS_ENABLED)
    PowerupInventoryPrevious_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004daa00, PowerupInventoryPrevious, PowerupInventoryPrevious_original)

// Key: '*' (keypad)
void (C2_HOOK_FASTCALL * ToggleCam_original)(void);
void C2_HOOK_FASTCALL ToggleCam(void) {
    CONTROLS_START();
#if defined(C2_HOOKS_ENABLED)
    ToggleCam_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00444ed0, ToggleCam, ToggleCam_original)

// Key: 'c'
void (C2_HOOK_FASTCALL * ToggleCockpit_original)(void);
void C2_HOOK_FASTCALL ToggleCockpit(void) {
    CONTROLS_START();
#if defined(C2_HOOKS_ENABLED)
    ToggleCockpit_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0040e900, ToggleCockpit, ToggleCockpit_original)

void C2_HOOK_FASTCALL DisposeAbuseomatic(void) {
    int i;

    for (i = 0; i < REC2_ASIZE(C2V(gAbuse_text)); i++) {
        if (C2V(gAbuse_text)[i] != NULL) {
            BrMemFree(C2V(gAbuse_text)[i]);
        }
    }
}
C2_HOOK_FUNCTION(0x00444ea0, DisposeAbuseomatic)
