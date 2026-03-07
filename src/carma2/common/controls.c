#include "controls.h"

#include "brucetrk.h"
#include "car.h"
#include "crush.h"
#include "depth.h"
#include "displays.h"
#include "52-errors.h"
#include "finteray.h"
#include "globvars.h"
#include "globvrkm.h"
#include "globvrpb.h"
#include "grafdata.h"
#include "graphics.h"
#include "init.h"
#include "input.h"
#include "joystick.h"
#include "loading.h"
#include "netgame.h"
#include "network.h"
#include "opponent.h"
#include "physics.h"
#include "piping.h"
#include "polyfont.h"
#include "powerups.h"
#include "pratcam.h"
#include "raycast.h"
#include "replay.h"
#include "sound.h"
#include "spark.h"
#include "structur.h"
#include "utility.h"
#include "world.h"

#include "platform.h"

#include "brender/brender.h"

#include "c2_string.h"

#include "rec2_macros.h"


// GLOBAL: CARMA2_HW 0x0067c474
int gEntering_message;

// GLOBAL: CARMA2_HW 0x0067c3c8
char* gAbuse_text[10];

// GLOBAL: CARMA2_HW 0x00590970
tCheat gKev_keys[] = {
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
    { 0x4b054b60, 0x4b054b60, RichardsSphere, 0x00000000 },
    { 0x00000000, 0x00000000, NULL,       0x00000000 },
};

// GLOBAL: CARMA2_HW 0x005900a0
tToggle_element gToggle_array[44] = {
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
};

// GLOBAL: CARMA2_HW 0x0067c3f8
int gRecovery_voucher_count;

// GLOBAL: CARMA2_HW 0x0079ec54
int gAuto_repair;

// GLOBAL: CARMA2_HW 0x0079ec50
int gInstant_handbrake;

// GLOBAL: CARMA2_HW 0x0067c47c
tU32 gToo_poor_for_recovery_timeout;

// GLOBAL: CARMA2_HW 0x00659b28
int gCheckpoint_finder_enabled = 1;


// GLOBAL: CARMA2_HW 0x0068b8e4
int gINT_0068b8e4;

// GLOBAL: CARMA2_HW 0x0068b8e8
int gINT_0068b8e8;

// GLOBAL: CARMA2_HW 0x0067c460
int gHad_auto_recover;

// GLOBAL: CARMA2_HW 0x0067c478
tU32 gPalette_fade_time;

// GLOBAL: CARMA2_HW 0x0067c3bc
int gRecover_timer;

// GLOBAL: CARMA2_HW 0x0067c3b8
int gToo_late;

// GLOBAL: CARMA2_HW 0x0067c470
int gINT_0067c470;

// GLOBAL: CARMA2_HW 0x0067c468
int gWhich_edit_mode; /* FIXME: enum tEdit_mode */

// GLOBAL: CARMA2_HW 0x005904f0
tEdit_func* gEdit_funcs[2][18][8] = {
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
            CycleCarSimplificationLevel, /* [0][ 8][0] */
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
        },
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
            ToggleAccessoryRendering, /* [0][12][4] */
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
            ShowCurrentJoystickName, /* [0][15][0] */
            ToggleJoystickHeadup, /* [0][15][1] */
            ToggleJoystickYAxis, /* [0][15][2] */
            ToggleJoystickDPad, /* [0][15][3] */
            CycleJoystick, /* [0][15][4] */
            ToggleJoystickEnable, /* [0][15][5] */
            IncreaseJoystickFFB, /* [0][15][6] */
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
            ResetMan, /* [1][ 1][4] */
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
        },
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
};

// GLOBAL: CARMA2_HW 0x0067c46c
int gAllow_car_flying;

// GLOBAL: CARMA2_HW 0x005904c0
const char * gEdit_mode_names[12] = {
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
};

// GLOBAL: CARMA2_HW 0x0068d8c4
tCar_spec* gTarget_lock_car_1;

// GLOBAL: CARMA2_HW 0x0068d6f0
tCar_spec* gTarget_lock_car_2;

// GLOBAL: CARMA2_HW 0x006a0940
int gInventory_cycling;

// GLOBAL: CARMA2_HW 0x006a0954
tU32 gInventory_timeout;

// GLOBAL: CARMA2_HW 0x0067c400
char gString[84];

// GLOBAL: CARMA2_HW 0x0067c498
int gInvulnerability;

// GLOBAL: CARMA2_HW 0x00590f60
int gCancel_recover = 1;

// GLOBAL: CARMA2_HW 0x0067c45c
int gRepair_last_time;

// GLOBAL: CARMA2_HW 0x0067c464
tU32 gLast_repair_time;


// FUNCTION: CARMA2_HW 0x00456950
void C2_HOOK_FASTCALL SetSoundDetailLevel(int pLevel) {

    gSound_detail_level = pLevel;
}

// FUNCTION: CARMA2_HW 0x00456960
void C2_HOOK_FASTCALL ReallySetSoundDetailLevel(int pLevel) {

    DRS3StopAllOutletSoundsExceptCDA();
    DisposeSoundSources();
    gSound_detail_level = pLevel;
    InitSound();
    InitSoundSources();
}

// FUNCTION: CARMA2_HW 0x004569e0
int C2_HOOK_FASTCALL GetSoundDetailLevel(void) {

    return gSound_detail_level;
}

// Key: 'm'
// FUNCTION: CARMA2_HW 0x004420e0
void C2_HOOK_FASTCALL ToggleMiniMap(void) {

    if (gMap_view == 2) {
        return;
    }
    gMini_map_visible = !gMini_map_visible;
    NewTextHeadupSlot(4, 0, 500, -4, GetMiscString(gMini_map_visible ? eMiscString_minimap_on : eMiscString_minimap_off));
    UpdateMapAndSaveOptions();
}

// Key: 'Enter'
// FUNCTION: CARMA2_HW 0x0042dd50
void C2_HOOK_FASTCALL ToggleFlaps(void) {

    if (gCountdown || gToo_poor_for_recovery_timeout != 0) {
        return;
    }
    if (gNet_mode == eNet_mode_none || gNet_mode == eNet_mode_host) {
        PhysicsObjectRecurse(gProgram_state.current_car.collision_info,
            ToggleFlapsCB,
            &gProgram_state.current_car);
    } else {
        tNet_message_chunk* chunk = NetAllocateMessageChunk(eNet_message_chunk_type_toggle_doors, 0);
        tNet_game_player_info *player_info = NetPlayerFromCar(&gProgram_state.current_car);
        if (player_info != NULL) {
            chunk->toggle_doors.id = player_info->ID;
        }
    }
}

// FUNCTION: CARMA2_HW 0x0042ddb0
int C2_HOOK_FASTCALL ToggleFlapsCB(tCollision_info* pCollision_info, void* data) {
    tCar_spec *pCar_spec = data;

    DRActorEnumRecurse(pCollision_info->actor, ToggleDoorsActorCallback, pCar_spec);
    return 0;
}

// FUNCTION: CARMA2_HW 0x0042ddc0
int C2_HOOK_CDECL ToggleDoorsActorCallback(br_actor* pActor, void* data) {
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

    C2_HOOK_BUG_ON(REC2_ASIZE(gToggled_doors) != 16);

    if (crush_buffer->flap_data->field_0x0) {
        if (gCount_toggled_doors != REC2_ASIZE(gToggled_doors)) {
            for (i = 0; i < gCount_toggled_doors; i++) {
                if (gToggled_doors[i].actor == pActor) {
                    return 0;
                }
            }
            gToggled_doors[gCount_toggled_doors].actor = pActor;
            gToggled_doors[gCount_toggled_doors].car = pCar;
            gToggled_doors[gCount_toggled_doors].field_0x8 = 0;
            gToggled_doors[gCount_toggled_doors].field_0xc = 0.f;
            gCount_toggled_doors += 1;
        }
    } else {
        if (gCount_toggled_doors != REC2_ASIZE(gToggled_doors)) {
            for (i = 0; i < gCount_toggled_doors; i++) {
                if (gToggled_doors[i].actor == pActor) {
                    return 0;
                }
            }
            gToggled_doors[gCount_toggled_doors].actor = pActor;
            gToggled_doors[gCount_toggled_doors].car = pCar;
            gToggled_doors[gCount_toggled_doors].field_0x8 = 1;
            gToggled_doors[gCount_toggled_doors].field_0xc = 20.f;
            gCount_toggled_doors += 1;
        }
    }
    return 0;
}

void C2_HOOK_FASTCALL ToggleMap2(void) {
    // GLOBAL: CARMA2_HW 0x0067c458
    static int old_indent;
    int original_indent;

    original_indent = gRender_indent;
    if (gMap_view == 2) {
        gMap_view = 1;
        gRender_indent = old_indent;
    } else {
        if (gAction_replay_mode) {
            return;
        }
        if (gNet_mode != eNet_mode_none && gCurrent_net_game->type == eNet_game_type_foxy && gThis_net_player_index == gIt_or_fox) {
            NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_fox_cant_do_that));
            return;
        }
        gRender_indent = 0;
        old_indent = original_indent;
        gMap_view = 2;
        gMap_time = PDGetTotalTime();
    }
    if (gCurrent_race.map_image != NULL) {
        // nop_FUN005191f0(gCurrent_race.map_image);
    }
}

// Key: 'tab'
// FUNCTION: CARMA2_HW 0x00444610
void C2_HOOK_FASTCALL ToggleMap(void) {

    ToggleMap2();
    AdjustRenderScreenSize();
}

// FUNCTION: CARMA2_HW 0x00444600
void C2_HOOK_FASTCALL ToggleHeadupMap(void) {

}

// Key: 'ctrl+tab'
// FUNCTION: CARMA2_HW 0x00494840
void C2_HOOK_FASTCALL CycleMapOptions(void) {

    gCheckpoint_finder_enabled = !gCheckpoint_finder_enabled;
    NewTextHeadupSlot2(4, 0, 1500, -4, GetMiscString(gCheckpoint_finder_enabled ? eMiscString_checkpoint_finder_turned_on : eMiscString_checkpoint_finder_turned_off), 0);
}

