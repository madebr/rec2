#include "controls.h"

#include "car.h"
#include "crush.h"
#include "displays.h"
#include "errors.h"
#include "finteray.h"
#include "globvars.h"
#include "globvrkm.h"
#include "globvrpb.h"
#include "graphics.h"
#include "init.h"
#include "input.h"
#include "loading.h"
#include "netgame.h"
#include "network.h"
#include "opponent.h"
#include "physics.h"
#include "polyfont.h"
#include "replay.h"
#include "sound.h"
#include "utility.h"

#include "platform.h"

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

C2_HOOK_VARIABLE_IMPLEMENT(int, gEntering_message, 0x0067c474);
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
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(tToggle_element, gToggle_array, 44, 0x005900a0, {
    { 0x23, -2, 1, 1, 0, ToggleMiniMap },
    { 0x4a, -2, 1, 1, 0, ToggleFlaps },
    { 0x39, -2, 1, 1, 0, ToggleMap },
    { 0x39, 8, 1, 1, 0, ToggleHeadupMap },
    { 0x39, 7, 1, 1, 0, CycleMapOptions },
    { 0x39, 9, 1, 1, 0, ToggleMapTransparency },
    { 0x3c, -2, 1, 1, 0, SetRecovery },
    { 0x04, 7, 1, 0, 0, AbortRace },
    { 0x25, -2, 0, 1, 0, ToggleSoundEnable },
    { 0x05, 8, 0, 1, 0, PrintScreen },
    { 0x4c, -2, 1, 0, 0, ToggleHeadupLevel },
    { 0x12, -2, 0, 0, 0, F4Key },
    { 0x13, -2, 1, 0, 0, F5Key },
    { 0x14, -2, 1, 0, 0, F6Key },
    { 0x15, -2, 1, 0, 0, F7Key },
    { 0x16, -2, 1, 0, 0, F8Key },
    { 0x17, -2, 1, 0, 0, F10Key },
    { 0x18, -2, 1, 0, 0, F11Key },
    { 0x19, -2, 1, 0, 0, F12Key },
    { 0x0f, -2, 1, 0, 0, NumberKey0 },
    { 0x26, -2, 1, 0, 0, NumberKey1 },
    { 0x27, -2, 1, 0, 0, NumberKey2 },
    { 0x28, -2, 1, 0, 0, NumberKey3 },
    { 0x29, -2, 1, 0, 0, NumberKey4 },
    { 0x2a, -2, 1, 0, 0, NumberKey5 },
    { 0x2b, -2, 1, 0, 0, NumberKey6 },
    { 0x2c, -2, 1, 0, 0, NumberKey7 },
    { 0x10, -2, 1, 0, 0, NumberKey8 },
    { 0x11, -2, 1, 0, 0, NumberKey9 },
    { 0x0b, 7, 1, 1, 0, ToggleFlying },
    { 0x41, -2, 1, 0, 0, ScreenSmaller },
    { 0x42, -2, 1, 0, 0, ScreenLarger },
    { 0x43, -2, 1, 0, 0, BuyArmour },
    { 0x44, -2, 1, 0, 0, BuyPower },
    { 0x45, -2, 1, 0, 0, BuyOffense },
    { 0x46, -2, 1, 0, 0, ViewNetPlayer },
    { 0x47, -2, 1, 0, 0, UserSendMessage },
    { 0x48, -2, 1, 0, 0, ToggleTargetLock },
    { 0x49, -2, 1, 0, 0, CycleTargetLock },
    { 0x3e, -2, 1, 0, 0, ToggleInventory },
    { 0x3f, -2, 1, 0, 0, LeftInventory },
    { 0x40, -2, 1, 0, 0, RightInventory },
    { 0x48, -2, 1, 0, 0, ChangeCameraTypeInGame },
    { 0x2e, -2, 1, 0, 0, ToggleCockpit },
});
C2_HOOK_VARIABLE_IMPLEMENT(int, gRecovery_voucher_count, 0x0067c3f8);
C2_HOOK_VARIABLE_IMPLEMENT(int, gAuto_repair, 0x0079ec54);
C2_HOOK_VARIABLE_IMPLEMENT(int, gInstant_handbrake, 0x0079ec50);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gToo_poor_for_recovery_timeout, 0x0067c47c);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gCheckpoint_finder_enabled, 0x00659b28);

