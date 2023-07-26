#include "controls.h"

#include "globvrpb.h"
#include "input.h"
#include "network.h"
#include "sound.h"
#include "utility.h"

#include "brender/brender.h"

#include "c2_string.h"

#include "rec2_macros.h"

#define DEBUG_CONTROLS

#if defined(DEBUG_CONTROLS)
#include "c2_stdio.h"
#define CONTROLS_START() c2_fprintf(c2_stderr, "%s: start\n", __FUNCTION__); C2_HOOK_DEBUGF("")
#else
#define CONTROLS_START()
#endif

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(char*, gAbuse_text, 10, 0x0067c3c8);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(tCheat, gKev_keys, 94, 0x00590970, {
    { 0xa11ee75d, 0xa11ee75d, SetFlag,    0xa11ee75d },
    { 0x398da28c, 0x398da28c, SetFlag,    0x564e78b9 },
    { 0x7dc510f3, 0x7dc510f3, SetFlag2,   0x00000001 },
    { 0x309e4f55, 0x309e4f55, FinishRace, 0x00000000 },
    { 0x1bcbe148, 0x1bcbe148, GetPowerup, 0x00000000 },
    { 0x1d5e7725, 0x1d5e7725, GetPowerup, 0x00000001 },
    { 0x22c65063, 0x22c65063, GetPowerup, 0x00000002 },
    { 0x1a37d28a, 0x1a37d28a, GetPowerup, 0x00000003 },
    { 0x1dcba360, 0x1dcba360, GetPowerup, 0x00000004 },
    { 0x24c99afb, 0x24c99afb, GetPowerup, 0x00000005 },
    { 0x200c1bd4, 0x200c1bd4, GetPowerup, 0x00000006 },
    { 0x252a2e6b, 0x252a2e6b, GetPowerup, 0x00000007 },
    { 0x218f555c, 0x218f555c, GetPowerup, 0x00000008 },
    { 0x1fc7655b, 0x1fc7655b, GetPowerup, 0x00000009 },
    { 0x2b2e6891, 0x2b2e6891, GetPowerup, 0x0000000a },
    { 0x2db8b34a, 0x2db8b34a, GetPowerup, 0x0000000b },
    { 0x3001467e, 0x3001467e, GetPowerup, 0x0000000c },
    { 0x23968eda, 0x23968eda, GetPowerup, 0x0000000d },
    { 0x1f3baa55, 0x1f3baa55, GetPowerup, 0x0000000f },
    { 0x214a2558, 0x214a2558, GetPowerup, 0x00000010 },
    { 0x373ae69a, 0x373ae69a, GetPowerup, 0x00000011 },
    { 0x327ebd75, 0x327ebd75, GetPowerup, 0x00000012 },
    { 0x350c0384, 0x350c0384, GetPowerup, 0x00000013 },
    { 0x17f03c24, 0x17f03c24, GetPowerup, 0x00000014 },
    { 0x32aeca21, 0x32aeca21, GetPowerup, 0x00000015 },
    { 0x191841aa, 0x191841aa, GetPowerup, 0x00000016 },
    { 0x26026896, 0x26026896, GetPowerup, 0x00000017 },
    { 0x2440ca1b, 0x2440ca1b, GetPowerup, 0x00000018 },
    { 0x37a11b1b, 0x37a11b1b, GetPowerup, 0x00000019 },
    { 0x2f2ea509, 0x2f2ea509, GetPowerup, 0x0000001a },
    { 0x28f522f1, 0x28f522f1, GetPowerup, 0x0000001b },
    { 0x26c15553, 0x26c15553, GetPowerup, 0x00000020 },
    { 0x3964b52b, 0x3964b52b, GetPowerup, 0x00000021 },
    { 0x18bf123a, 0x18bf123a, GetPowerup, 0x00000022 },
    { 0x2a439e13, 0x2a439e13, GetPowerup, 0x00000023 },
    { 0x28769902, 0x28769902, GetPowerup, 0x00000024 },
    { 0x2d5aa4e5, 0x2d5aa4e5, GetPowerup, 0x00000025 },
    { 0x1e73b354, 0x1e73b354, GetPowerup, 0x00000026 },
    { 0x1cac0a7c, 0x1cac0a7c, GetPowerup, 0x00000027 },
    { 0x1e3c613a, 0x1e3c613a, GetPowerup, 0x00000028 },
    { 0x2f4c3519, 0x2f4c3519, GetPowerup, 0x00000029 },
    { 0x21f0d261, 0x21f0d261, GetPowerup, 0x0000002a },
    { 0x1c727344, 0x1c727344, GetPowerup, 0x0000002b },
    { 0x2f574845, 0x2f574845, GetPowerup, 0x0000002c },
    { 0x1f0601e3, 0x1f0601e3, GetPowerup, 0x0000002d },
    { 0x26219ff3, 0x26219ff3, GetPowerup, 0x0000002f },
    { 0x26afbb31, 0x26afbb31, GetPowerup, 0x00000030 },
    { 0x25205546, 0x25205546, GetPowerup, 0x00000031 },
    { 0x1a0a8e5b, 0x1a0a8e5b, GetPowerup, 0x00000033 },
    { 0x1bdea925, 0x1bdea925, GetPowerup, 0x00000034 },
    { 0x2d4dd2a9, 0x2d4dd2a9, GetPowerup, 0x00000035 },
    { 0x2e7a7505, 0x2e7a7505, GetPowerup, 0x00000036 },
    { 0x17290940, 0x17290940, GetPowerup, 0x00000037 },
    { 0x1d4e7a9c, 0x1d4e7a9c, GetPowerup, 0x00000038 },
    { 0x3579d64a, 0x3579d64a, GetPowerup, 0x00000039 },
    { 0x28f4d49c, 0x28f4d49c, GetPowerup, 0x0000003a },
    { 0x1d6ba9c3, 0x1d6ba9c3, GetPowerup, 0x0000003b },
    { 0x310971ab, 0x310971ab, GetPowerup, 0x0000003c },
    { 0x28451eeb, 0x28451eeb, GetPowerup, 0x0000003d },
    { 0x1ebfa5ba, 0x1ebfa5ba, GetPowerup, 0x0000003e },
    { 0x27079773, 0x27079773, GetPowerup, 0x0000003f },
    { 0x388de72c, 0x388de72c, GetPowerup, 0x00000040 },
    { 0x1a0da9fc, 0x1a0da9fc, GetPowerup, 0x00000041 },
    { 0x2975a10c, 0x2975a10c, GetPowerup, 0x00000042 },
    { 0x1e5cc6ca, 0x1e5cc6ca, GetPowerup, 0x00000043 },
    { 0x250c6f99, 0x250c6f99, GetPowerup, 0x00000044 },
    { 0x33950e49, 0x33950e49, GetPowerup, 0x00000045 },
    { 0x4e5f487a, 0x4e5f487a, GetPowerup, 0x00000046 },
    { 0x3815584c, 0x3815584c, GetPowerup, 0x00000047 },
    { 0x459732b2, 0x459732b2, GetPowerup, 0x00000048 },
    { 0x62871003, 0x62871003, GetPowerup, 0x00000049 },
    { 0x2d72ebb4, 0x2d72ebb4, GetPowerup, 0x0000004a },
    { 0x2c3be2aa, 0x2c3be2aa, GetPowerup, 0x0000004b },
    { 0x2b2be28b, 0x2b2be28b, GetPowerup, 0x0000004c },
    { 0x29be089d, 0x29be089d, GetPowerup, 0x0000004d },
    { 0x4897982d, 0x4897982d, GetPowerup, 0x0000004e },
    { 0x44d50f49, 0x44d50f49, GetPowerup, 0x0000004f },
    { 0x403afae5, 0x403afae5, GetPowerup, 0x00000050 },
    { 0x45c19e5e, 0x45c19e5e, GetPowerup, 0x00000051 },
    { 0x2f790ebd, 0x2f790ebd, GetPowerup, 0x00000052 },
    { 0x244f60c9, 0x244f60c9, GetPowerup, 0x00000053 },
    { 0x2b0794d3, 0x2b0794d3, GetPowerup, 0x00000054 },
    { 0x2a44b628, 0x2a44b628, GetPowerup, 0x00000055 },
    { 0x2998e46d, 0x2998e46d, GetPowerup, 0x00000056 },
    { 0x23248728, 0x23248728, GetPowerup, 0x00000057 },
    { 0x289c1822, 0x289c1822, GetPowerup, 0x00000058 },
    { 0x35abb7d0, 0x35abb7d0, GetPowerup, 0x0000005a },
    { 0x1c1fdd92, 0x1c1fdd92, GetPowerup, 0x0000005b },
    { 0x253069c1, 0x253069c1, GetPowerup, 0x0000005c },
    { 0x33ca4873, 0x33ca4873, GetPowerup, 0x0000005d },
    { 0x1f56cde5, 0x1f56cde5, GetPowerup, 0x0000005e },
    { 0x1784995b, 0x1784995b, GetPowerup, 0x00000060 },
    { 0x3003eccb, 0x3003eccb, GetPowerup, 0x00000061 },
    { 0x4b054b60, 0x4b054b60, DoSteelGonadODeath, 0x00000000 },
    { 0x00000000, 0x00000000, NULL,       0x00000000 },
});

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