// key: 'alt+tab' ('alt' in map mode)
// FUNCTION: CARMA2_HW 0x00494880
void C2_HOOK_FASTCALL ToggleMapTransparency(void) {

    gMap_trans = !gMap_trans;
    SaveOptions();
}

// Key: 'r'
// FUNCTION: CARMA2_HW 0x00442300
void C2_HOOK_FASTCALL SetRecovery(void) {

    if (gINT_0068b8e4
        || gINT_0068b8e8
        || gRace_finished
        || gProgram_state.current_car.knackered
        || gWait_for_it
        || gHad_auto_recover
        || gPalette_fade_time != 0) {
        return;
    }
    if (gNet_mode == eNet_mode_none) {
        gRecover_car = 1;
        gRecover_timer = 0;
    } else if (gProgram_state.current_car.time_to_recover == 0) {
        gCancel_recover = 1;
        if (CheckRecoverCost()) {
            if (gCurrent_net_game->type == eNet_game_type_foxy) {
                if (gThis_net_player_index == gIt_or_fox) {
                    gToo_late = 0;
                    gProgram_state.current_car.time_to_recover = GetRaceTime() + 5000;
                } else {
                    gToo_late = 0;
                    gProgram_state.current_car.time_to_recover = GetRaceTime() + 1000;
                }
            } else {
                if (gCurrent_net_game->type == 6 &&
                    gNet_players[gThis_net_player_index].field_0x80 != 0) {
                    gToo_late = 0;
                    gProgram_state.current_car.time_to_recover = GetRaceTime() + 500;
                } else {
                    gToo_late = 0;
                    gProgram_state.current_car.time_to_recover = GetRaceTime() + 3000;
                }
            }
        }
    }
    else if (gCancel_recover) {
        if (GetRaceTime() + 600 < gProgram_state.current_car.time_to_recover) {
            gProgram_state.current_car.time_to_recover = 0;
            if (!gINT_0067c470) {
                float cost = (gNet_mode == eNet_mode_none) ? gRecovery_cost.initial[gProgram_state.skill_level] : gRecovery_cost.initial_network[gCurrent_net_game->type];
                EarnCredits((int)cost);
            }
            NewTextHeadupSlot2(4, 0, 2000, -4, GetMiscString(eMiscString_recovery_cancelled), 0);
        } else {
            NewTextHeadupSlot2(4, 0, 2000, -4, GetMiscString(eMiscString_too_late_to_cancel), 1);
            gToo_late = 1;
        }
    }
}

// FUNCTION: CARMA2_HW 0x00442750
int C2_HOOK_FASTCALL CheckRecoverCost(void) {
    int recovery_cost;

    if (gNet_mode == eNet_mode_none) {
        return 1;
    }

    if (gProgram_state.current_car.knackered
        || gRecovery_voucher_count != 0
        || (NetPlayerFromCar(&gProgram_state.current_car) != NULL && NetPlayerFromCar(&gProgram_state.current_car)->field_0x80 != 0)) {

        gINT_0067c470 = 1;
        return 1;
    }

    gINT_0067c470 = 0;
    recovery_cost = (int)(gNet_mode == eNet_mode_none ? gRecovery_cost.initial[gProgram_state.skill_level] : gRecovery_cost.initial[gCurrent_net_game->type]);

    if (recovery_cost > gProgram_state.credits) {
        int remaining_cost = recovery_cost;
        while (remaining_cost > 0
                && gProgram_state.current_car.power_up_levels[0] != 0
                && gProgram_state.current_car.power_up_levels[1] != 0
                && gProgram_state.current_car.power_up_levels[2] != 0) {
            int which;

            which = IRandomBetween(0, 2);
            if (gProgram_state.current_car.power_up_levels[which] != 0) {
                int part_value;

                gProgram_state.current_car.power_up_levels[which] -= 1;
                part_value = (gNet_mode == eNet_mode_none ? gTrade_in_value_APO.initial[gProgram_state.skill_level] : gTrade_in_value_APO.initial[gCurrent_net_game->type]);
                remaining_cost -= part_value;
                gProgram_state.credits += part_value;
            }
        }
    }
    if (recovery_cost > gProgram_state.credits) {
        gToo_poor_for_recovery_timeout = PDGetTotalTime() + 2000;
        NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_press_return_to_commit_suicide));
        return 0;
    }
    SpendCredits(recovery_cost);
    return 1;
}

// Key: 'ctrl+a'
// FUNCTION: CARMA2_HW 0x00441490
void C2_HOOK_FASTCALL AbortRace(void) {

    if (!gRace_finished) {
        gAbandon_game = 1;
    }
}

// FUNCTION: CARMA2_HW 0x00444f40
void C2_HOOK_FASTCALL ToggleHeadupLevel(void) {

    if (PDKeyDown(0)) {
        gHeadup_detail_level -= 1;
        if (gHeadup_detail_level < 0) {
            gHeadup_detail_level = kMax_headup_detail_level;
        }
    } else {
        gHeadup_detail_level += 1;
        if (gHeadup_detail_level > kMax_headup_detail_level) {
            gHeadup_detail_level = 0;
        }
    }
    NewTextHeadupSlot2(4, 0, 250, -4, GetMiscString(eMiscString_no_HUD + (gHeadup_detail_level % 3)), 0);
}

// FUNCTION: CARMA2_HW 0x00441580
void C2_HOOK_FASTCALL SetFlag(int i) {
    int match;

    match = gI_am_cheating == i;
    gI_am_cheating = i;
    if (match) {
        gI_am_cheating = 0;
        NewTextHeadupSlot(4, 0, 3000, -4, "Cheating Off");
    }
    F4Key();
}

// FUNCTION: CARMA2_HW 0x00444350
void C2_HOOK_FASTCALL SetFlag2(int i) {

    gAllow_car_flying = 1;
    if (gNet_mode == eNet_mode_none) {
        gCar_flying = !gCar_flying;
        if (gCar_flying) {
            NewTextHeadupSlot(4, 0, 500, -4, "We haev lift off!!");
        } else {
            NewTextHeadupSlot(4, 0, 500, -4, "Back down to Earth");
        }
        gAllow_car_flying = gCar_flying;
    } else {
        gCar_flying = 0;
        gAllow_car_flying = 0;
    }
}

// Key: 'f4'
// FUNCTION: CARMA2_HW 0x004414b0
void C2_HOOK_FASTCALL F4Key(void) {

    if (gI_am_cheating == 0xa11ee75d || (gI_am_cheating != 0x564e78b9 && gNet_mode != eNet_mode_none)) {
        char s[256];

        if (gINT_0068b8e4 == 1) {
            FUN_0045a670();
        }
        if (gINT_0068b8e4 == 2) {
            FUN_0045acf0();
        }
        if (gINT_0068b8e8) {
            ToggleJoystickHeadup();
        }
        if (PDKeyDown(0)) {
            gWhich_edit_mode -= 1;
            if (gWhich_edit_mode < 0) {
                gWhich_edit_mode = REC2_ASIZE(gEdit_funcs) - 1;
            }
        } else {
            gWhich_edit_mode += 1;
            if (gWhich_edit_mode >= REC2_ASIZE(gEdit_funcs)) {
                gWhich_edit_mode = 0;
            }
        }
        sprintf(s, "Edit mode: %s", gEdit_mode_names[gWhich_edit_mode]);
        NewTextHeadupSlot2(4, 0, 2000, -4, s, 0);
    } else {
        gWhich_edit_mode = 0;
    }
}

// FUNCTION: CARMA2_HW 0x004415e0
void C2_HOOK_FASTCALL EnsureSpecialVolumesHidden(void) {

}

// FUNCTION: CARMA2_HW 0x004415f0
void C2_HOOK_FASTCALL ShowSpecialVolumesIfRequ(void) {

}

void C2_HOOK_FASTCALL DoEditModeKey(int pIndex) {
    int modifiers;

    if (gI_am_cheating == 0xa11ee75d || (gI_am_cheating == 0x564e78b9 && gNet_mode == eNet_mode_none)) {
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
        if (gEdit_funcs[gWhich_edit_mode][pIndex][modifiers] != NULL) {
            gEdit_funcs[gWhich_edit_mode][pIndex][modifiers]();
        }
    } else {
        gWhich_edit_mode = 0;
    }
}

// Key: 'f5'
// FUNCTION: CARMA2_HW 0x00441600
void C2_HOOK_FASTCALL F5Key(void) {

    DoEditModeKey(0);
}

// Key: 'f6'
// FUNCTION: CARMA2_HW 0x00441680
void C2_HOOK_FASTCALL F6Key(void) {

    DoEditModeKey(1);
}

// Key: 'f7'
// FUNCTION: CARMA2_HW 0x00441700
void C2_HOOK_FASTCALL F7Key(void) {

    DoEditModeKey(2);
}

// Key: 'f8'
// FUNCTION: CARMA2_HW 0x00441780
void C2_HOOK_FASTCALL F8Key(void) {

    DoEditModeKey(3);
}

// Key: 'f10'
// FUNCTION: CARMA2_HW 0x00441800
void C2_HOOK_FASTCALL F10Key(void) {

    DoEditModeKey(4);
}

// Key: 'f11'
// FUNCTION: CARMA2_HW 0x00441880
void C2_HOOK_FASTCALL F11Key(void) {

    DoEditModeKey(5);
}

// Key: 'f12'
// FUNCTION: CARMA2_HW 0x00441900
void C2_HOOK_FASTCALL F12Key(void) {

    /* Do nothing */
}

// Key: '0'
// FUNCTION: CARMA2_HW 0x00441910
void C2_HOOK_FASTCALL NumberKey0(void) {

    DoEditModeKey(7);
}

// Key: '1'
// FUNCTION: CARMA2_HW 0x00441990
void C2_HOOK_FASTCALL NumberKey1(void) {

    DoEditModeKey(8);
}

// Key: '2'
// FUNCTION: CARMA2_HW 0x00441a10
void C2_HOOK_FASTCALL NumberKey2(void) {

    DoEditModeKey(9);
}

// Key: '3'
// FUNCTION: CARMA2_HW 0x00441a90
void C2_HOOK_FASTCALL NumberKey3(void) {

    DoEditModeKey(10);
}