C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_0068b8e4, 0x0068b8e4);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_0068b8e8, 0x0068b8e8);
C2_HOOK_VARIABLE_IMPLEMENT(int, gHad_auto_recover, 0x0067c460);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gPalette_fade_time, 0x0067c478);
C2_HOOK_VARIABLE_IMPLEMENT(int, gRecover_timer, 0x0067c3bc);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_00590f60, 0x00590f60);
C2_HOOK_VARIABLE_IMPLEMENT(int, gToo_late, 0x0067c3b8);
C2_HOOK_VARIABLE_IMPLEMENT(int, gINT_0067c470, 0x0067c470);
C2_HOOK_VARIABLE_IMPLEMENT(int, gWhich_edit_mode, 0x0067c468); /* FIXME: enum tEdit_mode */
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_ADV_INIT(tEdit_func*, gEdit_funcs, [2][18][8], 0x005904f0, {
    {
        {
            NULL, /* [0][ 0][0] */
            NULL, /* [0][ 0][1] */
            NULL, /* [0][ 0][2] */
            NULL, /* [0][ 0][3] */
            NULL, /* [0][ 0][4] */
            NULL, /* [0][ 0][5] */
            NULL, /* [0][ 0][6] */
            NULL, /* [0][ 0][7] */
        },
        {
            NULL, /* [0][ 1][0] */
            NULL, /* [0][ 1][1] */
            NULL, /* [0][ 1][2] */
            NULL, /* [0][ 1][3] */
            NULL, /* [0][ 1][4] */
            NULL, /* [0][ 1][5] */
            NULL, /* [0][ 1][6] */
            NULL, /* [0][ 1][7] */
        },
        {
            NULL, /* [0][ 2][0] */
            NULL, /* [0][ 2][1] */
            NULL, /* [0][ 2][2] */
            NULL, /* [0][ 2][3] */
            NULL, /* [0][ 2][4] */
            NULL, /* [0][ 2][5] */
            NULL, /* [0][ 2][6] */
            NULL, /* [0][ 2][7] */
        },
        {
            NULL, /* [0][ 3][0] */
            NULL, /* [0][ 3][1] */
            NULL, /* [0][ 3][2] */
            NULL, /* [0][ 3][3] */
            NULL, /* [0][ 3][4] */
            NULL, /* [0][ 3][5] */
            NULL, /* [0][ 3][6] */
            NULL, /* [0][ 3][7] */
        },
        {
            NULL, /* [0][ 4][0] */
            NULL, /* [0][ 4][1] */
            NULL, /* [0][ 4][2] */
            NULL, /* [0][ 4][3] */
            NULL, /* [0][ 4][4] */
            NULL, /* [0][ 4][5] */
            NULL, /* [0][ 4][6] */
            NULL, /* [0][ 4][7] */
        },
        {
            NULL, /* [0][ 5][0] */
            NULL, /* [0][ 5][1] */
            NULL, /* [0][ 5][2] */
            NULL, /* [0][ 5][3] */
            NULL, /* [0][ 5][4] */
            NULL, /* [0][ 5][5] */
            NULL, /* [0][ 5][6] */
            NULL, /* [0][ 5][7] */
        },
        {
            NULL, /* [0][ 6][0] */
            NULL, /* [0][ 6][1] */
            NULL, /* [0][ 6][2] */
            NULL, /* [0][ 6][3] */
            NULL, /* [0][ 6][4] */
            NULL, /* [0][ 6][5] */
            NULL, /* [0][ 6][6] */
            NULL, /* [0][ 6][7] */
        },
        {
            NULL, /* [0][ 7][0] */
            NULL, /* [0][ 7][1] */
            NULL, /* [0][ 7][2] */
            NULL, /* [0][ 7][3] */
            NULL, /* [0][ 7][4] */
            NULL, /* [0][ 7][5] */
            NULL, /* [0][ 7][6] */
            NULL, /* [0][ 7][7] */
        },
        {
            ToggleCarSimplification, /* [0][ 8][0] */
            NULL, /* [0][ 8][1] */
            NULL, /* [0][ 8][2] */
            NULL, /* [0][ 8][3] */
            NULL, /* [0][ 8][4] */
            NULL, /* [0][ 8][5] */
            NULL, /* [0][ 8][6] */
            NULL, /* [0][ 8][7] */
        },
        {
            ToggleShadow, /* [0][ 9][0] */
            NULL, /* [0][ 9][1] */
            NULL, /* [0][ 9][2] */
            NULL, /* [0][ 9][3] */
            NULL, /* [0][ 9][4] */
            NULL, /* [0][ 9][5] */
            NULL, /* [0][ 9][6] */
            NULL, /* [0][ 9][7] */
        },
        {
            NULL, /* [0][10][0] */
            NULL, /* [0][10][1] */
            NULL, /* [0][10][2] */
            NULL, /* [0][10][3] */
            NULL, /* [0][10][4] */
            NULL, /* [0][10][5] */
            NULL, /* [0][10][6] */
            NULL, /* [0][10][7] */
        }
        {
            ToggleSky, /* [0][11][0] */
            NULL, /* [0][11][1] */
            NULL, /* [0][11][2] */
            NULL, /* [0][11][3] */
            ToggleDepthCueing, /* [0][11][4] */
            NULL, /* [0][11][5] */
            NULL, /* [0][11][6] */
            NULL, /* [0][11][7] */
        },
        {
            CycleYonFactor, /* [0][12][0] */
            NULL, /* [0][12][1] */
            NULL, /* [0][12][2] */
            NULL, /* [0][12][3] */
            ToggleAccessories, /* [0][12][4] */
            NULL, /* [0][12][5] */
            NULL, /* [0][12][6] */
            NULL, /* [0][12][7] */
        },
        {
            DecreaseYon, /* [0][13][0] */
            NULL, /* [0][13][1] */
            NULL, /* [0][13][2] */
            NULL, /* [0][13][3] */
            IncreaseYon, /* [0][13][4] */
            NULL, /* [0][13][5] */
            NULL, /* [0][13][6] */
            NULL, /* [0][13][7] */
        },
        {
            CycleSoundDetailLevel, /* [0][14][0] */
            NULL, /* [0][14][1] */
            NULL, /* [0][14][2] */
            NULL, /* [0][14][3] */
            NULL, /* [0][14][4] */
            NULL, /* [0][14][5] */
            NULL, /* [0][14][6] */
            NULL, /* [0][14][7] */
        },
        {
            FUN_004599f0, /* [0][15][0] */
            FUN_00511240, /* [0][15][1] */
            FUN_00511280, /* [0][15][2] */
            FUN_00511330, /* [0][15][3] */
            FUN_00511270, /* [0][15][4] */
            FUN_005112d0, /* [0][15][5] */
            FUN_00511390, /* [0][15][6] */
            NULL, /* [0][15][7] */
        },
        {
            NULL, /* [0][16][0] */
            NULL, /* [0][16][1] */
            NULL, /* [0][16][2] */
            NULL, /* [0][16][3] */
            NULL, /* [0][16][4] */
            NULL, /* [0][16][5] */
            NULL, /* [0][16][6] */
            NULL, /* [0][16][7] */
        },
        {
            NULL, /* [0][17][0] */
            NULL, /* [0][17][1] */
            NULL, /* [0][17][2] */
            NULL, /* [0][17][3] */
            NULL, /* [0][17][4] */
            NULL, /* [0][17][5] */
            NULL, /* [0][17][6] */
            NULL, /* [0][17][7] */
        },
    },
    {
        {
            TotalRepair, /* [1][ 0][0] */
            MaxOutAPO, /* [1][ 0][1] */
            NULL, /* [1][ 0][2] */
            NULL, /* [1][ 0][3] */
            FUN_00447330, /* [1][ 0][4] */
            FUN_00447340, /* [1][ 0][5] */
            NULL, /* [1][ 0][6] */
            NULL, /* [1][ 0][7] */
        },
        {
            CycleInvulnerability, /* [1][ 1][0] */
            NULL, /* [1][ 1][1] */
            NULL, /* [1][ 1][2] */
            NULL, /* [1][ 1][3] */
            FUN_004b5ab0, /* [1][ 1][4] */
            NULL, /* [1][ 1][5] */
            NULL, /* [1][ 1][6] */
            NULL, /* [1][ 1][7] */
        },
        {
            NextPedCam, /* [1][ 2][0] */
            FindNearestPed, /* [1][ 2][1] */
            NULL, /* [1][ 2][2] */
            NULL, /* [1][ 2][3] */
            PrevPedCam, /* [1][ 2][4] */
            FindNearestPed, /* [1][ 2][5] */
            NULL, /* [1][ 2][6] */
            NULL, /* [1][ 2][7] */
        },
        {
            ToggleTimerFreeze, /* [1][ 3][0] */
            ShadowMode, /* [1][ 3][1] */
            NULL, /* [1][ 3][2] */
            NULL, /* [1][ 3][3] */
            ToggleShadow, /* [1][ 3][4] */
            NULL, /* [1][ 3][5] */
            NULL, /* [1][ 3][6] */
            NULL, /* [1][ 3][7] */
        },
        {
            IncrementLap, /* [1][ 4][0] */
            NULL, /* [1][ 4][1] */
            NULL, /* [1][ 4][2] */
            NULL, /* [1][ 4][3] */
            IncrementCheckpoint, /* [1][ 4][4] */
            NULL, /* [1][ 4][5] */
            NULL, /* [1][ 4][6] */
            NULL, /* [1][ 4][7] */
        },
        {
            EarnDosh, /* [1][ 5][0] */
            NULL, /* [1][ 5][1] */
            NULL, /* [1][ 5][2] */
            NULL, /* [1][ 5][3] */
            LoseDosh, /* [1][ 5][4] */
            NULL, /* [1][ 5][5] */
            NULL, /* [1][ 5][6] */
            NULL, /* [1][ 5][7] */
        },
        {
            ViewOpponent, /* [1][ 6][0] */
            ViewOpponent, /* [1][ 6][1] */
            ViewOpponent, /* [1][ 6][2] */
            ViewOpponent, /* [1][ 6][3] */
            ViewOpponent, /* [1][ 6][4] */
            ViewOpponent, /* [1][ 6][5] */
            ViewOpponent, /* [1][ 6][6] */
            ViewOpponent, /* [1][ 6][7] */
        },
        {
            GotPowerup0, /* [1][ 7][0] */
            GotPowerup0, /* [1][ 7][1] */
            GotPowerup0, /* [1][ 7][2] */
            GotPowerup0, /* [1][ 7][3] */
            GotPowerup0, /* [1][ 7][4] */
            GotPowerup0, /* [1][ 7][5] */
            GotPowerup0, /* [1][ 7][6] */
            GotPowerup0, /* [1][ 7][7] */
        },
        {
            GotPowerup1, /* [1][ 8][0] */
            GotPowerup1, /* [1][ 8][1] */
            GotPowerup1, /* [1][ 8][2] */
            GotPowerup1, /* [1][ 8][3] */
            GotPowerup1, /* [1][ 8][4] */
            GotPowerup1, /* [1][ 8][5] */
            GotPowerup1, /* [1][ 8][6] */
            GotPowerup1, /* [1][ 8][7] */
        },
        {
            GotPowerup2, /* [1][ 9][0] */
            GotPowerup2, /* [1][ 9][1] */
            GotPowerup2, /* [1][ 9][2] */
            GotPowerup2, /* [1][ 9][3] */
            GotPowerup2, /* [1][ 9][4] */
            GotPowerup2, /* [1][ 9][5] */
            GotPowerup2, /* [1][ 9][6] */
            GotPowerup2, /* [1][ 9][7] */
        },
        {
            GotPowerup3, /* [1][10][0] */
            GotPowerup3, /* [1][10][1] */
            GotPowerup3, /* [1][10][2] */
            GotPowerup3, /* [1][10][3] */
            GotPowerup3, /* [1][10][4] */
            GotPowerup3, /* [1][10][5] */
            GotPowerup3, /* [1][10][6] */
            GotPowerup3, /* [1][10][7] */
        }
        {
            GotPowerup4, /* [1][11][0] */
            GotPowerup4, /* [1][11][1] */
            GotPowerup4, /* [1][11][2] */
            GotPowerup4, /* [1][11][3] */
            GotPowerup4, /* [1][11][4] */
            GotPowerup4, /* [1][11][5] */
            GotPowerup4, /* [1][11][6] */
            GotPowerup4, /* [1][11][7] */
        },
        {
            GotPowerup5, /* [1][12][0] */
            GotPowerup5, /* [1][12][1] */
            GotPowerup5, /* [1][12][2] */
            GotPowerup5, /* [1][12][3] */
            GotPowerup5, /* [1][12][4] */
            GotPowerup5, /* [1][12][5] */
            GotPowerup5, /* [1][12][6] */
            GotPowerup5, /* [1][12][7] */
        },
        {
            GotPowerup6, /* [1][13][0] */
            GotPowerup6, /* [1][13][1] */
            GotPowerup6, /* [1][13][2] */
            GotPowerup6, /* [1][13][3] */
            GotPowerup6, /* [1][13][4] */
            GotPowerup6, /* [1][13][5] */
            GotPowerup6, /* [1][13][6] */
            GotPowerup6, /* [1][13][7] */
        },
        {
            GotPowerup7, /* [1][14][0] */
            GotPowerup7, /* [1][14][1] */
            GotPowerup7, /* [1][14][2] */
            GotPowerup7, /* [1][14][3] */
            GotPowerup7, /* [1][14][4] */
            GotPowerup7, /* [1][14][5] */
            GotPowerup7, /* [1][14][6] */
            GotPowerup7, /* [1][14][7] */
        },
        {
            GotPowerup8, /* [1][15][0] */
            GotPowerup8, /* [1][15][1] */
            GotPowerup8, /* [1][15][2] */
            GotPowerup8, /* [1][15][3] */
            GotPowerup8, /* [1][15][4] */
            GotPowerup8, /* [1][15][5] */
            GotPowerup8, /* [1][15][6] */
            GotPowerup8, /* [1][15][7] */
        },
        {
            GotPowerup9, /* [1][16][0] */
            GotPowerup9, /* [1][16][1] */
            GotPowerup9, /* [1][16][2] */
            GotPowerup9, /* [1][16][3] */
            GotPowerup9, /* [1][16][4] */
            GotPowerup9, /* [1][16][5] */
            GotPowerup9, /* [1][16][6] */
            GotPowerup9, /* [1][16][7] */
        },
        {
            NULL, /* [1][17][0] */
            NULL, /* [1][17][1] */
            NULL, /* [1][17][2] */
            NULL, /* [1][17][3] */
            NULL, /* [1][17][4] */
            NULL, /* [1][17][5] */
            NULL, /* [1][17][6] */
            NULL, /* [1][17][7] */
        },
    },
});
C2_HOOK_VARIABLE_IMPLEMENT(int, gAllow_car_flying, 0x0067c46c);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const char *, gEdit_mode_names, 12, 0x005904c0, {
    "Options",
    "Cheat",
    "Accessories",
    "Powerups",
    "Special Volumes",
    "Pedestrians",
    "Drones",
    "Opponents",
    "Prat-cam",
    "Depth effects",
    "Damage",
    "Crush Test",
});
C2_HOOK_VARIABLE_IMPLEMENT(tCar_spec*, gTarget_lock_car_1, 0x0068d8c4);
C2_HOOK_VARIABLE_IMPLEMENT(tCar_spec*, gTarget_lock_car_2, 0x0068d6f0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gInventory_cycling, 0x006a0940);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gInventory_timeout, 0x006a0954);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(char, gString, 84, 0x0067c400);