void C2_HOOK_FASTCALL CheckKevKeys(void) {
    int i;
    tU32* value;
    char s[128];

    value = KevKeyService();
    if (value[0] == 0) {
        return;
    }

    for (i = 0; C2V(gKev_keys)[i].action_proc != NULL; i++) {
        if (C2V(gKev_keys)[i].code == value[0] && C2V(gKev_keys)[i].code2 == value[1]) {
            break;
        }
    }

    if (C2V(gKev_keys)[i].action_proc != NULL) {
        if (C2V(gNet_mode) == eNet_mode_none) {
            C2V(gKev_keys)[i].action_proc(C2V(gKev_keys)[i].num);
        } else if (C2V(gKev_keys)[i].num == 0xa11ee75d) {
            c2_strcpy(s, C2V(gNet_players)[C2V(gThis_net_player_index)].player_name);
            c2_strcat(s, " ");
            c2_strcat(s, GetMiscString(225));
            NetSendHeadupToEverybody(s);
            C2V(gKev_keys)[i].action_proc(C2V(gKev_keys)[i].num);
        } else {
            c2_strcpy(s, C2V(gNet_players)[C2V(gThis_net_player_index)].player_name);
            c2_strcat(s, " ");
            c2_strcat(s, GetMiscString(224));
            NetSendHeadupToAllPlayers(s);
        }
    }
}
C2_HOOK_FUNCTION(0x00443c90, CheckKevKeys)