// Key: '4'
// FUNCTION: CARMA2_HW 0x00441b10
void C2_HOOK_FASTCALL NumberKey4(void) {

    DoEditModeKey(11);
}

// Key: '5'
// FUNCTION: CARMA2_HW 0x00441b90
void C2_HOOK_FASTCALL NumberKey5(void) {

    DoEditModeKey(12);
}

// Key: '6'
// FUNCTION: CARMA2_HW 0x00441c10
void C2_HOOK_FASTCALL NumberKey6(void) {

    DoEditModeKey(13);
}

// Key: '7'
// FUNCTION: CARMA2_HW 0x00441c90
void C2_HOOK_FASTCALL NumberKey7(void) {

    DoEditModeKey(14);
}

// Key: '8'
// FUNCTION: CARMA2_HW 0x00441d10
void C2_HOOK_FASTCALL NumberKey8(void) {

    DoEditModeKey(15);
}

// Key: '9'
// FUNCTION: CARMA2_HW 0x00441d90
void C2_HOOK_FASTCALL NumberKey9(void) {

    DoEditModeKey(16);
}

// Key: 'kp_1'
// FUNCTION: CARMA2_HW 0x004443c0
void C2_HOOK_FASTCALL ToggleFlying(void) {

    if (gAllow_car_flying && gNet_mode == eNet_mode_none) {
        gCar_flying = !gCar_flying;
        NewTextHeadupSlot(4, 0, 500, -4, gCar_flying ? "We have lift off!!" : "Back down to Earth");
    } else {
        gCar_flying = 0;
    }
}

// Key: '-'
// FUNCTION: CARMA2_HW 0x004e4cd0
void C2_HOOK_FASTCALL ScreenSmaller(void) {

    if (gMap_view != 2) {
        gRender_indent += 1;
        if (gRender_indent > 8) {
            gRender_indent = 8;
        }
        AdjustRenderScreenSize();
    }
}

// Key: '='
// FUNCTION: CARMA2_HW 0x004e4d00
void C2_HOOK_FASTCALL ScreenLarger(void) {

    if (gMap_view != 2) {
        gRender_indent -= 1;
        if (gRender_indent < 0) {
            gRender_indent = 0;
        }
        AdjustRenderScreenSize();
    }
}

// Key: 'Del'
// FUNCTION: CARMA2_HW 0x00502e50
void C2_HOOK_FASTCALL BuyArmour(void) {

    if (gINT_0068b8e4 == 0 && gINT_0068b8e8 == 0) {
        BuyPSPowerup(0);
    }
}

// Key: 'End'
// FUNCTION: CARMA2_HW 0x00502fd0
void C2_HOOK_FASTCALL BuyPower(void) {

    if (gINT_0068b8e4 == 0 && gINT_0068b8e8 == 0) {
        if (gNet_mode != eNet_mode_none && gCurrent_net_game->type == eNet_game_type_foxy && gThis_net_player_index == gIt_or_fox) {
            NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_fox_cant_do_that));
        } else {
            BuyPSPowerup(1);
        }
    }
}

// Key: 'PgDn'
// FUNCTION: CARMA2_HW 0x00503030
void C2_HOOK_FASTCALL BuyOffense(void) {

    if (gINT_0068b8e4 == 0 && gINT_0068b8e8 == 0) {
        BuyPSPowerup(2);
    }
}

// Key: ' '
// FUNCTION: CARMA2_HW 0x0040e430
void C2_HOOK_FASTCALL ViewNetPlayer(void) {

    NOT_IMPLEMENTED();
}

// Key: '`'
// FUNCTION: CARMA2_HW 0x004448f0
void C2_HOOK_FASTCALL UserSendMessage(void) {

    if (gNet_mode != eNet_mode_none && gCurrent_net_game->options.enable_text_messages) {
        gEntering_message = 1;
    }
}

// Key: 't'
// FUNCTION: CARMA2_HW 0x004945f0
void C2_HOOK_FASTCALL ToggleTargetLock(void) {

    if (gNet_mode != eNet_mode_none) {
        if (gCurrent_net_game->type == eNet_game_type_foxy) {
            return;
        }
        if (!gCurrent_net_game->options.show_powerups_on_map) {
            return;
        }
    }
    if (gTarget_lock_car_1 == NULL) {
        NewTextHeadupSlot2(4, 0, 1500, -4, GetMiscString(eMiscString_no_target_to_lock_onto), 0);
    } else {
        gTarget_lock_enabled = !gTarget_lock_enabled;
        if (gTarget_lock_enabled) {
            const char* driver_name;
            char s[256];

            if (gNet_mode == eNet_mode_none) {
                driver_name = GetDriverName(gTarget_lock_car_2->car_ID >> 8, gTarget_lock_car_2->car_ID & 0xff);
            } else {
                driver_name = NetPlayerFromCar(gTarget_lock_car_2)->player_name;
            }
            sprintf(s, "%s %s", driver_name, GetMiscString(eMiscString_locked_onto));
            NewTextHeadupSlot2(4, 0, 1500, -4, s, 0);
        } else {
            NewTextHeadupSlot2(4, 0, 1500, -4, GetMiscString(eMiscString_target_lock_off), 0);
        }
    }
}

// Key: 'y'
// FUNCTION: CARMA2_HW 0x00494700
void C2_HOOK_FASTCALL CycleTargetLock(void) {

    if (gNet_mode == eNet_mode_none || (gCurrent_net_game->type != eNet_game_type_foxy && gCurrent_net_game->options.show_players_on_map)) {
        int category;
        int car_index;
        int car_count;
        tCar_spec* car;

        category = gNet_mode != eNet_mode_none ? eVehicle_net_player : eVehicle_opponent;
        if (gTarget_lock_car_1 == NULL) {
            if (category == eVehicle_self) {
                gTarget_lock_car_1 = &gProgram_state.current_car;
            } else {
                gTarget_lock_car_1 = GetCarSpec(category, 0);
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
                c = &gProgram_state.current_car;
            } else {
                c = GetCarSpec(category, car_index);
            }
            if (c == gTarget_lock_car_1) {
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
                    car = &gProgram_state.current_car;
                } else {
                    car = GetCarSpec(category, car_index);
                }
                if (car_index == car_count - 1) {
                    car_index = 0;
                } else {
                    car_index += 1;
                }
                if (car == gTarget_lock_car_1 || !car->knackered) {
                    break;
                }
            }
            if (gNet_mode == eNet_mode_none) {
                if (gCurrent_race.race_spec->race_type == kRaceType_Cars && !car->is_race_goal) {
                    continue;
                }
                break;
            }
            if (gCurrent_net_game->type != eNet_game_type_6) {
                break;
            }
            if (!NetPlayerFromCar(car)->field_0x80) {
                break;
            }
        }
        gTarget_lock_enabled = 0;
        gTarget_lock_car_1 = car;
        gTarget_lock_car_2 = car;
    }
}


// Key: ';'
// FUNCTION: CARMA2_HW 0x004da9d0
void C2_HOOK_FASTCALL ToggleInventory(void) {

    gInventory_cycling = !gInventory_cycling;
    if (gInventory_cycling) {
        gInventory_timeout = GetTotalTime() + 3000;
    }
}

// Key: '['
// FUNCTION: CARMA2_HW 0x004dab80
void C2_HOOK_FASTCALL LeftInventory(void) {

    NOT_IMPLEMENTED();
}

// Key: ']'
// FUNCTION: CARMA2_HW 0x004daa00
void C2_HOOK_FASTCALL RightInventory(void) {

    NOT_IMPLEMENTED();
}

// Key: '*' (keypad)
// FUNCTION: CARMA2_HW 0x00444ed0
void C2_HOOK_FASTCALL ChangeCameraTypeInGame(void) {

    if (!gAction_replay_mode) {

        ChangeCameraType();
        NewTextHeadupSlot2(4, 0, 1000, -4, GetMiscString(eMiscString_standard_cam) + gAction_replay_mode, 0);
    }
}

// FUNCTION: CARMA2_HW 0x0040e7f0
void C2_HOOK_FASTCALL ChangeCameraType(void) {

    NOT_IMPLEMENTED();
}

// Key: 'c'
// FUNCTION: CARMA2_HW 0x0040e900
void C2_HOOK_FASTCALL ToggleCockpit(void) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00444ea0
void C2_HOOK_FASTCALL DisposeAbuseomatic(void) {
    int i;

    for (i = 0; i < REC2_ASIZE(gAbuse_text); i++) {
        if (gAbuse_text[i] != NULL) {
            BrMemFree(gAbuse_text[i]);
        }
    }
}

// FUNCTION: CARMA2_HW 0x00443c50
void C2_HOOK_FASTCALL CheckForCheatingGits(void) {
    tU32* keys;

    keys = KevKeyService();
    if (keys[0] == 0x616fb8ea && keys[0] == 0x7c6100a8) {
        ToggleSoundEnable();
        gProgram_state.game_completed = 1;
        DRS3StartSound(gEffects_outlet, eSoundId_FlaskGone);
    }
}

// FUNCTION: CARMA2_HW 0x00443c90
void C2_HOOK_FASTCALL CheckKevKeys(void) {
    int i;
    tU32* value;
    char s[128];

    value = KevKeyService();
    if (value[0] == 0) {
        return;
    }

    for (i = 0; gKev_keys[i].action_proc != NULL; i++) {
        if (gKev_keys[i].code == value[0] && gKev_keys[i].code2 == value[1]) {
            break;
        }
    }

    if (gKev_keys[i].action_proc != NULL) {
        if (gNet_mode == eNet_mode_none) {
            gKev_keys[i].action_proc(gKev_keys[i].num);
        } else if (gKev_keys[i].num == 0xa11ee75d) {
            strcpy(s, gNet_players[gThis_net_player_index].player_name);
            strcat(s, " ");
            strcat(s, GetMiscString(225));
            NetSendHeadupToEverybody(s);
            gKev_keys[i].action_proc(gKev_keys[i].num);
        } else {
            strcpy(s, gNet_players[gThis_net_player_index].player_name);
            strcat(s, " ");
            strcat(s, GetMiscString(224));
            NetSendHeadupToAllPlayers(s);
        }
    }
}