void C2_HOOK_FASTCALL SetSoundDetailLevel(int pLevel) {

    C2V(gSound_detail_level) = pLevel;
}
C2_HOOK_FUNCTION(0x00456950, SetSoundDetailLevel)

void (C2_HOOK_FASTCALL * ReallySetSoundDetailLevel_original)(int pLevel);
void C2_HOOK_FASTCALL ReallySetSoundDetailLevel(int pLevel) {

#if 0//defined(C2_HOOKS_ENABLED)
    ReallySetSoundDetailLevel_original(pLevel);
#else
    DRS3StopAllOutletSoundsExceptCDA();
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
#if 0//defined(C2_HOOKS_ENABLED)
    ToggleMiniMap_original();
#else
    if (C2V(gMap_view) == 2) {
        return;
    }
    C2V(gMini_map_visible) = !C2V(gMini_map_visible);
    NewTextHeadupSlot(4, 0, 500, -4, GetMiscString(C2V(gMini_map_visible) ? eMiscString_minimap_on : eMiscString_minimap_off));
    UpdateMapAndSaveOptions();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004420e0, ToggleMiniMap, ToggleMiniMap_original)

// Key: 'Enter'
void (C2_HOOK_FASTCALL * ToggleFlaps_original)(void);
void C2_HOOK_FASTCALL ToggleFlaps(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    ToggleFlaps_original();
#else
    if (C2V(gCountdown) || C2V(gToo_poor_for_recovery_timeout) != 0) {
        return;
    }
    if (C2V(gNet_mode) == eNet_mode_none || C2V(gNet_mode) == eNet_mode_host) {
        PhysicsObjectRecurse(C2V(gProgram_state).current_car.collision_info,
            ToggleFlapsCB,
            &C2V(gProgram_state).current_car);
    } else {
        tNet_message_chunk* chunk = NetAllocateMessageChunk(eNet_message_chunk_type_toggle_doors, 0);
        tNet_game_player_info *player_info = NetPlayerFromCar(&C2V(gProgram_state).current_car);
        if (player_info != NULL) {
            chunk->toggle_doors.id = player_info->ID;
        }
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0042dd50, ToggleFlaps, ToggleFlaps_original)

int C2_HOOK_FASTCALL ToggleFlapsCB(tCollision_info* pCollision_info, void* data) {
    tCar_spec *pCar_spec = data;

    DRActorEnumRecurse(pCollision_info->actor, ToggleDoorsActorCallback, pCar_spec);
    return 0;
}
C2_HOOK_FUNCTION(0x0042ddb0, ToggleFlapsCB)

C2_HOOK_VARIABLE_IMPLEMENT(int, gCount_toggled_doors, 0x0067bd58);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tToggled_door, gToggled_doors, 16, 0x00679450);

int (C2_HOOK_CDECL * ToggleDoorsActorCallback_original)(br_actor* pActor, void* data);
int C2_HOOK_CDECL ToggleDoorsActorCallback(br_actor* pActor, void* data) {

#if 0//defined(C2_HOOKS_ENABLED)
    return ToggleDoorsActorCallback_original(pActor, data);
#else
    tCar_spec* pCar = data;
    tUser_crush_data* user_data;
    tCar_crush_buffer_entry* crush_buffer;
    int i;

    user_data = pActor->user;
    if (user_data == NULL) {
        return 0;
    }
    crush_buffer = user_data->crush_data;
    if (crush_buffer == NULL) {
        return 0;
    }
    if (crush_buffer->flap_data == NULL || !crush_buffer->flap_data->kev_o_flap) {
        return 0;
    }

    C2_HOOK_BUG_ON(REC2_ASIZE(C2V(gToggled_doors)) != 16);

    if (crush_buffer->flap_data->field_0x0) {
        if (C2V(gCount_toggled_doors) != REC2_ASIZE(C2V(gToggled_doors))) {
            for (i = 0; i < C2V(gCount_toggled_doors); i++) {
                if (C2V(gToggled_doors)[i].actor == pActor) {
                    return 0;
                }
            }
            C2V(gToggled_doors)[C2V(gCount_toggled_doors)].actor = pActor;
            C2V(gToggled_doors)[C2V(gCount_toggled_doors)].car = pCar;
            C2V(gToggled_doors)[C2V(gCount_toggled_doors)].field_0x8 = 0;
            C2V(gToggled_doors)[C2V(gCount_toggled_doors)].field_0xc = 0.f;
            C2V(gCount_toggled_doors) += 1;
        }
    } else {
        if (C2V(gCount_toggled_doors) != REC2_ASIZE(C2V(gToggled_doors))) {
            for (i = 0; i < C2V(gCount_toggled_doors); i++) {
                if (C2V(gToggled_doors)[i].actor == pActor) {
                    return 0;
                }
            }
            C2V(gToggled_doors)[C2V(gCount_toggled_doors)].actor = pActor;
            C2V(gToggled_doors)[C2V(gCount_toggled_doors)].car = pCar;
            C2V(gToggled_doors)[C2V(gCount_toggled_doors)].field_0x8 = 1;
            C2V(gToggled_doors)[C2V(gCount_toggled_doors)].field_0xc = 20.f;
            C2V(gCount_toggled_doors) += 1;
        }
    }
    return 0;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0042ddc0, ToggleDoorsActorCallback, ToggleDoorsActorCallback_original)