// FUNCTION: CARMA2_HW 0x00442e90
void C2_HOOK_FASTCALL CheckSystemKeys(int pRacing) {
    tU32 start_menu_time;

    start_menu_time = PDGetTotalTime();
    CheckQuit();
    AddLostTime(PDGetTotalTime() - start_menu_time);
    CheckToggles(pRacing);
    if (pRacing & !gAction_replay_mode) {
        CheckOtherRacingKeys();
    }
}

void C2_HOOK_FASTCALL CheckToggles(int pRacing) {
    int i;
    int new_state;

    for (i = 0; i < REC2_ASIZE(gToggle_array); i++) {
        if (!pRacing && gToggle_array[i].in_game_only) {
            continue;
        }
        if ((gTyping || gEntering_message) && gToggle_array[i].key2 == -2) {
            continue;
        }
        new_state = 0;
        if (gToggle_array[i].key1 == -2 || KeyIsDownNoMouldiness(gToggle_array[i].key1)) {
            if (gToggle_array[i].key2 == -2 && gToggle_array[i].exact_modifiers) {
                if (!PDKeyDown(0) && !PDKeyDown(1) && !PDKeyDown(2) && !PDKeyDown(3)) {
                    new_state = 1;
                }
            } else {
                if (KeyIsDown(gToggle_array[i].key2)) {
                    new_state = 1;
                }
            }
        }
        if (gToggle_array[i].on_last_time != new_state) {
            gToggle_array[i].on_last_time = new_state;
            if (new_state) {
                gToggle_array[i].action_proc();
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x00443ba0
void C2_HOOK_FASTCALL CheckHorn3D(tCar_spec* pCar) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00442140
int C2_HOOK_FASTCALL CarWorldOffFallenCheckThingy(tCar_spec* pCar, int pCheck_around) {

    if (gCar_flying && pCar != NULL && pCar->driver == eDriver_local_human) {
        return 0;
    }
    if (gAction_replay_mode) {
        return 0;
    }
    if (pCar->number_of_wheels_on_ground != 0) {
        int i;

        for (i = pCar->collision_info->box_face_start; i < pCar->collision_info->box_face_end; i++) {
            const tFace_ref *face_ref;
            face_ref = &gFace_list__car[i];
            if (face_ref->material != NULL && face_ref->material->identifier != NULL && face_ref->material->identifier[0] == '^') {
                return 1;
            }
        }
        return 0;
    } else {
        br_vector3 car_pos;
        br_vector3 offset_c;
        br_vector3 offset_w;
        int result;

        BrVector3Copy(&car_pos, &pCar->car_master_actor->t.t.translate.t);
        car_pos.v[1] += 0.5f;
        if (FindYVerticallyBelow2(&car_pos) >= -100.f) {
            if (gMaterial_below->identifier != NULL && gMaterial_below->identifier[0] == '^') {
                return 1;
            }
            return 0;
        } else {
            BrVector3Set(&offset_c, 0.f, 1.f, 0.f);
            BrMatrix34ApplyV(&offset_w, &offset_c, &pCar->car_master_actor->t.t.mat);
            BrVector3Add(&car_pos, &pCar->car_master_actor->t.t.translate.t, &offset_w);
            if (FindYVerticallyBelow2(&car_pos) >= -100.f) {
                return 0;
            }
            if (!pCheck_around) {
                return 1;
            }
            pCar->car_master_actor->t.t.translate.t.v[0] += .05f;
            result = CarWorldOffFallenCheckThingy(pCar, 0);
            pCar->car_master_actor->t.t.translate.t.v[0] -= .05f;
            if (!result) {
                return 0;
            }
            pCar->car_master_actor->t.t.translate.t.v[2] += .05f;
            result = CarWorldOffFallenCheckThingy(pCar, 0);
            pCar->car_master_actor->t.t.translate.t.v[2] -= .05f;
            if (!result) {
                return 0;
            }
            return 1;
        }
    }
}

// FUNCTION: CARMA2_HW 0x00442130
int C2_HOOK_FASTCALL HasCarFallenOffWorld(tCar_spec* pCar) {

    return CarWorldOffFallenCheckThingy(pCar, 1);
}

void C2_HOOK_FASTCALL CheckHornLocal(tCar_spec* pCar) {

    if (pCar->keys.horn && pCar->horn_sound_tag == 0) {
        pCar->horn_sound_tag = DRS3StartSound(gEffects_outlet, eSoundId_Horn);
    } else if (!pCar->keys.horn && pCar->horn_sound_tag != 0) {
        while (S3SoundStillPlaying(pCar->horn_sound_tag) != 0) {
            DRS3StopSound(pCar->horn_sound_tag);
            DRS3StopOutletSound(gEffects_outlet);
        }
        if (!S3SoundStillPlaying(pCar->horn_sound_tag)) {
            pCar->horn_sound_tag = 0;
        }
    }
}

void C2_HOOK_FASTCALL CheckHorns(void) {
    int i;

    if (gNet_mode != eNet_mode_none) {
        for (i = 0; i < gNumber_of_net_players; i++) {
            CheckHorn3D(gNet_players[i].car);
        }
    } else {
        CheckHornLocal(&gProgram_state.current_car);
    }
}

void C2_HOOK_FASTCALL CheckMapRenderMove(void) {
    int amount;
    float old_x;
    float old_y;

    old_y = gMap_render_y;
    old_x = gMap_render_x;
    if (gMap_view == 2) {
        amount = (int)(gFrame_period / 10.f);
        KeyIsDown(8);
        if (KeyIsDown(31)) {
            gMap_render_y -= (float)amount;
        } else if (KeyIsDown(32)) {
            gMap_render_y += (float)amount;
        }
        if (KeyIsDown(33)) {
            gMap_render_x -= (float)amount;
        } else if (KeyIsDown(34)) {
            gMap_render_x += (float)amount;
        }
        if (gMap_render_x != old_x || gMap_render_y != old_y) {
            SetIntegerMapRenders();
            if (gMap_render_x_i < gCurrent_graf_data->map_render_x_marg) {
                gMap_render_x = (float)((gCurrent_graf_data->map_render_x_marg + 3) & ~3);
            }
            if (gMap_render_y_i < gCurrent_graf_data->map_render_y_marg) {
                gMap_render_y = (float)((gCurrent_graf_data->map_render_y_marg + 1) & ~1);
            }
            if (gBack_screen->width - gCurrent_graf_data->map_render_x_marg - gMap_render_width_i < gMap_render_x_i) {
                gMap_render_x = (float)((gBack_screen->width - gCurrent_graf_data->map_render_x_marg - gMap_render_width_i) & ~3);
            }
            if (gBack_screen->height - gCurrent_graf_data->map_render_y_marg - gMap_render_height_i < gMap_render_y_i) {
                gMap_render_y = (float)((gBack_screen->height - gCurrent_graf_data->map_render_y_marg - gMap_render_height_i) & ~1);
            }
            SetIntegerMapRenders();
            AdjustRenderScreenSize();
        }
    }
}

int C2_HOOK_FASTCALL TradeInAPO(tS32 cost) {

    if (cost > 0) {
        while (cost > 0
                && (gProgram_state.current_car.power_up_levels[0] != 0
                        || gProgram_state.current_car.power_up_levels[1] != 0
                        || gProgram_state.current_car.power_up_levels[2] != 0)) {
            int type;
            int value;

            type = IRandomBetween(0, 2);
            if (gProgram_state.current_car.power_up_levels[type] != 0) {
                gProgram_state.current_car.power_up_levels[type] -= 1;

                if (gNet_mode == eNet_mode_none) {
                    value = gTrade_in_value_APO.initial[gProgram_state.skill_level];
                }
                else {
                    value = gTrade_in_value_APO.initial_network[gCurrent_net_game->type];
                }
                cost -= value;
                if (gNet_mode == eNet_mode_none) {
                    value = gTrade_in_value_APO.initial[gProgram_state.skill_level];
                }
                else {
                    value = gTrade_in_value_APO.initial_network[gCurrent_net_game->type];
                }
                gProgram_state.credits += value;
            }
        }
    }
    if (cost < 0) {
        cost = 0;
    }
    return cost;
}

void C2_HOOK_FASTCALL RecoverCar(void) {

    if (gNet_mode == eNet_mode_none || gPalette_fade_time == 0) {
        gRecover_car = 1;
    }
    gProgram_state.current_car.time_to_recover = 0;
}

void C2_HOOK_FASTCALL SetQuickRecovery(void) {

    if (gNet_mode == eNet_mode_none) {
        RecoverCar();
    } else {
        if (gProgram_state.current_car.time_to_recover == 0 && !CheckRecoverCost()) {
            NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_cant_afford_to_recover));
            DoFancyHeadup(27);
            KnackerThisCar(&gProgram_state.current_car);
            SendGameplayToHost(3, 0, 0, 0, 0);
        } else {
            if (gProgram_state.current_car.time_to_recover == 0 || GetRaceTime() + 500 < gProgram_state.current_car.time_to_recover) {
                gProgram_state.current_car.time_to_recover = GetRaceTime() + 500;
                gCancel_recover = 0;
            }
        }
    }
}

void C2_HOOK_FASTCALL CheckForBeingOutOfThisWorld(void) {
    // GLOBAL: CARMA2_HW 0x0067c3f4
    static tU32 the_time;
    // GLOBAL: CARMA2_HW 0x0067c480
    static tU32 sLast_check;

    the_time = PDGetTotalTime();
    if (gProgram_state.current_car.disabled) {
        gRecover_timer = 0;
        return;
    }

    if (gRecover_timer != 0) {
        if ((!gProgram_state.current_car.frame_collision_flag && gProgram_state.current_car.number_of_wheels_on_ground == 0) || IsCarInTheSea()) {
            gRecover_timer -= gFrame_period;
            if (gRecover_timer <= 0 || IsCarInTheSea() == 2) {
                gRecover_timer = 0;
                SetQuickRecovery();
                gHad_auto_recover = 1;
            }
            return;
        } else {
            gRecover_timer = 0;
        }
    }

    if (the_time - sLast_check > 200) {
        sLast_check = the_time;
        if (HasCarFallenOffWorld(&gProgram_state.current_car)) {
            gRecover_timer = 3000;
        }
    }
    if (IsCarInTheSea() && gRecover_timer == 0) {
        gRecover_timer = 3000;
    }
}

void C2_HOOK_FASTCALL SortOutRecover(tCar_spec* pCar) {
    int the_time;
    int val;
    // GLOBAL: CARMA2_HW 0x0067c494
    static int old_time;

    the_time = GetRaceTime() - gPalette_fade_time;
    if (the_time < 0) {
        gPalette_fade_time = 0;
        old_time = 0;
    }
    if (the_time < 500) {
        val = 256 - (the_time * 256) / 500;
    } else {
        if (old_time < 500) {
            FlipUpCar(pCar);
            PipeSingleSpecial(ePipe_special_fade);
        }
        val = ((the_time - 1000) * 256) / 500;
        if (val >= 256) {
            val = 256;
            gPalette_fade_time = 0;
            old_time = 0;
            SetCollisionInfoChildsDoNothing(pCar->collision_info, 0);
        } else if (!pCar->collision_info->disable_move_rotate) {
            SetCollisionInfoChildsDoNothing(pCar->collision_info, 1);
        }
    }
    if (val <= 0) {
        val = 0;
    }
    SetFadedPalette(val);
    old_time = the_time;
}

void C2_HOOK_FASTCALL SetFlipUpCar(tCar_spec* pCar) {

    if (gNet_mode != eNet_mode_none && pCar != NULL && pCar->driver == eDriver_local_human) {
        DisableCar(pCar);
        gPalette_fade_time = GetRaceTime();
        NetPlayerStatusChanged(ePlayer_status_recovering);
    } else {
        FlipUpCar(pCar);
    }
}

// FUNCTION: CARMA2_HW 0x00442f90
void C2_HOOK_FASTCALL CheckOtherRacingKeys(void) {
    int i;
    int new_level;
    int old_level;
    char s[256];
    tU32 cost;
    br_scalar ts;
    tCar_spec* car;
    float bodywork_repair_amount;
    float cost_float;
    // GLOBAL: CARMA2_HW 0x0067c484
    static tU32 total_repair_cost;
    // GLOBAL: CARMA2_HW 0x0067c488
    static tS3_sound_tag sound_tag;
    // GLOBAL: CARMA2_HW 0x0067c3fc
    static br_scalar amount;
    // GLOBAL: CARMA2_HW 0x0067c48c
    static int NeedToExpandBoundingBox;
    // GLOBAL: CARMA2_HW 0x0067c3c0
    static int total_difference;
    // GLOBAL: CARMA2_HW 0x0067c490
    static int stopped_repairing;

    (void)total_difference;

    car = GetCarSpec(eVehicle_self, 0);
    CheckMapRenderMove();
    CheckHorns();
    CheckForBeingOutOfThisWorld();
    if (gPalette_fade_time != 0) {
        SortOutRecover(car);
    } else if (gNet_mode != eNet_mode_none && NetGetPlayerStatus() == ePlayer_status_recovering) {
        NetPlayerStatusChanged(ePlayer_status_racing);
    }

    if ((gAuto_repair || KeyIsDown(45)) && !gRace_finished && !gProgram_state.current_car.knackered && !gWait_for_it && !gEntering_message) {
        if (gProgram_state.current_car.car_crush_spec != NULL && gProgram_state.current_car.car_crush_spec->field_0x4b8 != 0) {
            FlipUpCar(&gProgram_state.current_car);
        }
        if (!gAuto_repair && gRepair_last_time == 0 && GetTotalTime() - gLast_repair_time < 1200) {
            gAuto_repair = 1;
        }
        gLast_repair_time = GetTotalTime();
        gRepair_last_time = 1;
        if (!NeedToExpandBoundingBox) {
            bodywork_repair_amount = RepairCar(gProgram_state.current_car.car_ID, gFrame_period, &amount);
            NeedToExpandBoundingBox = bodywork_repair_amount > 0.f;
            cost = 0;
            for (i = 0; i < REC2_ASIZE(gProgram_state.current_car.damage_units); i++) {
                tDamage_unit* unit = &gProgram_state.current_car.damage_units[i];

                old_level = unit->damage_level;
                if (amount == 0.0f) {
                    new_level = 0;
                } else {
                    new_level = (int)((float)unit->damage_level - floorf((float)unit->damage_level * bodywork_repair_amount / amount));
                }
                if (new_level < 0) {
                    unit->damage_level = 0;
                } else if (new_level >= 100) {
                    unit->damage_level = 99;
                } else {
                    unit->damage_level = new_level;
                }
                unit->smoke_last_level = unit->damage_level;
                if (gNet_mode == eNet_mode_none) {
                    ts = gRepair_cost.initial[gProgram_state.skill_level];
                } else {
                    ts = gRepair_cost.initial_network[gCurrent_net_game->type];
                }
                cost = (int)((float)cost + (float)(old_level - unit->damage_level) * ts);
                total_difference += old_level - new_level;
            }
            total_difference %= 100;
            cost = 10 * (cost / 10);
            if (((total_repair_cost == 0 && cost != 0) || bodywork_repair_amount != 0.0f) && sound_tag == 0) {
                sound_tag = DRS3StartSound(gCar_outlet, eSoundId_DamageRepair);
            }
            StopCarSmoking(&gProgram_state.current_car);
            if (cost == 0 && bodywork_repair_amount == 0.0) {
                gAuto_repair = 0;
            }
            if (!gFree_repairs) {
                if (cost > (tU32)gProgram_state.credits) {
                    TradeInAPO(cost);
                }
                SpendCredits(cost);
            }
            total_repair_cost += cost;
            if (total_repair_cost) {
                if (gFree_repairs) {
                    NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_repairing_for_free));
                } else {
                    sprintf(s, "%s %d", GetMiscString(eMiscString_repair_cost_colon), total_repair_cost);
                    NewTextHeadupSlot(4, 0, 1000, -4, s);
                }
            }
        }
    } else {
        (void)stopped_repairing;

        gRepair_last_time = 0;
        stopped_repairing = 0;
        total_repair_cost = 0;
        total_difference = 0;
        if (sound_tag != 0) {
            while (S3SoundStillPlaying(sound_tag)) {
                DRS3StopSound(sound_tag);
            }
            sound_tag = 0;
        }
    }
    if (NeedToExpandBoundingBox) {
        NeedToExpandBoundingBox = gProgram_state.current_car.car_crush_spec->expand_bounding_box;
    }
    if (gToo_poor_for_recovery_timeout) {
        if (PDGetTotalTime() < gToo_poor_for_recovery_timeout) {
            if (PDKeyDown(51)) {
                NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_cant_afford_to_recover));
                DoFancyHeadup(27);
                KnackerThisCar(&gProgram_state.current_car);
                SendGameplayToHost(3, 0, 0, 0, 0);
            }
        } else {
            gToo_poor_for_recovery_timeout = 0;
        }
    }

    if (!gRecover_car || gProgram_state.current_car.knackered) {
        gHad_auto_recover = 0;
    } else {
        gRecover_timer = 0;
        SetFlipUpCar(car);
        if (gNet_mode != eNet_mode_none) {
            NewTextHeadupSlot(4, 0, 1500, -4, " ");
        }
        if (gRecovery_voucher_count != 0
                || (gNet_mode != eNet_mode_none && NetPlayerFromCar(&gProgram_state.current_car) != NULL && NetPlayerFromCar(&gProgram_state.current_car)->field_0x80)) {
            if (gRecovery_voucher_count != 0) {
                gRecovery_voucher_count -= 1;
            }
            sprintf(s, "%s", GetMiscString(eMiscString_recovering_for_free));
            NewTextHeadupSlot(4, 0, 1500, -4, s);
        } else {
            if (gNet_mode == eNet_mode_none) {

                if (!gProgram_state.current_car.knackered) {

                    if (gNet_mode == eNet_mode_none) {
                        cost_float = gRecovery_cost.initial[gProgram_state.skill_level];
                    } else {
                        cost_float = gRecovery_cost.initial_network[gCurrent_net_game->type];
                    }
                    if (cost_float > (float)gProgram_state.credits) {
                        if (gNet_mode == eNet_mode_none) {
                            cost_float = gRecovery_cost.initial[gProgram_state.skill_level];
                        } else {
                            cost_float = gRecovery_cost.initial_network[gCurrent_net_game->type];
                        }
                        TradeInAPO((int)cost_float);
                    }
                }
                if (gNet_mode == eNet_mode_none) {
                    cost_float = gRecovery_cost.initial[gProgram_state.skill_level];
                } else {
                    cost_float = gRecovery_cost.initial_network[gCurrent_net_game->type];
                }
                SpendCredits((int)cost_float);
            }
            if (gNet_mode == eNet_mode_none) {
                cost_float = gRecovery_cost.initial[gProgram_state.skill_level];
            } else {
                cost_float = gRecovery_cost.initial_network[gCurrent_net_game->type];
            }
            sprintf(s, "%s %d", GetMiscString(eMiscString_recovery_cost_colon), (int)cost_float);
            NewTextHeadupSlot(4, 0, 1500, -4, s);
        }
        CancelPendingCunningStunt();
        PipeSingleSpecial(ePipe_special_fade);
    }
    gRecover_car = 0;
}

// FUNCTION: CARMA2_HW 0x004429b0
void C2_HOOK_FASTCALL FlipUpCar(tCar_spec* pCar_spec) {
    int new_pos;
    int i;
    int j;
    int k;

    if (pCar_spec != NULL && pCar_spec->driver == eDriver_local_human && gNet_mode == eNet_mode_none) {
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
}

// FUNCTION: CARMA2_HW 0x00442e10
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

// FUNCTION: CARMA2_HW 0x00444710
void C2_HOOK_FASTCALL ResetRecoveryVouchers(void) {

    gRecovery_voucher_count = 0;
}

// FUNCTION: CARMA2_HW 0x00444910
void C2_HOOK_FASTCALL EnterUserMessage(void) {
    // GLOBAL: CARMA2_HW 0x00590f64
    static int last_key = -1;
    // GLOBAL: CARMA2_HW 0x0067c49c
    static int about_to_die;
    // GLOBAL: CARMA2_HW 0x0067c454
    static tU32 next_time;
    char* the_message;
    int len;
    int the_key;
    int abuse_num;

    if (!gEntering_message) {
        return;
    }
    if (gNet_mode == eNet_mode_none) {
        return;
    }
    if (!gCurrent_net_game->options.enable_text_messages) {
        return;
    }
    the_key = PDAnyKeyDown();
    if (gEntering_message == 1) {
        if (the_key != -1) {
            return;
        }
        gEntering_message = 2;
        PDClearKeyboardBuffer();
    }
    if (about_to_die) {
        if (the_key != -1) {
            return;
        }
        gEntering_message = 0;
        about_to_die = 0;
        return;
    }
    if (the_key == last_key) {
        if (next_time < PDGetTotalTime()) {
            next_time += 100;
        } else {
            the_key = -1000;
        }
    } else {
        last_key = the_key;
        next_time = PDGetTotalTime() + 500;
    }
    switch (the_key) {
    case -1:
    case 0:
        break;
    case 2:
    case 3:
    case 53:
    case 63:
        about_to_die = 1;
        break;
    case 50:
    case 65:
    case 70:
        len = strlen(&gString[20]);
        if (len > 0) {
            gString[20 + len - 1] = '\0';
        }
        break;
    case 51:
    case 52:
        len = strlen(gNet_players[gThis_net_player_index].player_name);
        if (len <= 18) {
            the_message = gString + 18 - len;
            strcpy(the_message, gNet_players[gThis_net_player_index].player_name);
            the_message[len + 0] = ':';
            the_message[len + 1] = ' ';
            gString[REC2_ASIZE(gString) - 1] = '\0';
            NetSendHeadupToAllPlayers(the_message);
            gString[20] = '\0';
            NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_message_sent));
            about_to_die = 1;
        }
        break;
    default:
        if (gKey_mapping[71] == the_key) {
            about_to_die = 1;
        } else if (the_key <= 74 || the_key >= 106) {
            for (;;) {
                int raw_ch = PDGetKeyboardCharacter();
                int ch;
                if (raw_ch == '\0') {
                    break;
                }
                ch = PDConvertToASCIILessThan128(raw_ch);
                len = strlen(&gString[20]);
                if (len < 64 - 1) {
                    gString[20 + len] = ch;
                    if (ch < gFonts[4].offset || ch >= gFonts[4].offset + gFonts[4].num_entries) {
                        gString[20 + len] = '\0';
                    }
                    gString[20 + len + 1] = '\0';
                    dr_dprintf("NETMESSAGE: '%s'", &gString[20]);
                }
            }
        } else if (the_key >= 81 && the_key <= 90) {
            if (the_key == 81) {
                abuse_num = 9;
            } else {
                abuse_num = the_key - 82;
            }
            if (gAbuse_text[abuse_num] != NULL) {
                strcpy(&gString[20], gAbuse_text[abuse_num]);
            }
        } else {
            gEntering_message = 0;
        }
    }
}