void C2_HOOK_FASTCALL ToggleMap2(void) {
    static C2_HOOK_VARIABLE_IMPLEMENT(int, old_indent, 0x0067c458);
    int original_indent;

    original_indent = C2V(gRender_indent);
    if (C2V(gMap_view) == 2) {
        C2V(gMap_view) = 1;
        C2V(gRender_indent) = C2V(old_indent);
    } else {
        if (C2V(gAction_replay_mode)) {
            return;
        }
        if (C2V(gNet_mode) != eNet_mode_none && C2V(gCurrent_net_game)->type == eNet_game_type_foxy && C2V(gThis_net_player_index) == C2V(gIt_or_fox)) {
            NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_fox_cant_do_that));
            return;
        }
        C2V(gRender_indent) = 0;
        C2V(old_indent) = original_indent;
        C2V(gMap_view) = 2;
        C2V(gMap_time) = PDGetTotalTime();
    }
    if (C2V(gCurrent_race).map_image != NULL) {
        // nop_FUN005191f0(C2V(gCurrent_race).map_image);
    }
}

// Key: 'tab'
void (C2_HOOK_FASTCALL * ToggleMap_original)(void);
void C2_HOOK_FASTCALL ToggleMap(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    ToggleMap_original();
#else
    ToggleMap2();
    AdjustRenderScreenSize();
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
void (C2_HOOK_FASTCALL * CycleMapOptions_original)(void);
void C2_HOOK_FASTCALL CycleMapOptions(void) {
    CONTROLS_START();
#if 0//defined(C2_HOOKS_ENABLED)
    CycleMapOptions_original();
#else

    C2V(gCheckpoint_finder_enabled) = !C2V(gCheckpoint_finder_enabled);
    NewTextHeadupSlot2(4, 0, 1500, -4, GetMiscString(C2V(gCheckpoint_finder_enabled) ? eMiscString_checkpoint_finder_turned_on : eMiscString_checkpoint_finder_turned_off), 0);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00494840, CycleMapOptions, CycleMapOptions_original)

// key: 'alt+tab' ('alt' in map mode)
void (C2_HOOK_FASTCALL * ToggleMapTransparency_original)(void);
void C2_HOOK_FASTCALL ToggleMapTransparency(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    ToggleMapTransparency_original();
#else
    C2V(gMap_trans) = !C2V(gMap_trans);
    SaveOptions();
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00494880, ToggleMapTransparency, ToggleMapTransparency_original)

// Key: 'r'
void (C2_HOOK_FASTCALL * SetRecovery_original)(void);
void C2_HOOK_FASTCALL SetRecovery(void) {
    CONTROLS_START();
#if 0//defined(C2_HOOKS_ENABLED)
    SetRecovery_original();
#else
    if (C2V(gINT_0068b8e4)
        || C2V(gINT_0068b8e8)
        || C2V(gRace_finished)
        || C2V(gProgram_state).current_car.knackered
        || C2V(gWait_for_it)
        || C2V(gHad_auto_recover)
        || C2V(gPalette_fade_time) != 0) {
        return;
    }
    if (C2V(gNet_mode) == eNet_mode_none) {
        C2V(gRecover_car) = 1;
        C2V(gRecover_timer) = 0;
    } else if (C2V(gProgram_state).current_car.time_to_recover == 0) {
        C2V(gINT_00590f60) = 1;
        if (CheckRecoverCost()) {
            if (C2V(gCurrent_net_game)->type == eNet_game_type_foxy) {
                if (C2V(gThis_net_player_index) == C2V(gIt_or_fox)) {
                    C2V(gToo_late) = 0;
                    C2V(gProgram_state).current_car.time_to_recover = GetRaceTime() + 5000;
                } else {
                    C2V(gToo_late) = 0;
                    C2V(gProgram_state).current_car.time_to_recover = GetRaceTime() + 1000;
                }
            } else {
                if (C2V(gCurrent_net_game)->type == 6 &&
                    C2V(gNet_players)[C2V(gThis_net_player_index)].field_0x80 != 0) {
                    C2V(gToo_late) = 0;
                    C2V(gProgram_state).current_car.time_to_recover = GetRaceTime() + 500;
                } else {
                    C2V(gToo_late) = 0;
                    C2V(gProgram_state).current_car.time_to_recover = GetRaceTime() + 3000;
                }
            }
        }
    }
    else if (C2V(gINT_00590f60)) {
        if (GetRaceTime() + 600 < C2V(gProgram_state).current_car.time_to_recover) {
            C2V(gProgram_state).current_car.time_to_recover = 0;
            if (!C2V(gINT_0067c470)) {
                float cost = (C2V(gNet_mode) == eNet_mode_none) ? C2V(gRecovery_cost).initial[C2V(gProgram_state).skill_level] : C2V(gRecovery_cost).initial_network[C2V(gCurrent_net_game)->type];
                EarnCredits((int)cost);
            }
            NewTextHeadupSlot2(4, 0, 2000, -4, GetMiscString(eMiscString_recovery_cancelled), 0);
        } else {
            NewTextHeadupSlot2(4, 0, 2000, -4, GetMiscString(eMiscString_too_late_to_cancel), 1);
            C2V(gToo_late) = 1;
        }
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00442300, SetRecovery, SetRecovery_original)

int (C2_HOOK_FASTCALL * CheckRecoverCost_original)(void);
int C2_HOOK_FASTCALL CheckRecoverCost(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    return CheckRecoverCost_original();
#else
    int recovery_cost;

    if (C2V(gNet_mode) == eNet_mode_none) {
        return 1;
    }

    if (C2V(gProgram_state).current_car.knackered
        || C2V(gRecovery_voucher_count) != 0
        || (NetPlayerFromCar(&C2V(gProgram_state).current_car) != NULL && NetPlayerFromCar(&C2V(gProgram_state).current_car)->field_0x80 != 0)) {

        C2V(gINT_0067c470) = 1;
        return 1;
    }

    C2V(gINT_0067c470) = 0;
    recovery_cost = (int)(C2V(gNet_mode) == eNet_mode_none ? C2V(gRecovery_cost).initial[C2V(gProgram_state).skill_level] : C2V(gRecovery_cost).initial[C2V(gCurrent_net_game)->type]);

    if (recovery_cost > C2V(gProgram_state).credits) {
        int remaining_cost = recovery_cost;
        while (remaining_cost > 0
                && C2V(gProgram_state).current_car.power_up_levels[0] != 0
                && C2V(gProgram_state).current_car.power_up_levels[1] != 0
                && C2V(gProgram_state).current_car.power_up_levels[2] != 0) {
            int which;

            which = IRandomBetween(0, 2);
            if (C2V(gProgram_state).current_car.power_up_levels[which] != 0) {
                int part_value;

                C2V(gProgram_state).current_car.power_up_levels[which] -= 1;
                part_value = (C2V(gNet_mode) == eNet_mode_none ? C2V(gTrade_in_value_APO).initial[C2V(gProgram_state).skill_level] : C2V(gTrade_in_value_APO).initial[C2V(gCurrent_net_game)->type]);
                remaining_cost -= part_value;
                C2V(gProgram_state).credits += part_value;
            }
        }
    }
    if (recovery_cost > C2V(gProgram_state).credits) {
        C2V(gToo_poor_for_recovery_timeout) = PDGetTotalTime() + 2000;
        NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_press_return_to_commit_suicide));
        return 0;
    }
    SpendCredits(recovery_cost);
    return 1;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00442750, CheckRecoverCost, CheckRecoverCost_original)