void C2_HOOK_FASTCALL MapChanged(void) {

    InitMap();
    SaveOptions();
}

// FUNCTION: CARMA2_HW 0x00497620
void C2_HOOK_FASTCALL CheckMapMoveKeys(int pKey0) {

    if (KeyIsDown(31) && pKey0) {
        gHeadup_map_y_float -= (float)gFrame_period / 20.f;
        if (gHeadup_map_y_float < 0.f) {
            gHeadup_map_y_float = 0.f;
        }
        MapChanged();
    } else if (KeyIsDown(32) && pKey0) {
        gHeadup_map_y_float += (float)gFrame_period / 20.f;
        if (gHeadup_map_y_float + gHeadup_map_h_float > (float)gBack_screen->height) {
            gHeadup_map_y_float = (float)(gBack_screen->height - gHeadup_map_h);
        }
        MapChanged();
    }
    if (KeyIsDown(33) && pKey0) {
        gHeadup_map_x_float -= (float)gFrame_period / 20.f;
        if (gHeadup_map_x_float < 0.f) {
            gHeadup_map_x_float = 0.f;
        }
        MapChanged();
    } else if (KeyIsDown(34) && pKey0) {
        gHeadup_map_x_float += (float)gFrame_period / 20.f;
        if (gHeadup_map_x_float + gHeadup_map_w_float + 8.f > (float)gBack_screen->width) {
            gHeadup_map_x_float = (float)(gBack_screen->width - gHeadup_map_w - 8);
        }
        MapChanged();
    }
}

// FUNCTION: CARMA2_HW 0x00444270
void C2_HOOK_FASTCALL PollCameraControls(tU32 pCamera_period) {
    int down0;

    down0 = PDKeyDown(0);
    if (down0 || PDKeyDown(1)) {
        CheckMapMoveKeys(down0);
        gCamera_key_flags.field_0x0_bit1 = 0;
        gCamera_key_flags.field_0x0_bit2 = 0;
        gCamera_key_flags.field_0x0_bit3 = 0;
        gCamera_key_flags.field_0x0_bit4 = 0;
    } else {
        gCamera_key_flags.field_0x0_bit1 = KeyIsDownNoMouldiness(31);
        gCamera_key_flags.field_0x0_bit2 = KeyIsDownNoMouldiness(32);
        gCamera_key_flags.field_0x0_bit3 = KeyIsDownNoMouldiness(33);
        gCamera_key_flags.field_0x0_bit4 = KeyIsDownNoMouldiness(34);
        gCamera_key_flags.field_0x0_bit0 = PDKeyDown(2);
    }
}

void C2_HOOK_FASTCALL ExplodeCar(tCar_spec* pCar) {
    br_vector3 tv;
    br_vector3 v;
    br_vector3 pos;

    pCar->collision_info->field_0x49c = 0;
    BrVector3Set(&tv,
        pCar->collision_info->cmpos.v[0],
        pCar->collision_info->cmpos.v[1],
        pCar->collision_info->bb2.min.v[2] + .3f * (pCar->collision_info->bb2.max.v[2] - pCar->collision_info->bb2.min.v[2]));
    BrVector3Scale(&v, &pCar->collision_info->v, WORLD_SCALE);
    BrMatrix34ApplyP(&pos, &tv, &pCar->car_master_actor->t.t.mat);
    CreatePuffOfSmoke(&pos, &v, 1.f, 1.f, 7);

    tv.v[2] = pCar->collision_info->bb2.min.v[2] + .7f * (pCar->collision_info->bb2.max.v[2] - pCar->collision_info->bb2.min.v[2]);
    BrMatrix34ApplyP(&pos, &tv, &pCar->car_master_actor->t.t.mat);
    CreatePuffOfSmoke(&pos, &v, 1.f, 1.f, 7);
    if (pCar->car_crush_spec->field_0x4b8) {
        WeldCar(pCar);
    }
    DisableCar(pCar);
}

// FUNCTION: CARMA2_HW 0x00442500
void C2_HOOK_FASTCALL CheckRecoveryOfCars(tU32 pEndFrameTime) {

    if (gProgram_state.current_car.time_to_recover != 0) {
        if (gProgram_state.current_car.knackered) {
            gProgram_state.current_car.time_to_recover = 0;
        } else {
            int time;
            char str[256];

            time = (gProgram_state.current_car.time_to_recover - pEndFrameTime + 1000) / 1000;
            sprintf(str, "%s %d %s",
                GetMiscString(eMiscString_recovery_in),
                time,
                GetMiscString(time > 1 ? eMiscString_seconds : eMiscString_second));
            if (!gToo_late && gCancel_recover) {
                NewTextHeadupSlot2(4, 0, 2000, -1, str, 0);
            }
            if (pEndFrameTime >= gProgram_state.current_car.time_to_recover) {
                RecoverCar();
            }
        }
    }
    if (gNet_mode != eNet_mode_none) {
        int i;

        for (i = 0; i < gNumber_of_net_players; i++) {
            if (i != gThis_net_player_index && gNet_players[i].car->time_to_recover != 0 && pEndFrameTime >= gNet_players[i].car->time_to_recover) {
                gNet_players[i].player_status = ePlayer_status_recovering;
                gNet_players[i].car->collision_info->field_0x261 = 0xff;
                gNet_players[i].car->collision_info->message_time = pEndFrameTime;
                ExplodeCar(gNet_players[i].car);
                gNet_players[i].car->time_to_recover = 0;
            }
        }
    }
}

void C2_HOOK_FASTCALL BrakeInstantly(void) {
    int i;
    br_scalar speed_squared;

    gProgram_state.current_car.revs = 0.f;
    speed_squared = BrVector3LengthSquared(&gProgram_state.current_car.collision_info->v);
    if (gProgram_state.current_car.number_of_wheels_on_ground != 0 && speed_squared > .0001f) {
        PratcamEvent(41);
        for (i = 0; i < 5; i++) {
            DRS3StartSound(gCar_outlet, eSoundId_TyreScreeching + i);
        }
    }
    if (speed_squared > .1f) {
        PDPlayFFBEffect("Instant_Handbrake");
    }
    BrVector3Set(&gProgram_state.current_car.collision_info->v, 0.f, 0.f, 0.f);
}