// Key: 'ctrl+a'
void (C2_HOOK_FASTCALL * AbortRace_original)(void);
void C2_HOOK_FASTCALL AbortRace(void) {
    CONTROLS_START();
#if 0//defined(C2_HOOKS_ENABLED)
    AbortRace_original();
#else

    if (!C2V(gRace_finished)) {
        C2V(gAbandon_game) = 1;
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00441490, AbortRace, AbortRace_original)

void (C2_HOOK_FASTCALL * ToggleHeadupLevel_original)(void);
void C2_HOOK_FASTCALL ToggleHeadupLevel(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    ToggleHeadupLevel_original();
#else

    if (PDKeyDown(0)) {
        C2V(gHeadup_detail_level) -= 1;
        if (C2V(gHeadup_detail_level) < 0) {
            C2V(gHeadup_detail_level) = kMax_headup_detail_level;
        }
    } else {
        C2V(gHeadup_detail_level) += 1;
        if (C2V(gHeadup_detail_level) > kMax_headup_detail_level) {
            C2V(gHeadup_detail_level) = 0;
        }
    }
    NewTextHeadupSlot2(4, 0, 250, -4, GetMiscString(eMiscString_no_HUD + (C2V(gHeadup_detail_level) % 3)), 0);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00444f40, ToggleHeadupLevel, ToggleHeadupLevel_original)

void C2_HOOK_FASTCALL SetFlag(tU32 i) {
    int match;

    match = C2V(gI_am_cheating) == i;
    C2V(gI_am_cheating) = i;
    if (match) {
        C2V(gI_am_cheating) = 0;
        NewTextHeadupSlot(4, 0, 3000, -4, "Cheating Off");
    }
    F4Key();
}
C2_HOOK_FUNCTION(0x00441580, SetFlag)

// Key: 'f4'
void (C2_HOOK_FASTCALL * F4Key_original)(void);
void C2_HOOK_FASTCALL F4Key(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    F4Key_original();
#else
    if (C2V(gI_am_cheating) == 0xa11ee75d || (C2V(gI_am_cheating) != 0x564e78b9 && C2V(gNet_mode) != eNet_mode_none)) {
        char s[256];

        if (C2V(gINT_0068b8e4) == 1) {
            FUN_0045a670();
        }
        if (C2V(gINT_0068b8e4) == 2) {
            FUN_0045acf0();
        }
        if (C2V(gINT_0068b8e8)) {
            FUN_00511240();
        }
        if (PDKeyDown(0)) {
            C2V(gWhich_edit_mode) -= 1;
            if (C2V(gWhich_edit_mode) < 0) {
                C2V(gWhich_edit_mode) = REC2_ASIZE(C2V(gEdit_funcs)) - 1;
            }
        } else {
            C2V(gWhich_edit_mode) += 1;
            if (C2V(gWhich_edit_mode) >= REC2_ASIZE(C2V(gEdit_funcs))) {
                C2V(gWhich_edit_mode) = 0;
            }
        }
        c2_sprintf(s, "Edit mode: %s", C2V(gEdit_mode_names)[C2V(gWhich_edit_mode)]);
        NewTextHeadupSlot2(4, 0, 2000, -4, s, 0);
    } else {
        C2V(gWhich_edit_mode) = 0;
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004414b0, F4Key, F4Key_original)

void C2_HOOK_FASTCALL EnsureSpecialVolumesHidden(void) {

}
C2_HOOK_FUNCTION(0x004415e0, EnsureSpecialVolumesHidden)

void C2_HOOK_FASTCALL ShowSpecialVolumesIfRequ(void) {

}
C2_HOOK_FUNCTION(0x004415f0, ShowSpecialVolumesIfRequ)

void C2_HOOK_FASTCALL DoEditModeKey(int pIndex) {
    int modifiers;

    if (C2V(gI_am_cheating) == 0xa11ee75d || (C2V(gI_am_cheating) == 0x564e78b9 && C2V(gNet_mode) == eNet_mode_none)) {
        modifiers = 0;
        if (PDKeyDown(0)) {
            modifiers |= 4;
        }
        if (PDKeyDown(1)) {
            modifiers |= 2;
        }
        if (PDKeyDown(2)) {
            modifiers |= 1;
        }
        if (C2V(gEdit_funcs)[C2V(gWhich_edit_mode)][pIndex][modifiers] != NULL) {
            C2V(gEdit_funcs)[C2V(gWhich_edit_mode)][pIndex][modifiers]();
        }
    } else {
        C2V(gWhich_edit_mode) = 0;
    }
}

// Key: 'f5'
void (C2_HOOK_FASTCALL * F5Key_original)(void);
void C2_HOOK_FASTCALL F5Key(void) {
#if 0//defined(C2_HOOKS_ENABLED)
    F5Key_original();
#else
    DoEditModeKey(0);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00441600, F5Key, F5Key_original)

// Key: 'f6'
void (C2_HOOK_FASTCALL * F6Key_original)(void);
void C2_HOOK_FASTCALL F6Key(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    F6Key_original();
#else
    DoEditModeKey(1);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00441680, F6Key, F6Key_original)

// Key: 'f7'
void (C2_HOOK_FASTCALL * F7Key_original)(void);
void C2_HOOK_FASTCALL F7Key(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    F7Key_original();
#else
    DoEditModeKey(2);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00441700, F7Key, F7Key_original)

// Key: 'f8'
void (C2_HOOK_FASTCALL * F8Key_original)(void);
void C2_HOOK_FASTCALL F8Key(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    F8Key_original();
#else
    DoEditModeKey(3);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00441780, F8Key, F8Key_original)

// Key: 'f10'
void (C2_HOOK_FASTCALL * F10Key_original)(void);
void C2_HOOK_FASTCALL F10Key(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    F10Key_original();
#else
    DoEditModeKey(4);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00441800, F10Key, F10Key_original)

// Key: 'f11'
void (C2_HOOK_FASTCALL * F11Key_original)(void);
void C2_HOOK_FASTCALL F11Key(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    F11Key_original();
#else
    DoEditModeKey(5);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00441880, F11Key, F11Key_original)

// Key: 'f12'
void (C2_HOOK_FASTCALL * F12Key_original)(void);
void C2_HOOK_FASTCALL F12Key(void) {

#if 0///defined(C2_HOOKS_ENABLED)
    F12Key_original();
#else
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00441900, F12Key, F12Key_original)

// Key: '0'
void (C2_HOOK_FASTCALL * NumberKey0_original)(void);
void C2_HOOK_FASTCALL NumberKey0(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    NumberKey0_original();
#else
    DoEditModeKey(7);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00441910, NumberKey0, NumberKey0_original)

// Key: '1'
void (C2_HOOK_FASTCALL * NumberKey1_original)(void);
void C2_HOOK_FASTCALL NumberKey1(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    NumberKey1_original();
#else
    DoEditModeKey(8);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00441990, NumberKey1, NumberKey1_original)

// Key: '2'
void (C2_HOOK_FASTCALL * NumberKey2_original)(void);
void C2_HOOK_FASTCALL NumberKey2(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    NumberKey2_original();
#else
    DoEditModeKey(9);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00441a10, NumberKey2, NumberKey2_original)

// Key: '3'
void (C2_HOOK_FASTCALL * NumberKey3_original)(void);
void C2_HOOK_FASTCALL NumberKey3(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    NumberKey3_original();
#else
    DoEditModeKey(10);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00441a90, NumberKey3, NumberKey3_original)

// Key: '4'
void (C2_HOOK_FASTCALL * NumberKey4_original)(void);
void C2_HOOK_FASTCALL NumberKey4(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    NumberKey4_original();
#else
    DoEditModeKey(11);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00441b10, NumberKey4, NumberKey4_original)

// Key: '5'
void (C2_HOOK_FASTCALL * NumberKey5_original)(void);
void C2_HOOK_FASTCALL NumberKey5(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    NumberKey5_original();
#else
    DoEditModeKey(12);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00441b90, NumberKey5, NumberKey5_original)

// Key: '6'
void (C2_HOOK_FASTCALL * NumberKey6_original)(void);
void C2_HOOK_FASTCALL NumberKey6(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    NumberKey6_original();
#else
    DoEditModeKey(13);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00441c10, NumberKey6, NumberKey6_original)

// Key: '7'
void (C2_HOOK_FASTCALL * NumberKey7_original)(void);
void C2_HOOK_FASTCALL NumberKey7(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    NumberKey7_original();
#else
    DoEditModeKey(14);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00441c90, NumberKey7, NumberKey7_original)

// Key: '8'
void (C2_HOOK_FASTCALL * NumberKey8_original)(void);
void C2_HOOK_FASTCALL NumberKey8(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    NumberKey8_original();
#else
    DoEditModeKey(15);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00441d10, NumberKey8, NumberKey8_original)

// Key: '9'
void (C2_HOOK_FASTCALL * NumberKey9_original)(void);
void C2_HOOK_FASTCALL NumberKey9(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    NumberKey9_original();
#else
    DoEditModeKey(16);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00441d90, NumberKey9, NumberKey9_original)

// Key: 'kp_1'
void (C2_HOOK_FASTCALL * ToggleFlying_original)(void);
void C2_HOOK_FASTCALL ToggleFlying(void) {
#if 0//defined(C2_HOOKS_ENABLED)
    ToggleFlying_original();
#else

    if (C2V(gAllow_car_flying) && C2V(gNet_mode) == eNet_mode_none) {
        C2V(gCar_flying) = !C2V(gCar_flying);
        NewTextHeadupSlot(4, 0, 500, -4, C2V(gCar_flying) ? "We have lift off!!" : "Back down to Earth");
    } else {
        C2V(gCar_flying) = 0;
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004443c0, ToggleFlying, ToggleFlying_original)

// Key: '-'
void (C2_HOOK_FASTCALL * ScreenSmaller_original)(void);
void C2_HOOK_FASTCALL ScreenSmaller(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    ScreenSmaller_original();
#else
    if (C2V(gMap_view) != 2) {
        C2V(gRender_indent) += 1;
        if (C2V(gRender_indent) > 8) {
            C2V(gRender_indent) = 8;
        }
        AdjustRenderScreenSize();
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004e4cd0, ScreenSmaller, ScreenSmaller_original)

// Key: '='
void (C2_HOOK_FASTCALL * ScreenLarger_original)(void);
void C2_HOOK_FASTCALL ScreenLarger(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    ScreenLarger_original();
#else
    if (C2V(gMap_view) != 2) {
        C2V(gRender_indent) -= 1;
        if (C2V(gRender_indent) < 0) {
            C2V(gRender_indent) = 0;
        }
        AdjustRenderScreenSize();
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004e4d00, ScreenLarger, ScreenLarger_original)

// Key: 'Del'
void (C2_HOOK_FASTCALL * BuyArmour_original)(void);
void C2_HOOK_FASTCALL BuyArmour(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    BuyArmour_original();
#else
    if (C2V(gINT_0068b8e4) == 0 && C2V(gINT_0068b8e8) == 0) {
        BuyPSPowerup(0);
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00502e50, BuyArmour, BuyArmour_original)

// Key: 'End'
void (C2_HOOK_FASTCALL * BuyPower_original)(void);
void C2_HOOK_FASTCALL BuyPower(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    BuyPower_original();
#else
    if (C2V(gINT_0068b8e4) == 0 && C2V(gINT_0068b8e8) == 0) {
        if (C2V(gNet_mode) != eNet_mode_none && C2V(gCurrent_net_game)->type == eNet_game_type_foxy && C2V(gThis_net_player_index) == C2V(gIt_or_fox)) {
            NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_fox_cant_do_that));
        } else {
            BuyPSPowerup(1);
        }
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00502fd0, BuyPower, BuyPower_original)

// Key: 'PgDn'
void (C2_HOOK_FASTCALL * BuyOffense_original)(void);
void C2_HOOK_FASTCALL BuyOffense(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    BuyOffense_original();
#else
    if (C2V(gINT_0068b8e4) == 0 && C2V(gINT_0068b8e8) == 0) {
        BuyPSPowerup(2);
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00503030, BuyOffense, BuyOffense_original)

// Key: ' '
void (C2_HOOK_FASTCALL * ViewNetPlayer_original)(void);
void C2_HOOK_FASTCALL ViewNetPlayer(void) {
    CONTROLS_START();
#if defined(C2_HOOKS_ENABLED)
    int* a = (int*)0x006792bc;
    c2_printf("*%p=0x%x\n", a, *a);
    int* b = (int*)0x006792bc;
    c2_printf("*%p=0x%x\n", b, *b);
    ViewNetPlayer_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0040e430, ViewNetPlayer, ViewNetPlayer_original)

// Key: '`'
void (C2_HOOK_FASTCALL * UserSendMessage_original)(void);
void C2_HOOK_FASTCALL UserSendMessage(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    UserSendMessage_original();
#else
    if (C2V(gNet_mode) != eNet_mode_none && C2V(gCurrent_net_game)->options.enable_text_messages) {
        C2V(gEntering_message) = 1;
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004448f0, UserSendMessage, UserSendMessage_original)

// Key: 't'
void (C2_HOOK_FASTCALL * ToggleTargetLock_original)(void);
void C2_HOOK_FASTCALL ToggleTargetLock(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    ToggleTargetLock_original();
#else
    if (C2V(gNet_mode) != eNet_mode_none) {
        if (C2V(gCurrent_net_game)->type == eNet_game_type_foxy) {
            return;
        }
        if (!C2V(gCurrent_net_game)->options.show_powerups_on_map) {
            return;
        }
    }
    if (C2V(gTarget_lock_car_1) == NULL) {
        NewTextHeadupSlot2(4, 0, 1500, -4, GetMiscString(eMiscString_no_target_to_lock_onto), 0);
    } else {
        C2V(gTarget_lock_enabled) = !C2V(gTarget_lock_enabled);
        if (C2V(gTarget_lock_enabled)) {
            const char* driver_name;
            char s[256];

            if (C2V(gNet_mode) == eNet_mode_none) {
                driver_name = GetDriverName(C2V(gTarget_lock_car_2)->car_ID >> 8, C2V(gTarget_lock_car_2)->car_ID & 0xff);
            } else {
                driver_name = NetPlayerFromCar(C2V(gTarget_lock_car_2))->player_name;
            }
            c2_sprintf(s, "%s %s", driver_name, GetMiscString(eMiscString_locked_onto));
            NewTextHeadupSlot2(4, 0, 1500, -4, s, 0);
        } else {
            NewTextHeadupSlot2(4, 0, 1500, -4, GetMiscString(eMiscString_target_lock_off), 0);
        }
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004945f0, ToggleTargetLock, ToggleTargetLock_original)

// Key: 'y'
void (C2_HOOK_FASTCALL * CycleTargetLock_original)(void);
void C2_HOOK_FASTCALL CycleTargetLock(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    CycleTargetLock_original();
#else

    if (C2V(gNet_mode) == eNet_mode_none || (C2V(gCurrent_net_game)->type != eNet_game_type_foxy && C2V(gCurrent_net_game)->options.show_players_on_map)) {
        int category;
        int car_index;
        int car_count;
        tCar_spec* car;

        category = C2V(gNet_mode) != eNet_mode_none ? eVehicle_net_player : eVehicle_opponent;
        if (C2V(gTarget_lock_car_1) == NULL) {
            if (category == eVehicle_self) {
                C2V(gTarget_lock_car_1) = &C2V(gProgram_state).current_car;
            } else {
                C2V(gTarget_lock_car_1) = GetCarSpec(category, 0);
            }
        }
        if (category == eVehicle_self) {
            car_count = 1;
        } else {
            car_count = GetCarCount(category);
        }
        for (car_index = 0;; car_index++) {
            tCar_spec* c;

            if (car_index >= car_count) {
                return;
            }

            if (category == eVehicle_self) {
                c = &C2V(gProgram_state).current_car;
            } else {
                c = GetCarSpec(category, car_index);
            }
            if (c == C2V(gTarget_lock_car_1)) {
                break;
            }
        }
        if (car_index == car_count - 1) {
            car_index = 0;
        } else {
            car_index += 1;
        }

        for (;;) {
            for (;;) {
                if (category == eVehicle_self) {
                    car = &C2V(gProgram_state).current_car;
                } else {
                    car = GetCarSpec(category, car_index);
                }
                if (car_index == car_count - 1) {
                    car_index = 0;
                } else {
                    car_index += 1;
                }
                if (car == C2V(gTarget_lock_car_1) || !car->knackered) {
                    break;
                }
            }
            if (C2V(gNet_mode) == eNet_mode_none) {
                if (C2V(gCurrent_race).race_spec->race_type == kRaceType_Cars && !car->is_race_goal) {
                    continue;
                }
                break;
            }
            if (C2V(gCurrent_net_game)->type != eNet_game_type_6) {
                break;
            }
            if (!NetPlayerFromCar(car)->field_0x80) {
                break;
            }
        }
        C2V(gTarget_lock_enabled) = 0;
        C2V(gTarget_lock_car_1) = car;
        C2V(gTarget_lock_car_2) = car;
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00494700, CycleTargetLock, CycleTargetLock_original)


// Key: ';'
void (C2_HOOK_FASTCALL * ToggleInventory_original)(void);
void C2_HOOK_FASTCALL ToggleInventory(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    ToggleInventory_original();
#else
    C2V(gInventory_cycling) = !C2V(gInventory_cycling);
    if (C2V(gInventory_cycling)) {
        C2V(gInventory_timeout) = GetTotalTime() + 3000;
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004da9d0, ToggleInventory, ToggleInventory_original)

// Key: '['
void (C2_HOOK_FASTCALL * LeftInventory_original)(void);
void C2_HOOK_FASTCALL LeftInventory(void) {
    CONTROLS_START();
#if defined(C2_HOOKS_ENABLED)
    LeftInventory_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004dab80, LeftInventory, LeftInventory_original)

// Key: ']'
void (C2_HOOK_FASTCALL * RightInventory_original)(void);
void C2_HOOK_FASTCALL RightInventory(void) {
    CONTROLS_START();
#if defined(C2_HOOKS_ENABLED)
    RightInventory_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004daa00, RightInventory, RightInventory_original)

// Key: '*' (keypad)
void (C2_HOOK_FASTCALL * ChangeCameraTypeInGame_original)(void);
void C2_HOOK_FASTCALL ChangeCameraTypeInGame(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    ChangeCameraTypeInGame_original();
#else
    if (!C2V(gAction_replay_mode)) {

        ChangeCameraType();
        NewTextHeadupSlot2(4, 0, 1000, -4, GetMiscString(eMiscString_standard_cam) + C2V(gAction_replay_mode), 0);
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00444ed0, ChangeCameraTypeInGame, ChangeCameraTypeInGame_original)

void (C2_HOOK_FASTCALL * ChangeCameraType_original)(void);
void C2_HOOK_FASTCALL ChangeCameraType(void) {

#if defined(C2_HOOKS_ENABLED)
    ChangeCameraType_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0040e7f0, ChangeCameraType, ChangeCameraType_original)

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

void C2_HOOK_FASTCALL CheckForCheatingGits(void) {
    tU32* keys;

    keys = KevKeyService();
    if (keys[0] == 0x616fb8ea && keys[0] == 0x7c6100a8) {
        ToggleSoundEnable();
        C2V(gProgram_state).game_completed = 1;
        DRS3StartSound(C2V(gEffects_outlet), eSoundId_FlaskGone);
    }
}
C2_HOOK_FUNCTION(0x00443c50, CheckForCheatingGits)

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

void C2_HOOK_FASTCALL CheckSystemKeys(int pRacing) {
    tU32 start_menu_time;

    start_menu_time = PDGetTotalTime();
    CheckQuit();
    AddLostTime(PDGetTotalTime() - start_menu_time);
    CheckToggles(pRacing);
    if (pRacing & !C2V(gAction_replay_mode)) {
        CheckOtherRacingKeys();
    }
}
C2_HOOK_FUNCTION(0x00442e90, CheckSystemKeys)

void C2_HOOK_FASTCALL CheckToggles(int pRacing) {
    int i;
    int new_state;

    for (i = 0; i < REC2_ASIZE(C2V(gToggle_array)); i++) {
        if (!pRacing && C2V(gToggle_array)[i].in_game_only) {
            continue;
        }
        if ((C2V(gTyping) || C2V(gEntering_message)) && C2V(gToggle_array)[i].key2 == -2) {
            continue;
        }
        new_state = 0;
        if (C2V(gToggle_array)[i].key1 == -2 || KeyIsDown2(C2V(gToggle_array)[i].key1)) {
            if (C2V(gToggle_array)[i].key2 == -2 && C2V(gToggle_array)[i].exact_modifiers) {
                if (!PDKeyDown(0) && !PDKeyDown(1) && !PDKeyDown(2) && !PDKeyDown(3)) {
                    new_state = 1;
                }
            } else {
                if (KeyIsDown(C2V(gToggle_array)[i].key2)) {
                    new_state = 1;
                }
            }
        }
        if (C2V(gToggle_array)[i].on_last_time != new_state) {
            C2V(gToggle_array)[i].on_last_time = new_state;
            if (new_state) {
                C2V(gToggle_array)[i].action_proc();
            }
        }
    }
}

void (C2_HOOK_FASTCALL * CheckOtherRacingKeys_original)(void);
void C2_HOOK_FASTCALL CheckOtherRacingKeys(void) {

#if defined(C2_HOOKS_ENABLED)
    CheckOtherRacingKeys_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00442f90, CheckOtherRacingKeys, CheckOtherRacingKeys_original)

void (C2_HOOK_FASTCALL * FlipUpCar_original)(tCar_spec* pCar_spec);
void C2_HOOK_FASTCALL FlipUpCar(tCar_spec* pCar_spec) {

#if 0//defined(C2_HOOKS_ENABLED)
    FlipUpCar_original(car);
#else
    int new_pos;
    int i;
    int j;
    int k;

    if (pCar_spec != NULL && pCar_spec->driver == eDriver_local_human && C2V(gNet_mode) == eNet_mode_none) {
        FadePaletteDown();
        while (KeyIsDown(0x3c)) {
        }
    }

    if (pCar_spec->car_crush_spec != NULL && pCar_spec->car_crush_spec->field_0x4b8) {
        WeldCar(pCar_spec);
    }
    SetCollisionInfoChildsDoNothing(pCar_spec->collision_info, 0);
    EnableCar(pCar_spec);

    new_pos = 1;
    for (i = 0; i < 4; i++) {
        if (pCar_spec->susp_height[i / 2] <= pCar_spec->oldd[i]) {
            new_pos = 0;
        }
    }
    for (i = 0; i < 3; i++) {
        br_scalar t;
        br_vector3 dir;
        br_vector3 nor;
        br_vector3 p;
        br_material* mat;

        BrVector3Sub(&nor, &pCar_spec->car_master_actor->t.t.translate.t, (br_vector3*)pCar_spec->last_safe_positions[0].m[3]);
        if (BrVector3LengthSquared(&nor) > 8.3015966f) {
            new_pos = 0;
        }
        BrMatrix34Copy(&pCar_spec->car_master_actor->t.t.mat, &pCar_spec->last_safe_positions[new_pos]);
        BrMatrix34Copy(&pCar_spec->collision_info->transform_matrix, &pCar_spec->last_safe_positions[new_pos]);
        BrMatrix34Copy(&pCar_spec->old_frame_mat, &pCar_spec->last_safe_positions[new_pos]);
        BrVector3Set(&dir, 0.f, 0.2898551f, 0.f);
        FindFace(&pCar_spec->car_master_actor->t.t.translate.t, &dir, &nor, &t, &mat);
        if (t > 1.f) {
            BrVector3Accumulate(&pCar_spec->car_master_actor->t.t.translate.t, &dir);
            BrVector3Copy((br_vector3*)pCar_spec->collision_info->transform_matrix.m[3], &pCar_spec->car_master_actor->t.t.translate.t);
            BrVector3Copy((br_vector3*)pCar_spec->old_frame_mat.m[3], &pCar_spec->car_master_actor->t.t.translate.t);
        }
        BrVector3Set(&nor, 0.f, 0.f, -0.0002f);
        BrMatrix34ApplyV(&pCar_spec->collision_info->v, &nor, &pCar_spec->car_master_actor->t.t.mat);
        BrVector3Set(&pCar_spec->collision_info->omega, 0.f, 0.f, 0.f);
        BrVector3Negate(&pCar_spec->direction, (br_vector3*)pCar_spec->collision_info->transform_matrix.m[2]);

        for (j = 0; j <= new_pos; j++) {
            for (k = 0; k < REC2_ASIZE(pCar_spec->last_safe_positions) - 1; k++) {
                BrMatrix34Copy(&pCar_spec->last_safe_positions[k], &pCar_spec->last_safe_positions[k + 1]);
            }
        }
        FlipUpCollisionInfo(pCar_spec->collision_info);
        PositionChildren(pCar_spec->collision_info);
        for (j = 0; !TestForCarInSensiblePlace(pCar_spec, &p) && j < 10; j++) {
            BrVector3InvScale(&p, &p, 2.f * WORLD_SCALE);
            BrVector3Accumulate(&pCar_spec->car_master_actor->t.t.translate.t, &p);
            BrVector3Copy((br_vector3*)pCar_spec->collision_info->transform_matrix.m[3], &pCar_spec->car_master_actor->t.t.translate.t);
            BrVector3Copy((br_vector3*)pCar_spec->old_frame_mat.m[3], &pCar_spec->car_master_actor->t.t.translate.t);
        }
    }
    BrVector3Copy((br_vector3*)pCar_spec->collision_info->transform_matrix.m[3],
        &pCar_spec->car_master_actor->t.t.translate.t);
    pCar_spec->curvature = 0.f;
    for (i = 0; i < REC2_ASIZE(pCar_spec->oldd); i++) {
        pCar_spec->oldd[i] = pCar_spec->ride_height;
    }
    pCar_spec->revs = 0.f;
    pCar_spec->gear = 0.f;
    pCar_spec->collision_info->auto_special_volume = NULL;
    if (pCar_spec != NULL && pCar_spec->driver == eDriver_local_human) {
        InitialiseExternalCamera();
        PositionExternalCamera(pCar_spec, 100);
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004429b0, FlipUpCar, FlipUpCar_original)

void C2_HOOK_FASTCALL FlipUpCollisionInfo(tCollision_info* pCollision_info) {
    tCollision_info *child;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCollision_info, v, 0x68);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCollision_info, omega, 0x74);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCollision_info, transform_matrix, 0x8c);

    for (child = pCollision_info->child; child != NULL; child = child->next) {
        int i;

        for (i = 0; i < 3; i++) {
            BrVector3Copy((br_vector3*)child->transform_matrix.m[i], (br_vector3*)pCollision_info->transform_matrix.m[i]);
        }
        BrVector3SetFloat(&child->v, 0.f, 0.f, 0.f);
        BrVector3SetFloat(&child->omega, 0.f, 0.f, 0.f);
        FlipUpCollisionInfo(child);
    }
}
C2_HOOK_FUNCTION(0x00442e10, FlipUpCollisionInfo)

void C2_HOOK_FASTCALL ResetRecoveryVouchers(void) {

    C2V(gRecovery_voucher_count) = 0;
}
C2_HOOK_FUNCTION(0x00444710, ResetRecoveryVouchers)

void (C2_HOOK_FASTCALL * EnterUserMessage_original)(void);
void C2_HOOK_FASTCALL EnterUserMessage(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    EnterUserMessage_original();
#else
    static C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, last_key, 0x00590f64, -1);
    static C2_HOOK_VARIABLE_IMPLEMENT(int, about_to_die, 0x0067c49c);
    static C2_HOOK_VARIABLE_IMPLEMENT(tU32, next_time, 0x0067c454);
    char* the_message;
    int len;
    int the_key;
    int abuse_num;

    if (!C2V(gEntering_message)) {
        return;
    }
    if (C2V(gNet_mode) == eNet_mode_none) {
        return;
    }
    if (!C2V(gCurrent_net_game)->options.enable_text_messages) {
        return;
    }
    the_key = PDAnyKeyDown();
    if (C2V(gEntering_message) == 1) {
        if (the_key != -1) {
            return;
        }
        C2V(gEntering_message) = 2;
        PDClearKeyboardBuffer();
    }
    if (C2V(about_to_die)) {
        if (the_key != -1) {
            return;
        }
        C2V(gEntering_message) = 0;
        C2V(about_to_die) = 0;
        return;
    }
    if (the_key == C2V(last_key)) {
        if (C2V(next_time) < PDGetTotalTime()) {
            C2V(next_time) += 100;
        } else {
            the_key = -1000;
        }
    } else {
        C2V(last_key) = the_key;
        C2V(next_time) = PDGetTotalTime() + 500;
    }
    switch (the_key) {
    case -1:
    case 0:
        break;
    case 2:
    case 3:
    case 53:
    case 63:
        C2V(about_to_die) = 1;
        break;
    case 50:
    case 65:
    case 70:
        len = c2_strlen(&C2V(gString)[20]);
        if (len > 0) {
            C2V(gString)[20 + len - 1] = '\0';
        }
        break;
    case 51:
    case 52:
        len = c2_strlen(C2V(gNet_players)[C2V(gThis_net_player_index)].player_name);
        if (len <= 18) {
            the_message = C2V(gString) + 18 - len;
            c2_strcpy(the_message, C2V(gNet_players)[C2V(gThis_net_player_index)].player_name);
            the_message[len + 0] = ':';
            the_message[len + 1] = ' ';
            C2V(gString)[REC2_ASIZE(C2V(gString)) - 1] = '\0';
            NetSendHeadupToAllPlayers(the_message);
            C2V(gString)[20] = '\0';
            NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_message_sent));
            C2V(about_to_die) = 1;
        }
        break;
    default:
        if (C2V(gKey_mapping)[71] == the_key) {
            C2V(about_to_die) = 1;
        } else if (the_key <= 74 || the_key >= 106) {
            for (;;) {
                int raw_ch = PDGetKeyboardCharacter();
                int ch;
                if (raw_ch == '\0') {
                    break;
                }
                ch = PDConvertToASCIILessThan128(raw_ch);
                len = c2_strlen(&C2V(gString)[20]);
                if (len < 64 - 1) {
                    C2V(gString)[20 + len] = ch;
                    if (ch < C2V(gFonts)[4].offset || ch >= C2V(gFonts)[4].offset + C2V(gFonts)[4].num_entries) {
                        C2V(gString)[20 + len] = '\0';
                    }
                    C2V(gString)[20 + len + 1] = '\0';
                    dr_dprintf("NETMESSAGE: '%s'", &C2V(gString)[20]);
                }
            }
        } else if (the_key >= 81 && the_key <= 90) {
            if (the_key == 81) {
                abuse_num = 9;
            } else {
                abuse_num = the_key - 82;
            }
            if (C2V(gAbuse_text)[abuse_num] != NULL) {
                c2_strcpy(&C2V(gString)[20], C2V(gAbuse_text)[abuse_num]);
            }
        } else {
            C2V(gEntering_message) = 0;
        }
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00444910, EnterUserMessage, EnterUserMessage_original)

void (C2_HOOK_FASTCALL * PollCameraControls_original)(tU32 pCamera_period);
void C2_HOOK_FASTCALL PollCameraControls(tU32 pCamera_period) {

#if defined(C2_HOOKS_ENABLED)
    PollCameraControls_original(pCamera_period);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00444270, PollCameraControls, PollCameraControls_original)

void (C2_HOOK_FASTCALL * CheckRecoveryOfCars_original)(tU32 pEndFrameTime);
void C2_HOOK_FASTCALL CheckRecoveryOfCars(tU32 pEndFrameTime) {

#if defined(C2_HOOKS_ENABLED)
    CheckRecoveryOfCars_original(pEndFrameTime);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00442500, CheckRecoveryOfCars, CheckRecoveryOfCars_original)

void (C2_HOOK_FASTCALL * PollCarControls_original)(tU32 pTime_difference);
void C2_HOOK_FASTCALL PollCarControls(tU32 pTime_difference) {

#if defined(C2_HOOKS_ENABLED)
    PollCarControls_original(pTime_difference);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00443e80, PollCarControls, PollCarControls_original)

void (C2_HOOK_FASTCALL * FUN_0045a670_original)(void);
void C2_HOOK_FASTCALL FUN_0045a670(void) {

#if defined(C2_HOOKS_ENABLED)
    FUN_0045a670_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0045a670, FUN_0045a670, FUN_0045a670_original)

void (C2_HOOK_FASTCALL * FUN_0045acf0_original)(void);
void C2_HOOK_FASTCALL FUN_0045acf0(void) {

#if defined(C2_HOOKS_ENABLED)
    FUN_0045acf0_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0045acf0, FUN_0045acf0, FUN_0045acf0_original)

void (C2_HOOK_FASTCALL * FUN_00511240_original)(void);
void C2_HOOK_FASTCALL FUN_00511240(void) {

#if defined(C2_HOOKS_ENABLED)
    FUN_00511240_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00511240, FUN_00511240, FUN_00511240_original)

void (C2_HOOK_FASTCALL * BuyPSPowerup_original)(int pIndex);
void C2_HOOK_FASTCALL BuyPSPowerup(int pIndex) {

#if defined(C2_HOOKS_ENABLED)
    BuyPSPowerup_original(pIndex);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00502e70, BuyPSPowerup, BuyPSPowerup_original)