// FUNCTION: CARMA2_HW 0x00443e80
void C2_HOOK_FASTCALL PollCarControls(tU32 pTime_difference) {
    tCar_controls keys;
    tJoystick joystick;
    tCar_spec* c;

    c = &gProgram_state.current_car;
    CheckKeysForMouldiness();
    memset(&keys, 0, sizeof(tCar_controls));
    joystick.left = -1;
    joystick.right = -1;
    joystick.acc = -1;
    joystick.dec = -1;
    if (!gEntering_message) {

        if (gKey_mapping[47] < 143 && gKey_mapping[48] < 143) {
            if (KeyIsDownNoMouldiness(47)) {
                keys.left = 1;
            }
            if (KeyIsDownNoMouldiness(48)) {
                keys.right = 1;
            }
        }
        if (!(keys.left || keys.right)) {
            if (PDIsJoystickDPadEnabled()) {
                if (gJoy_array[0] > 10) {
                    keys.left = 1;
                }
                if (gJoy_array[1] > 10) {
                    keys.right = 1;
                }
            } else {
                joystick.left = gJoy_array[0];
                joystick.right = gJoy_array[1];
                if (gJoy_array[0] < 0 && gJoy_array[1] < 0) {
                    joystick.left = 0;
                }
            }
        }
        if (KeyIsDownNoMouldiness(13)) {
            keys.holdw = 1;
        }
        if (KeyIsDownNoMouldiness(54) || gRace_finished) {
            if (gInstant_handbrake && !gRace_finished) {
                BrakeInstantly();
            } else {
                keys.brake = 1;
            }
        }
        if (KeyIsDownNoMouldiness(49) && !gRace_finished && !c->knackered && !gWait_for_it) {
            keys.acc = 1;
        }
        if (!keys.acc) {
            if (PDIsJoystickDPadEnabled()) {
                if (gJoy_array[2] > 10) {
                    keys.acc = 1;
                }
            } else {
                if (HasCurrentJoystick()) {
                    joystick.acc = gJoy_array[2];
                }
            }
        }
        if (KeyIsDownNoMouldiness(50) && !gRace_finished && !c->knackered && !gWait_for_it) {
            keys.dec = 1;
        }
        if (!keys.dec) {
            if (PDIsJoystickDPadEnabled()) {
                if (gJoy_array[3] > 10) {
                    keys.dec = 1;
                }
            } else {
                if (HasCurrentJoystick()) {
                    joystick.dec = gJoy_array[3];
                }
            }
        }

        if (gCredit_multiplier) {
            int temp;

            temp = keys.acc;
            keys.acc = keys.dec;
            keys.dec = temp;

            temp = keys.left;
            keys.left = keys.right;
            keys.right = temp;

            temp = joystick.left;
            joystick.left = joystick.right;
            joystick.right = temp;

            temp = joystick.acc;
            joystick.acc = joystick.dec;
            joystick.dec = temp;
        }
        if (KeyIsDownNoMouldiness(56) && c->gear >= 0) {
            keys.change_down = 1;
            c->just_changed_gear = 1;
            if (keys.acc || joystick.acc > 32000) {
                c->traction_control = 0;
            } else if (c->gear > 1 && !c->keys.change_down) {
                c->gear -= 1;
            }
            if (gCountdown != 0 && !c->keys.change_down) {
                JumpTheStart();
            }
        }
        if (gCar_flying) {
            if (KeyIsDownNoMouldiness(14)) {
                keys.up = 1;
            }
            if (KeyIsDownNoMouldiness(12)) {
                keys.down = 1;
            }
        }
        if (KeyIsDownNoMouldiness(59)) {
            if (!gEntering_message) {
                keys.horn = 1;
            }
        }
    }
    c->keys = keys;
    c->joystick = joystick;
}

// FUNCTION: CARMA2_HW 0x0045a670
void C2_HOOK_FASTCALL FUN_0045a670(void) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x0045acf0
void C2_HOOK_FASTCALL FUN_0045acf0(void) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00502e70
void C2_HOOK_FASTCALL BuyPSPowerup(int pIndex) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004415c0
void C2_HOOK_FASTCALL FinishRace(int i) {
    gRace_over_reason = eRace_over_0;
    gAbandon_game = 2;
}

// FUNCTION: CARMA2_HW 0x00442e80
void C2_HOOK_FASTCALL GetPowerup(int pNum) {

    GotPowerup(&gProgram_state.current_car, pNum);
}

void C2_HOOK_FASTCALL SteelBollock(void) {

    SphericizeCar(gCar_to_view);
    NewTextHeadupSlot(4, 0, 2000, -4, "STEEL GONAD O' DEATH");
}

void (C2_HOOK_FASTCALL * RichardsSphere_original)(int pNum);
void C2_HOOK_FASTCALL RichardsSphere(int pNum) {

    TotallyRepairACar(gCar_to_view);
    SteelBollock();
}

// FUNCTION: CARMA2_HW 0x00444820
void C2_HOOK_FASTCALL CycleCarSimplificationLevel(void) {
    const char *format;
    char *buffer;
    gCar_simplification_level = (gCar_simplification_level + 1) % 5;
    format = GetMiscString(eMiscString_car_simplification_level_D);
    buffer = BrMemAllocate(strlen(format) + 20, kMem_simp_level);
    sprintf(buffer, format, gCar_simplification_level);
    NewTextHeadupSlot(4, 0, 2000, -4, buffer);
    BrMemFree(buffer);
}

// FUNCTION: CARMA2_HW 0x00446ee0
void C2_HOOK_FASTCALL ToggleSky(void) {
    int new_sky_on;

    new_sky_on = !gSky_on;
    if (gSky_on != new_sky_on) {
        br_pixelmap* sky_texture = gSky_texture_0079ec1c;
        gSky_texture_0079ec1c = gProgram_state.current_depth_effect.sky_texture;
        gProgram_state.current_depth_effect.sky_texture = sky_texture;
        gProgram_state.default_depth_effect.sky_texture = sky_texture;
        if (gHorizon_material != NULL && sky_texture != NULL) {
            gHorizon_material->colour_map = sky_texture;
            BrMaterialUpdate(gHorizon_material, BR_MATU_ALL);
            MungeSkyVs(gForward_sky_model, gHorizon_material);
        }
    }
    if (gProgram_state.current_depth_effect.sky_texture != NULL) {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_sky_texture_on));
    } else if (gSky_texture_0079ec1c != NULL) {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_sky_texture_off));
    } else {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_no_sky_texture_for_this_race));
    }
    gSky_on = new_sky_on;
}

// FUNCTION: CARMA2_HW 0x00444720
void C2_HOOK_FASTCALL CycleYonFactor(void) {
    float factor;

    factor = 0.5f * GetYonFactor();
    if (factor < 0.1f) {
        factor = 1.0f;
    }
    SetYonFactor(factor);
    if (factor > 0.75f) {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_track_appears_very_quickly));
    } else if (factor > 0.375f) {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_track_appears_quite_quickly));
    } else if (factor > 0.187f) {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_track_appears_quite_late));
    } else {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_track_appears_very_late));
    }
}

// FUNCTION: CARMA2_HW 0x004448a0
void C2_HOOK_FASTCALL ToggleAccessoryRendering(void) {

    if (gNet_mode == eNet_mode_none) {
        int new_enabled = !GetAccessoryRendering();
        SetAccessoryRendering(new_enabled);
        if (new_enabled) {
            NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_accessories_on));
        } else {
            NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_accessories_off));
        }
    } else {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_net_games_always_accessorized));
    }
}

// FUNCTION: CARMA2_HW 0x00446ad0
void C2_HOOK_FASTCALL DecreaseYon(void) {
    char text[256];
    br_camera* camera;
    int i;

    gCamera_yon -= 5.f;
    if (gCamera_yon < 5.0f) {
        gCamera_yon = 5.0f;
    }
    for (i = 0; i < REC2_ASIZE(gCamera_list); i++) {
        camera = gCamera_list[i]->type_data;
        camera->yon_z = gYon_multiplier * gCamera_yon;
    }
    sprintf(text, GetMiscString(eMiscString_yon_decreased_to_D), (int)camera->yon_z);
    NewTextHeadupSlot(4, 0, 2000, -4, text);
}

// FUNCTION: CARMA2_HW 0x00446a40
void C2_HOOK_FASTCALL IncreaseYon(void) {
    char text[256];
    br_camera* camera;
    int i;

    gCamera_yon += 5.f;
    for (i = 0; i < REC2_ASIZE(gCamera_list); i++) {
        camera = gCamera_list[i]->type_data;
        camera->yon_z = gYon_multiplier * gCamera_yon;
    }
    sprintf(text, GetMiscString(eMiscString_yon_increased_to_D), (int)camera->yon_z);
    NewTextHeadupSlot(4, 0, 2000, -4, text);
}

// FUNCTION: CARMA2_HW 0x004447c0
void C2_HOOK_FASTCALL CycleSoundDetailLevel(void) {
    int new_level;

    new_level = (gSound_detail_level + 1) % 3;
    ReallySetSoundDetailLevel(new_level);
    SetSoundDetailLevel(new_level);
    if (gSound_detail_level == 0) {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_fewest_sounds));
    } else if (gSound_detail_level == 1) {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_partial_sound));
    } else if (gSound_detail_level == 2) {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_all_sounds));
    }
}

// FUNCTION: CARMA2_HW 0x004599f0
void C2_HOOK_FASTCALL ShowCurrentJoystickName(void) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00511240
void C2_HOOK_FASTCALL ToggleJoystickHeadup(void) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00511280
void C2_HOOK_FASTCALL ToggleJoystickYAxis(void) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00511330
void C2_HOOK_FASTCALL ToggleJoystickDPad(void) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00511270
void C2_HOOK_FASTCALL CycleJoystick(void) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x005112d0
void C2_HOOK_FASTCALL ToggleJoystickEnable(void) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00511390
void C2_HOOK_FASTCALL IncreaseJoystickFFB(void) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00447330
void C2_HOOK_FASTCALL FUN_00447330(void) {

}

// FUNCTION: CARMA2_HW 0x00447340
void C2_HOOK_FASTCALL FUN_00447340(void) {

}

// FUNCTION: CARMA2_HW 0x00444420
void C2_HOOK_FASTCALL CycleInvulnerability(void) {
    char message[128];

    gInvulnerability = (gInvulnerability + 1) % 8;
    gCar_to_view->invulnerable_no_damage = gInvulnerability & 1;
    gCar_to_view->invulnerable_no_crushage = gInvulnerability & 2;
    gCar_to_view->invulnerable_no_wastage = gInvulnerability & 4;
    message[0] = '\0';
    if (gInvulnerability == 0) {
        strcpy(message, "FULLY VULNERABLE");
    }
    if (gCar_to_view->invulnerable_no_damage) {
        strcat(message, " NO DAMAGE");
    }
    if (gCar_to_view->invulnerable_no_crushage != 0) {
        strcat(message, " NO CRUSHAGE");
    }
    if (gCar_to_view->invulnerable_no_wastage != 0) {
        strcat(message, " NO WASTAGE");
    }
    NewTextHeadupSlot(4, 0, 1000, -4, message);
}

// FUNCTION: CARMA2_HW 0x004b5ab0
void C2_HOOK_FASTCALL ResetMan(void) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004d6240
void C2_HOOK_FASTCALL NextPedCam(void) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004d6290
void C2_HOOK_FASTCALL PrevPedCam(void) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004d62e0
void C2_HOOK_FASTCALL FindNearestPed(void) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x00444590
void C2_HOOK_FASTCALL ToggleTimerFreeze(void) {

    gFreeze_timer = !gFreeze_timer;
    if (gFreeze_timer) {
        NewTextHeadupSlot(4, 0, 1000, -4, GetMiscString(eMiscString_timer_frozen));
    } else {
        NewTextHeadupSlot(4, 0, 1000, -4, "Timer thawed out");
    }
}

// FUNCTION: CARMA2_HW 0x004445e0
void C2_HOOK_FASTCALL EarnDosh(void) {

    EarnCredits(5000);
}

// FUNCTION: CARMA2_HW 0x004445f0
void C2_HOOK_FASTCALL LoseDosh(void) {

    EarnCredits(-5000);
}

// FUNCTION: CARMA2_HW 0x0040e590
void C2_HOOK_FASTCALL ViewOpponent(void) {

    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004db8e0
void C2_HOOK_FASTCALL MungeRepulseRays(tU32 pTime) {
    int i;

    ARStartPipingSession(ePipe_chunk_repulse_ray);
    for (i = 0; i < REC2_ASIZE(gRepulse_links); i++) {
        tRepulse_link* link = &gRepulse_links[i];
        tU32 delta_time;
        br_model *model;
        br_vector3 delta;
        br_vector3 dir;
        br_vector3 delta_orth;
        br_vector3 delta_unit;
        br_vector3 y_unit;
        br_vector3 orth_dirs[5];
        br_vector3 orth_pieces1[5];
        br_vector3 orth_pieces2[5];
        br_vector3 p;
        int blend;
        int j;
        float percentage;

        if (link->time == 0) {
            continue;
        }
        delta_time = pTime - link->time;
        if (delta_time >= 700) {
            link->time = 0;
            if (link->actor->parent == gNon_track_actor) {
                BrActorRemove(link->actor);
            }
            continue;
        }
        BrVector3Sub(&delta, link->pos_victim, link->pos_origin);
        model = link->actor->model;
        if (delta.v[0] == 0.f && delta.v[2] == 0.f) {
            continue;
        }
        BrVector3Scale(&delta_unit, &delta, 1.f / (float)(model->nvertices / 2 - 1));
        BrVector3Normalise(&dir, &delta);
        BrVector3Set(&y_unit, 0.f, 1.f, 0.f);
        BrVector3Cross(&delta_orth, &delta, &y_unit);
        BrVector3Copy(&p, link->pos_origin);
        BrVector3Normalise(&orth_dirs[0], &delta_orth);
        for (j = 1; j < 5; j++) {
            br_matrix34 mat;

            BrMatrix34Rotate(&mat, (i * (br_angle)0x10000) / 5, &dir);
            BrMatrix34ApplyV(&orth_dirs[i], &orth_dirs[0], &mat);
        }
        for (j = 0; j < 5; j++) {
            BrVector3Scale(&orth_pieces1[i], &orth_dirs[i], link->field_0x18);
            BrVector3Scale(&orth_pieces2[i], &orth_dirs[i], link->field_0x1c);
        }
        for (j = 0; j < model->nvertices / 2; j++) {
            BrVector3Accumulate(&p, &delta_unit);
            BrVector3Add(&model->vertices[j].p, &p, &orth_pieces1[j % 5]);
            BrVector3Add(&model->vertices[j + model->nvertices / 2].p, &p, &orth_pieces2[j % 5]);
        }
        percentage = (float)delta_time / 700.f;
        if (percentage > .9f) {
            blend = 25;
        } else if (percentage > .8f) {
            blend = 50;
        } else if (percentage > .7f) {
            blend = 75;
        } else {
            blend = 100;
        }
        if (blend != link->field_0x10) {
            BlendifyMaterial(link->material, blend);
            BrMaterialUpdate(link->material, BR_MATU_ALL);
            link->field_0x10 = blend;
        }
        BrModelUpdate(model, BR_MODU_ALL);
        if (link->actor->parent != gNon_track_actor) {
            BrActorAdd(gNon_track_actor, link->actor);
        }
        AddRepulseRayToPipingSession(i, link->pos_origin, link->pos_victim, link->time);
    }
    AREndPipingSession();
}

void C2_HOOK_FASTCALL GotPowerupN(int pN) {
    int modifiers;

    modifiers = 0;
    if (PDKeyDown(0)) {
        modifiers += 10;
    }
    if (PDKeyDown(1)) {
        modifiers += 20;
    }
    if (PDKeyDown(2)) {
        modifiers += 40;
    }
    if (PDKeyDown(4)) {
        modifiers += 80;
    }
    GotPowerup(&gProgram_state.current_car, modifiers + pN);
}

// FUNCTION: CARMA2_HW 0x004dbdb0
void C2_HOOK_FASTCALL GotPowerup0(void) {

    GotPowerupN(0);
}

// FUNCTION: CARMA2_HW 0x004dbe10
void C2_HOOK_FASTCALL GotPowerup1(void) {

    GotPowerupN(1);
}

// FUNCTION: CARMA2_HW 0x004dbe70
void C2_HOOK_FASTCALL GotPowerup2(void) {

    GotPowerupN(2);
}

// FUNCTION: CARMA2_HW 0x004dbed0
void C2_HOOK_FASTCALL GotPowerup3(void) {

    GotPowerupN(3);
}

// FUNCTION: CARMA2_HW 0x004dbf30
void C2_HOOK_FASTCALL GotPowerup4(void) {

    GotPowerupN(4);
}

// FUNCTION: CARMA2_HW 0x004dbf90
void C2_HOOK_FASTCALL GotPowerup5(void) {

    GotPowerupN(5);
}

// FUNCTION: CARMA2_HW 0x004dbff0
void C2_HOOK_FASTCALL GotPowerup6(void) {

    GotPowerupN(6);
}

// FUNCTION: CARMA2_HW 0x004dc050
void C2_HOOK_FASTCALL GotPowerup7(void) {

    GotPowerupN(7);
}

// FUNCTION: CARMA2_HW 0x004dc0b0
void C2_HOOK_FASTCALL GotPowerup8(void) {

    GotPowerupN(8);
}

// FUNCTION: CARMA2_HW 0x004dc110
void C2_HOOK_FASTCALL GotPowerup9(void) {

    GotPowerupN(9);
}

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
    for (i = 0; i < REC2_ASIZE(gAbuse_text); i++) {
        if (PFfgets(s, REC2_ASIZE(s) - 1, f) == NULL) {
            break;
        }
        len = strlen(s);
        if (len > 63) {
            s[63] = '\0';
        }
        len = strlen(s);
        while (len != 0 && s[len - 1] < ' ') {
            s[len - 1] = '\0';
            len--;
        }
        gAbuse_text[i] = BrMemAllocate(strlen(s) + 1, kMem_abuse_text);
        strcpy(gAbuse_text[i], s);
    }
    PFfclose(f);
}

// FUNCTION: CARMA2_HW 0x00441e10
void C2_HOOK_FASTCALL LookLeft(void) {

    PratcamEvent(27);
    gProgram_state.old_view = gProgram_state.which_view;
    if (gProgram_state.which_view == eView_left) {
        LookForward();
    } else if (gProgram_state.which_view == eView_right) {
        LookForward();
        gProgram_state.pending_view = eView_left;
    } else {
        ClearWobbles();
        gProgram_state.new_view = eView_left;
        gProgram_state.view_change_start = PDGetTotalTime();
        gProgram_state.pending_view = eView_undefined;
    }
}

// FUNCTION: CARMA2_HW 0x00441ec0
void C2_HOOK_FASTCALL LookForward(void) {

    if (gProgram_state.which_view == eView_right) {
        PratcamEvent(27);
    } else if (gProgram_state.which_view == eView_left) {
        PratcamEvent(28);
    }
    if (gProgram_state.which_view != eView_forward) {
        gProgram_state.old_view = gProgram_state.which_view;
        ClearWobbles();
        gProgram_state.new_view = eView_forward;
        gProgram_state.view_change_start = PDGetTotalTime();
        gProgram_state.pending_view = eView_undefined;
    }
}

// FUNCTION: CARMA2_HW 0x00441f20
void C2_HOOK_FASTCALL LookRight(void) {

    PratcamEvent(28);
    gProgram_state.old_view = gProgram_state.which_view;
    if (gProgram_state.which_view == eView_right) {
        LookForward();
    } else if (gProgram_state.which_view == eView_left) {
        LookForward();
        gProgram_state.pending_view = eView_right;
    } else {
        ClearWobbles();
        gProgram_state.new_view = eView_right;
        gProgram_state.view_change_start = PDGetTotalTime();
        gProgram_state.pending_view = eView_undefined;
    }
}

// FUNCTION: CARMA2_HW 0x00444c00
void C2_HOOK_FASTCALL DisplayUserMessage(void) {
    char* the_message;
    int len;
    tDR_font* font;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tGraf_data, net_message_enter_y, 0x4b4);

    font = &gFonts[6];
    the_message = &gString[20];
    if (!gEntering_message || gNet_mode == eNet_mode_none) {
        return;
    }

    len = strlen(the_message);
    if (len < 63 && (PDGetTotalTime() & 0x200)) {
        the_message[len] = '_';
        the_message[len + 1] = '\0';
    }
    DimRectangle(gBack_screen,
        15 * gBack_screen->width / 100,
        gCurrent_graf_data->net_message_enter_y - font->height,
        85 * gBack_screen->width / 100,
        gCurrent_graf_data->net_message_enter_y + 6 * font->height,
        1);

    TransDRPixelmapText(gBack_screen, 20 * gBack_screen->width / 100, gCurrent_graf_data->net_message_enter_y, font, GetMiscString(eMiscString_enter_message), 100);
    OoerrIveGotTextInMeBoxMissus(
        6,
        the_message,
        gBack_screen,
        20 * gBack_screen->width / 100,
        gCurrent_graf_data->net_message_enter_y + 2 * font->height,
        80 * gBack_screen->width / 100,
        gCurrent_graf_data->net_message_enter_y + 6 * font->height,
        0);
    the_message[len] = '\0';
}
