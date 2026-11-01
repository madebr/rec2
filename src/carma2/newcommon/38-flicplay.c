/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "38-flicplay.h"

#include "08-loading1.h"
#include "19-font.h"
#include "41-utility.h"
#include "52-errors.h"
#include "70-packfile.h"
#include "globvars.h"
#include "platform.h"
#include "rec2_macros.h"

#include "c2_stdio.h"
#include "c2_string.h"

// GLOBAL: CARMA2_HW 0x00599920
// GLOBAL: CARMA2_SW 0x00594868
int gFlic_bunch0[29] = {
    10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 35, 26,
    27, 36, 28, 29, 130, 131, 132, 42, 43, 135, 45
};

// GLOBAL: CARMA2_HW 0x00599998
// GLOBAL: CARMA2_SW 0x005948e0

int gFlic_bunch1[31] = {
    140, 141, 42, 43, 144, 145, 146, 147, 45, 150, 151, 42, 43, 154, 155,
    156, 160, 161, 42, 43, 154, 170, 171, 176, 177, 172, 180, 181, 42, 43, 154
};

// GLOBAL: CARMA2_HW 0x00599a18
// GLOBAL: CARMA2_SW 0x00594960
int gFlic_bunch2[8] = {
    70, 71, 72, 73, 74, 56, 57, 59
};

// GLOBAL: CARMA2_HW 0x00599a38
// GLOBAL: CARMA2_SW 0x00594980
int gFlic_bunch3[13] = {
    40, 41, 42, 43, 44, 45, 50, 51, 73, 74, 56, 57, 59
};

// GLOBAL: CARMA2_HW 0x00599a70
// GLOBAL: CARMA2_SW 0x005949b8
int gFlic_bunch4[22] = {
    80, 81, 82, 83, 84, 85, 42, 43, 88, 45, 110, 111, 42, 43, 45, 115,
    116, 117, 118, 119, 120, 121
};

// GLOBAL: CARMA2_HW 0x00599ac8
// GLOBAL: CARMA2_SW 0x00594a10
int gFlic_bunch5[5] = {
    100, 101, 42, 43, 45
};

// GLOBAL: CARMA2_HW 0x00599ae0
// GLOBAL: CARMA2_SW 0x00594a28
int gFlic_bunch6[51] = {
    190, 191, 192, 42, 43, 195, 200, 201, 210, 212, 213, 220, 221, 222,
    220, 221, 225, 230, 231, 42, 43, 154, 45, 220, 221, 222, 220, 221,
    225, 250, 251, 42, 43, 254, 255, 256, 154, 42, 43, 260, 220, 221,
    222, 220, 221, 225, 280, 281, 42, 43, 284
};

// GLOBAL: CARMA2_HW 0x00599bb0
// GLOBAL: CARMA2_SW 0x00594af8
int gFlic_bunch7[7] = {
    130, 131, 132, 42, 43, 135, 45
};

// GLOBAL: CARMA2_HW 0x00599bd0
// GLOBAL: CARMA2_SW 0x00594b18
int gFlic_bunch8[16] = {
    290, 291, 292, 293, 294, 295, 296, 297, 42, 43, 154, 301, 42, 43, 304, 305
};

// GLOBAL: CARMA2_HW 0x00599c10
// GLOBAL: CARMA2_SW 0x00594b58
tFlic_bunch gFlic_bunch[9] = {
    { REC2_ASIZE(gFlic_bunch0), gFlic_bunch0 },
    { REC2_ASIZE(gFlic_bunch1), gFlic_bunch1 },
    { REC2_ASIZE(gFlic_bunch2), gFlic_bunch2 },
    { REC2_ASIZE(gFlic_bunch3), gFlic_bunch3 },
    { REC2_ASIZE(gFlic_bunch4), gFlic_bunch4 },
    { REC2_ASIZE(gFlic_bunch5), gFlic_bunch5 },
    { REC2_ASIZE(gFlic_bunch6), gFlic_bunch6 },
    { REC2_ASIZE(gFlic_bunch7), gFlic_bunch7 },
    { REC2_ASIZE(gFlic_bunch8), gFlic_bunch8 },
};

// GLOBAL: CARMA2_HW 0x0068b8b4
// GLOBAL: CARMA2_SW 0x00685e14
float gFlic_sound_delay_pre_smack;

// GLOBAL: CARMA2_HW 0x0068b8b8
// GLOBAL: CARMA2_SW 0x00685e18
float gFlic_sound_delay_post_smack;

// GLOBAL: CARMA2_HW 0x0068b8bc
// GLOBAL: CARMA2_SW 0x00685e1c
float gFlic_sound_delay_not_in_demo;

// GLOBAL: CARMA2_HW 0x0068b8c0
// GLOBAL: CARMA2_SW 0x00685e20
float gFlic_sound_delay_post_demo;

// GLOBAL: CARMA2_HW 0x006861dc
// GLOBAL: CARMA2_SW 0x0068091c
int gTranslation_count;

// GLOBAL: CARMA2_HW 0x006861e8
// GLOBAL: CARMA2_SW 0x00680928
tFlic_descriptor* gFirst_flic;

// GLOBAL: CARMA2_HW 0x006861d8
// GLOBAL: CARMA2_SW 0x00680918
void* gPalette_pixels;

// GLOBAL: CARMA2_HW 0x00686208
// GLOBAL: CARMA2_SW 0x00680948
br_pixelmap* gPalette;

// GLOBAL: CARMA2_HW 0x00686210
// GLOBAL: CARMA2_SW 0x00680950
tTranslation_record* gTranslations;

// GLOBAL: CARMA2_HW 0x00686218
// GLOBAL: CARMA2_SW 0x00680958
tFlic_descriptor gPanel_flic[2];

// GLOBAL: CARMA2_HW 0x00686320
// GLOBAL: CARMA2_SW 0x00680a60
br_pixelmap* gPanel_buffer[2];

// GLOBAL: CARMA2_HW 0x006861e0
// GLOBAL: CARMA2_SW 0x00680920
tDR_font* gTrans_fonts[2];

// GLOBAL: CARMA2_HW 0x00686308
// GLOBAL: CARMA2_SW 0x00680a48
int gPalette_fuck_prevention;

// GLOBAL: CARMA2_HW 0x0068630c
// GLOBAL: CARMA2_SW 0x00680a4c
int gDark_mode;

// GLOBAL: CARMA2_HW 0x00686310
// GLOBAL: CARMA2_SW 0x00680a50
int gTransparency_on;

// GLOBAL: CARMA2_HW 0x00686314
// GLOBAL: CARMA2_SW 0x00680a54
int gPanel_flic_disable;

// GLOBAL: CARMA2_HW 0x00686318
// GLOBAL: CARMA2_SW 0x00680a58
int gPlay_from_disk;

// GLOBAL: CARMA2_HW 0x005964c8
// GLOBAL: CARMA2_SW 0x00591410
int gPending_flic = -1;

// GLOBAL: CARMA2_HW 0x0068620c
// GLOBAL: CARMA2_SW 0x0068094c
int gPending_pending_flic;

// GLOBAL: CARMA2_HW 0x005964d0
// GLOBAL: CARMA2_SW 0x00591418
tFlic_spec gMain_flic_list[372] = {
    { "MAINSTIL.FLI", 1, 0, 0, 0, 0, 25, },
    { "DEMSTRT2.FLI", 1, 0, 0, 0, 0, 0, },
    { "DEMSTRT1.FLI", 1, 0, 0, 0, 0, 0, },
    { "NEWFLIC.FLI", 1, 0, 0, 0, 0, 0, },
    { "GOTOFLIC.FLI", 1, 0, 0, 0, 0, 0, },
    { "ENDFLIC.FLI", 1, 0, 0, 0, 0, 0, },
    { "OVERFLIC.FLI", 1, 0, 0, 0, 0, 0, },
    { "DEMO8.FLI", 1, 0, 0, 0, 0, 0, },
    { "COMPLETE.FLI", 1, 0, 0, 0, 0, 0, },
    { "DEMOEND.FLI", 1, 0, 0, 0, 0, 0, },
    { "MAINSTIL.FLI", 0, 0, 0, 0, 0, 0, },
    { "MAINCOME.FLI", 0, 0, 0, 0, 0, 0, },
    { "MAINAWAY.FLI", 0, 1, 0, 0, 0, 0, },
    { "MAINCNFL.FLI", 0, 0, 0, 0, 0, 0, },
    { "MAINCNGL.FLI", 0, 0, 0, 0, 0, 0, },
    { "MAINCNIN.FLI", 0, 0, 0, 0, 0, 0, },
    { "MAINABFL.FLI", 0, 0, 0, 0, 0, 0, },
    { "MAINABGL.FLI", 0, 0, 0, 0, 0, 0, },
    { "MAINQTFL.FLI", 0, 0, 0, 0, 0, 0, },
    { "MAINQTGL.FLI", 0, 0, 0, 0, 0, 0, },
    { "MAINSVFL.FLI", 0, 0, 0, 0, 0, 0, },
    { "MAINSVGL.FLI", 0, 0, 0, 0, 0, 0, },
    { "MAINLDFL.FLI", 0, 0, 0, 0, 0, 0, },
    { "MAINLDGL.FLI", 0, 0, 0, 0, 0, 0, },
    { "MAINRCFL.FLI", 0, 0, 0, 0, 0, 0, },
    { "MAINRCGL.FLI", 0, 0, 0, 0, 0, 0, },
    { "MAINARFL.FLI", 0, 0, 0, 0, 0, 0, },
    { "MAINARGL.FLI", 0, 0, 0, 0, 0, 0, },
    { "MAINOPFL.FLI", 0, 0, 0, 0, 0, 0, },
    { "MAINOPGL.FLI", 0, 0, 0, 0, 0, 0, },
    { "MAI2STIL.FLI", 0, 0, 0, 0, 0, 0, },
    { "MAI2COME.FLI", 0, 0, 0, 0, 0, 0, },
    { "MAI2AWAY.FLI", 0, 1, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "MAINRCGY.FLI", 0, 0, 0, 0, 0, 0, },
    { "MAINARGY.FLI", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "SVVYSTIL.FLI", 0, 0, 0, 0, 0, 0, },
    { "SVVYAWAY.FLI", 0, 1, 0, 0, 0, 0, },
    { "BGBUTTFL.FLI", 0, 0, 0, 0, 0, 0, },
    { "BGBUTTGL.FLI", 0, 0, 0, 0, 0, 0, },
    { "SVVYOKIN.FLI", 0, 0, 0, 0, 0, 0, },
    { "CANBUTIN.FLI", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "SAVECOME.FLI", 0, 0, 0, 0, 0, 0, },
    { "SAVEAWAY.FLI", 0, 1, 0, 0, 0, 0, },
    { "SMLBUTFL.FLI", 0, 0, 0, 0, 0, 0, },
    { "SMLBUTGL.FLI", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "SMLBUTFL.FLI", 0, 0, 0, 0, 0, 0, },
    { "SMLBUTGL.FLI", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "SAVECAIN.FLI", 0, 0, 0, 0, 0, 0, },
    { "NRACCOME.FLI", 0, 0, 0, 0, 0, 0, },
    { "NRACAWAY.FLI", 0, 0, 0, 0, 0, 0, },
    { "NCARCOME.FLI", 0, 0, 0, 0, 0, 0, },
    { "NSUMSTIL.FLI", 0, 0, 0, 0, 0, 0, },
    { "NSUMAWAY.FLI", 0, 0, 0, 0, 0, 0, },
    { "NOPTCOME.FLI", 0, 0, 0, 0, 0, 0, },
    { "NOPTAWAY.FLI", 0, 0, 0, 0, 0, 0, },
    { "NOPTDEIN.FLI", 0, 0, 0, 0, 0, 0, },
    { "NOPT00GL.FLI", 0, 0, 0, 0, 0, 0, },
    { "NOPT00FL.FLI", 0, 0, 0, 0, 0, 0, },
    { "LOADSTIL.FLI", 0, 0, 0, 0, 0, 0, },
    { "LOADCOME.FLI", 0, 0, 0, 0, 0, 0, },
    { "LOADAWAY.FLI", 0, 1, 0, 0, 0, 0, },
    { "LOADHIFL.FLI", 0, 0, 0, 0, 0, 0, },
    { "LOADHIGL.FLI", 0, 0, 0, 0, 0, 0, },
    { "NOPT01GL.FLI", 0, 0, 0, 0, 0, 0, },
    { "NOPT01FL.FLI", 0, 0, 0, 0, 0, 0, },
    { "NOPT02GL.FLI", 0, 0, 0, 0, 0, 0, },
    { "NOPT02FL.FLI", 0, 0, 0, 0, 0, 0, },
    { "NOPT03GL.FLI", 0, 0, 0, 0, 0, 0, },
    { "NEWGCOME.FLI", 0, 0, 0, 0, 0, 0, },
    { "NEWGAWAY.FLI", 0, 1, 0, 0, 0, 0, },
    { "NEWGHLFL.FLI", 0, 0, 0, 0, 0, 0, },
    { "NEWGHLGL.FLI", 0, 0, 0, 0, 0, 0, },
    { "NEWGDNAV.FLI", 0, 0, 0, 0, 0, 0, },
    { "NEWGDNGY.FLI", 0, 0, 0, 0, 0, 0, },
    { "NEWGDNFL.FLI", 0, 0, 0, 0, 0, 0, },
    { "NEWGDNGL.FLI", 0, 0, 0, 0, 0, 0, },
    { "NEWGDNIN.FLI", 0, 0, 0, 0, 0, 0, },
    { "NEWGCAIN.FLI", 0, 0, 0, 0, 0, 0, },
    { "NEWNSTIN.FLI", 0, 0, 0, 0, 0, 0, },
    { "NEWNSTFL.FLI", 0, 0, 0, 0, 0, 0, },
    { "NEWNSTGL.FLI", 0, 0, 0, 0, 0, 0, },
    { "CHCKBXFL.FLI", 0, 0, 0, 0, 0, 0, },
    { "CHCKBXGL.FLI", 0, 0, 0, 0, 0, 0, },
    { "CHCKBXON.FLI", 0, 0, 0, 0, 0, 0, },
    { "CHCKBXOF.FLI", 0, 0, 0, 0, 0, 0, },
    { "RADBUTFL.FLI", 0, 0, 0, 0, 0, 0, },
    { "RADBUTGL.FLI", 0, 0, 0, 0, 0, 0, },
    { "RADBUTON.FLI", 0, 0, 0, 0, 0, 0, },
    { "NEWNCOME.FLI", 0, 0, 0, 0, 0, 0, },
    { "NEWNAWAY.FLI", 0, 1, 0, 0, 0, 0, },
    { "NETTCOME.FLI", 0, 0, 0, 0, 0, 0, },
    { "NETTAWAY.FLI", 0, 0, 0, 0, 0, 0, },
    { "NETOCOME.FLI", 0, 0, 0, 0, 0, 0, },
    { "NETOAWAY.FLI", 0, 0, 0, 0, 0, 0, },
    { "NETNCOME.FLI", 0, 0, 0, 0, 0, 0, },
    { "NETNAWAY.FLI", 0, 0, 0, 0, 0, 0, },
    { "RADBUTOF.FLI", 0, 0, 0, 0, 0, 0, },
    { "NOPT03FL.FLI", 0, 0, 0, 0, 0, 0, },
    { "SKILCOME.FLI", 0, 0, 0, 0, 0, 0, },
    { "SKILAWAY.FLI", 0, 1, 0, 0, 0, 0, },
    { "NOPT04GL.FLI", 0, 0, 0, 0, 0, 0, },
    { "NOPT04FL.FLI", 0, 0, 0, 0, 0, 0, },
    { "NOPT05GL.FLI", 0, 0, 0, 0, 0, 0, },
    { "SKILL1FL.FLI", 0, 0, 0, 0, 0, 0, },
    { "SKILL1GL.FLI", 0, 0, 0, 0, 0, 0, },
    { "SKILLIN.FLI", 0, 0, 0, 0, 0, 0, },
    { "SKILL2FL.FLI", 0, 0, 0, 0, 0, 0, },
    { "SKILL2GL.FLI", 0, 0, 0, 0, 0, 0, },
    { "SKILL3FL.FLI", 0, 0, 0, 0, 0, 0, },
    { "SKILL3GL.FLI", 0, 0, 0, 0, 0, 0, },
    { "NCHOCOME.FLI", 0, 0, 0, 0, 0, 0, },
    { "NCHOAWAY.FLI", 0, 0, 0, 0, 0, 0, },
    { "NCHOOPIN.FLI", 0, 0, 0, 0, 0, 0, },
    { "ERRRSTIL.FLI", 0, 0, 0, 0, 0, 0, },
    { "NOPT05FL.FLI", 0, 0, 0, 0, 0, 0, },
    { "NOPT06GL.FLI", 0, 0, 0, 0, 0, 0, },
    { "NOPT06FL.FLI", 0, 0, 0, 0, 0, 0, },
    { "NOPT07GL.FLI", 0, 0, 0, 0, 0, 0, },
    { "QUT1STIL.FLI", 0, 0, 0, 0, 0, 0, },
    { "QUT2STIL.FLI", 0, 0, 0, 0, 0, 0, },
    { "QUT3STIL.FLI", 0, 0, 0, 0, 0, 0, },
    { "NOPT07FL.FLI", 0, 0, 0, 0, 0, 0, },
    { "NOPT08GL.FLI", 0, 0, 0, 0, 0, 0, },
    { "QUITOKIN.FLI", 0, 0, 0, 0, 0, 0, },
    { "NOPT08FL.FLI", 0, 0, 0, 0, 0, 0, },
    { "NOPT09GL.FLI", 0, 0, 0, 0, 0, 0, },
    { "NOPT09FL.FLI", 0, 0, 0, 0, 0, 0, },
    { "NOPT10GL.FLI", 0, 0, 0, 0, 0, 0, },
    { "OPTNCOME.FLI", 0, 0, 0, 0, 0, 0, },
    { "OPTNAWAY.FLI", 0, 1, 0, 0, 0, 0, },
    { "NOPT10FL.FLI", 0, 0, 0, 0, 0, 0, },
    { "NOPT11GL.FLI", 0, 0, 0, 0, 0, 0, },
    { "OPTNSNIN.FLI", 0, 0, 0, 0, 0, 0, },
    { "OPTNCNIN.FLI", 0, 0, 0, 0, 0, 0, },
    { "OPTNGRIN.FLI", 0, 0, 0, 0, 0, 0, },
    { "OPTNMSIN.FLI", 0, 0, 0, 0, 0, 0, },
    { "NOPT11FL.FLI", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "SNDOCOME.FLI", 0, 0, 0, 0, 0, 0, },
    { "SNDOAWAY.FLI", 0, 1, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "DNEBUTIN.FLI", 0, 0, 0, 0, 0, 0, },
    { "SNDOOLFL.FLI", 0, 0, 0, 0, 0, 0, },
    { "SNDOOLGL.FLI", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "GRPHCOME.FLI", 0, 0, 0, 0, 0, 0, },
    { "GRPHAWAY.FLI", 0, 1, 0, 0, 0, 0, },
    { "NCHO00GL.FLI", 0, 0, 0, 0, 0, 0, },
    { "NCHO01GL.FLI", 0, 0, 0, 0, 0, 0, },
    { "NCHO02GL.FLI", 0, 0, 0, 0, 0, 0, },
    { "NCHO03GL.FLI", 0, 0, 0, 0, 0, 0, },
    { "NCHO04GL.FLI", 0, 0, 0, 0, 0, 0, },
    { "NCHO05GL.FLI", 0, 0, 0, 0, 0, 0, },
    { "NCHO06GL.FLI", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "CNTLCOME.FLI", 0, 0, 0, 0, 0, 0, },
    { "CNTLAWAY.FLI", 0, 1, 0, 0, 0, 0, },
    { "CNTLDNIN.FLI", 0, 0, 0, 0, 0, 0, },
    { "CNTLCAIN.FLI", 0, 0, 0, 0, 0, 0, },
    { "CNTLDFIN.FLI", 0, 0, 0, 0, 0, 0, },
    { "CNTLMRIN.FLI", 0, 0, 0, 0, 0, 0, },
    { "CNTLDNFL.FLI", 0, 0, 0, 0, 0, 0, },
    { "CNTLDNGL.FLI", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "CNTLSTIL.FLI", 0, 0, 0, 0, 0, 0, },
    { "OTHRCOME.FLI", 0, 0, 0, 0, 0, 0, },
    { "OTHRAWAY.FLI", 0, 1, 0, 0, 0, 0, },
    { "NCHO00FL.FLI", 0, 0, 0, 0, 0, 0, },
    { "NCHO01FL.FLI", 0, 0, 0, 0, 0, 0, },
    { "NCHO02FL.FLI", 0, 0, 0, 0, 0, 0, },
    { "NCHO03FL.FLI", 0, 0, 0, 0, 0, 0, },
    { "NCHO04FL.FLI", 0, 0, 0, 0, 0, 0, },
    { "NCHO05FL.FLI", 0, 0, 0, 0, 0, 0, },
    { "NCHO06FL.FLI", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "STRTSTIL.FLI", 0, 0, 0, 0, 0, 0, },
    { "STRTCOME.FLI", 0, 0, 0, 0, 0, 0, },
    { "STRTAWAY.FLI", 0, 1, 0, 0, 0, 0, },
    { "CNTL00FL.FLI", 0, 0, 0, 0, 0, 0, },
    { "CNTL00GL.FLI", 0, 0, 0, 0, 0, 0, },
    { "STRTCRIN.FLI", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "STRTPSIN.FLI", 0, 0, 0, 0, 0, 0, },
    { "STRTSRIN.FLI", 0, 0, 0, 0, 0, 0, },
    { "STRTCCIN.FLI", 0, 0, 0, 0, 0, 0, },
    { "NTSHSTIL.FLI", 0, 0, 0, 0, 0, 0, },
    { "NTSCSTIL.FLI", 0, 0, 0, 0, 0, 0, },
    { "NTSHSTIN.FLI", 0, 0, 0, 0, 0, 0, },
    { "NTSHENIN.FLI", 0, 0, 0, 0, 0, 0, },
    { "NTSCLVIN.FLI", 0, 0, 0, 0, 0, 0, },
    { "NTSHSTEN.FLI", 0, 0, 0, 0, 0, 0, },
    { "NTSXSTIL.FLI", 0, 0, 0, 0, 0, 0, },
    { "VWSC2IN.FLI", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "VWIN2OPP.FLI", 0, 0, 0, 0, 0, 0, },
    { "VWOPP2SC.FLI", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "2BUTONFL.FLI", 0, 0, 0, 0, 0, 0, },
    { "2BUTONGL.FLI", 0, 0, 0, 0, 0, 0, },
    { "VWOPUPIN.FLI", 0, 0, 0, 0, 0, 0, },
    { "VWOPDWFL.FLI", 0, 0, 0, 0, 0, 0, },
    { "VWOPDWGL.FLI", 0, 0, 0, 0, 0, 0, },
    { "VWOPDWIN.FLI", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "CHRCCOME.FLI", 0, 0, 0, 0, 0, 0, },
    { "CHRCAWAY.FLI", 0, 1, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "CHCRCOME.FLI", 0, 0, 0, 0, 0, 0, },
    { "CHCRAWAY.FLI", 0, 1, 0, 0, 0, 0, },
    { "GRPH00GL.FLI", 0, 0, 0, 0, 0, 0, },
    { "GRPH01GL.FLI", 0, 0, 0, 0, 0, 0, },
    { "GRPH02GL.FLI", 0, 0, 0, 0, 0, 0, },
    { "GRPH03GL.FLI", 0, 0, 0, 0, 0, 0, },
    { "GRPH04GL.FLI", 0, 0, 0, 0, 0, 0, },
    { "GRPH05GL.FLI", 0, 0, 0, 0, 0, 0, },
    { "GRPH06GL.FLI", 0, 0, 0, 0, 0, 0, },
    { "GRPH07GL.FLI", 0, 0, 0, 0, 0, 0, },
    { "GRPH08GL.FLI", 0, 0, 0, 0, 0, 0, },
    { "GRPH09GL.FLI", 0, 0, 0, 0, 0, 0, },
    { "GRPH10GL.FLI", 0, 0, 0, 0, 0, 0, },
    { "GRPH11GL.FLI", 0, 0, 0, 0, 0, 0, },
    { "PARTCOME.FLI", 0, 0, 0, 0, 0, 0, },
    { "PARTAWAY.FLI", 0, 1, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "PARTARGL.FLI", 0, 0, 0, 0, 0, 0, },
    { "PARTARIN.FLI", 0, 0, 0, 0, 0, 0, },
    { "PARTPFIN.FLI", 0, 0, 0, 0, 0, 0, },
    { "PARTOFIN.FLI", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "PARTEXIN.FLI", 0, 0, 0, 0, 0, 0, },
    { "PARTSPIN.FLI", 0, 0, 0, 0, 0, 0, },
    { "PARTARGL.FLI", 0, 0, 0, 0, 0, 0, },
    { "PARTPFGL.FLI", 0, 0, 0, 0, 0, 0, },
    { "PARTOFGL.FLI", 0, 0, 0, 0, 0, 0, },
    { "GRPH00FL.FLI", 0, 0, 0, 0, 0, 0, },
    { "GRPH01FL.FLI", 0, 0, 0, 0, 0, 0, },
    { "GRPH02FL.FLI", 0, 0, 0, 0, 0, 0, },
    { "GRPH03FL.FLI", 0, 0, 0, 0, 0, 0, },
    { "GRPH04FL.FLI", 0, 0, 0, 0, 0, 0, },
    { "GRPH05FL.FLI", 0, 0, 0, 0, 0, 0, },
    { "GRPH06FL.FLI", 0, 0, 0, 0, 0, 0, },
    { "GRPH07FL.FLI", 0, 0, 0, 0, 0, 0, },
    { "GRPH08FL.FLI", 0, 0, 0, 0, 0, 0, },
    { "GRPH09FL.FLI", 0, 0, 0, 0, 0, 0, },
    { "GRPH10FL.FLI", 0, 0, 0, 0, 0, 0, },
    { "GRPH11FL.FLI", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "PSRMCOME.FLI", 0, 0, 0, 0, 0, 0, },
    { "PSRMAWAY.FLI", 0, 1, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "PSRMDIIN.FLI", 0, 0, 0, 0, 0, 0, },
    { "RADBUTFL.FLI", 0, 0, 0, 0, 0, 0, },
    { "RADBUTGL.FLI", 0, 0, 0, 0, 0, 0, },
    { "RADBUTOF.FLI", 0, 0, 0, 0, 0, 0, },
    { "RADBUTON.FLI", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "GRIDSTIL.FLI", 0, 0, 0, 0, 0, 0, },
    { "GRIDAWAY.FLI", 0, 1, 0, 0, 0, 0, },
    { "GRIDLFFL.FLI", 0, 0, 0, 0, 0, 0, },
    { "GRIDLFGL.FLI", 0, 0, 0, 0, 0, 0, },
    { "GRIDLFIN.FLI", 0, 0, 0, 0, 0, 0, },
    { "GRIDRTFL.FLI", 0, 0, 0, 0, 0, 0, },
    { "GRIDRTGL.FLI", 0, 0, 0, 0, 0, 0, },
    { "GRIDRTIN.FLI", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "DARECOME.FLI", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "DAREACIN.FLI", 0, 0, 0, 0, 0, 0, },
    { "DARECHIN.FLI", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "SUM1STIL.FLI", 0, 0, 0, 0, 0, 0, },
    { "SUM1AWAY.FLI", 0, 1, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "SUM2STIL.FLI", 0, 0, 0, 0, 0, 0, },
    { "BGBUT8GL.FLI", 0, 0, 0, 0, 0, 0, },
    { "BGBUT8FL.FLI", 0, 0, 0, 0, 0, 0, },
    { "DNBUT8IN.FLI", 0, 0, 0, 0, 0, 0, },
    { "BKBUT8IN.FLI", 0, 0, 0, 0, 0, 0, },
    { "BKBUTOFF.FLI", 0, 0, 0, 0, 0, 0, },
    { "BKBUTON.FLI", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "", 0, 0, 0, 0, 0, 0, },
    { "MAI2QTFL.FLI", 0, 0, 0, 0, 0, 0, },
    { "MAI2QTGL.FLI", 0, 0, 0, 0, 0, 0, },
    { "MAI2LDFL.FLI", 0, 0, 0, 0, 0, 0, },
    { "MAI2LDGL.FLI", 0, 0, 0, 0, 0, 0, },
    { "MAI2N1FL.FLI", 0, 0, 0, 0, 0, 0, },
    { "MAI2N1GL.FLI", 0, 0, 0, 0, 0, 0, },
    { "MAI2NNFL.FLI", 0, 0, 0, 0, 0, 0, },
    { "MAI2NNGL.FLI", 0, 0, 0, 0, 0, 0, },
    { "MAI2OPFL.FLI", 0, 0, 0, 0, 0, 0, },
    { "MAI2OPGL.FLI", 0, 0, 0, 0, 0, 0, },
};

// FUNCTION: CARMA2_HW 0x00461990
// FUNCTION: CARMA2_SW 0x00461430
int C2_HOOK_FASTCALL TranslationMode(void) {

    return gTranslation_count;
}

// FUNCTION: CARMA2_HW 0x004619a0
void C2_HOOK_FASTCALL DontLetFlicFuckWithPalettes(void) {
    gPalette_fuck_prevention = 1;
}

// FUNCTION: CARMA2_HW 0x004619b0
// FUNCTION: CARMA2_SW 0x00461450
void C2_HOOK_FASTCALL LetFlicFuckWithPalettes(void) {
    gPalette_fuck_prevention = 0;
}

// FUNCTION: CARMA2_HW 0x004619c0
// FUNCTION: CARMA2_SW 0x00461460
void C2_HOOK_STDCALL PlayFlicsInDarkness(void) {
    gDark_mode = 1;
}

// FUNCTION: CARMA2_HW 0x004619e0
// FUNCTION: CARMA2_SW 0x00461480
void C2_HOOK_FASTCALL TurnFlicTransparencyOn(void) {
    gTransparency_on = 1;
}

// FUNCTION: CARMA2_HW 0x004619f0
// FUNCTION: CARMA2_SW 0x00461490
void C2_HOOK_FASTCALL TurnFlicTransparencyOff(void) {
    gTransparency_on = 0;
}

// FUNCTION: CARMA2_HW 0x00461a00
// FUNCTION: CARMA2_SW 0x004614a0
void C2_HOOK_FASTCALL PlayFlicsFromDisk(void) {
    gPlay_from_disk = 1;
}

// FUNCTION: CARMA2_HW 0x00461a10
// FUNCTION: CARMA2_SW 0x004614b0
void C2_HOOK_FASTCALL PlayFlicsFromMemory(void) {
    gPlay_from_disk = 0;
}

// FUNCTION: CARMA2_HW 0x00461a20
// FUNCTION: CARMA2_SW 0x004614c0
void C2_HOOK_FASTCALL TurnOffPanelFlics(void) {
    gPanel_flic_disable = 1;
}

// FUNCTION: CARMA2_HW 0x00461a30
// FUNCTION: CARMA2_SW 0x004614d0
void C2_HOOK_FASTCALL TurnOnPanelFlics(void) {
    gPanel_flic_disable = 0;
}

// FUNCTION: CARMA2_HW 0x00461a40
// FUNCTION: CARMA2_SW 0x004614e0
int C2_HOOK_FASTCALL GetPanelFlicFrameIndex(int pIndex) {

    return gPanel_flic[pIndex].current_frame;
}

// FUNCTION: CARMA2_HW 0x00461a60
// FUNCTION: CARMA2_SW 0x00461500
void C2_HOOK_FASTCALL FlicPaletteAllocate(void) {

    gPalette_pixels = BrMemAllocate(0x400, kMem_misc);
    gPalette = DRPixelmapAllocate(BR_PMT_RGBX_888, 1, 256, gPalette_pixels, 0);
}

// AssertFlicPixelmap

// StartFlic

// FUNCTION: CARMA2_HW 0x00461d60
// FUNCTION: CARMA2_SW 0x004617f0
int C2_HOOK_FASTCALL EndFlic(tFlic_descriptor_ptr pFlic_info) {
    if (pFlic_info->f != NULL) {
        BrMemFree(pFlic_info->data_start);
        pFlic_info->data_start = NULL;
        PFfclose(pFlic_info->f);
        pFlic_info->f = NULL;
    }
    if (pFlic_info->data != NULL) {
        pFlic_info->data = NULL;
    }
    return 0;
}

// DoColourMap

// FUNCTION: CARMA2_HW 0x00462390
// FUNCTION: CARMA2_SW 0x00461e20
void C2_HOOK_FASTCALL DoDifferenceX(tFlic_descriptor* pFlic_info, tU32 pChunk_length) {
    int i;
    int j;
    int k;
    int skipped_lines;
    int number_of_lines;
    int number_of_packets;
    int skip_count;
    int size_count;
    tU8* pixel_ptr;
    tU8* line_pixel_ptr;
    tU8 the_byte;
    tU32 the_row_bytes;

    (void)pChunk_length;
    skipped_lines = MemReadU16(&pFlic_info->data);
    number_of_lines = MemReadU16(&pFlic_info->data);
    the_row_bytes = pFlic_info->the_pixelmap->row_bytes;
    pixel_ptr = pFlic_info->first_pixel + skipped_lines * the_row_bytes;
    for (i = 0; i < number_of_lines; i++) {
        line_pixel_ptr = pixel_ptr;
        number_of_packets = MemReadU8(&pFlic_info->data);
        for (j = 0; j < number_of_packets; j++) {
            skip_count = MemReadU8(&pFlic_info->data);
            size_count = MemReadS8(&pFlic_info->data);
            line_pixel_ptr += skip_count;
            if (size_count >= 0) {
                for (k = 0; k < size_count; k++) {
                    *line_pixel_ptr++ = *pFlic_info->data;
                    pFlic_info->data++;
                }
            } else {
                the_byte = *pFlic_info->data;
                pFlic_info->data++;
                for (k = 0; k < -size_count; k++) {
                    *line_pixel_ptr = the_byte;
                    line_pixel_ptr++;
                }
            }
        }
        pixel_ptr += the_row_bytes;
    }
}

// FUNCTION: CARMA2_HW 0x00462480
// FUNCTION: CARMA2_SW 0x00461f10
void C2_HOOK_FASTCALL DoDifferenceTrans(tFlic_descriptor* pFlic_info, tU32 pChunk_length) {
    int i;
    int j;
    int k;
    int skipped_lines;
    int number_of_lines;
    int number_of_packets;
    int skip_count;
    int size_count;
    tU8* pixel_ptr;
    tU8* line_pixel_ptr;
    tU8 the_byte;
    tU32 the_row_bytes;

    (void)pChunk_length;
    skipped_lines = MemReadU16(&pFlic_info->data);
    number_of_lines = MemReadU16(&pFlic_info->data);
    the_row_bytes = pFlic_info->the_pixelmap->row_bytes;
    pixel_ptr = pFlic_info->first_pixel + skipped_lines * the_row_bytes;
    for (i = 0; i < number_of_lines; i++) {
        line_pixel_ptr = pixel_ptr;
        number_of_packets = MemReadU8(&pFlic_info->data);
        for (j = 0; j < number_of_packets; j++) {
            skip_count = MemReadU8(&pFlic_info->data);
            size_count = MemReadS8(&pFlic_info->data);
            line_pixel_ptr += skip_count;
            if (size_count >= 0) {
                for (k = 0; k < size_count; k++) {
                    the_byte = *pFlic_info->data++;
                    if (the_byte != 0) {
                        *line_pixel_ptr = the_byte;
                        line_pixel_ptr++;
                    } else {
                        line_pixel_ptr++;
                    }
                }
            } else {
                the_byte = *pFlic_info->data++;
                if (the_byte != 0) {
                    for (k = 0; k < -size_count; k++) {
                        *line_pixel_ptr = the_byte;
                        line_pixel_ptr++;
                    }
                } else {
                    line_pixel_ptr += size_count;
                }
            }
        }
        pixel_ptr += the_row_bytes;
    }
}

// DoColour256

// DoDeltaTrans

// FUNCTION: CARMA2_HW 0x00462580
// FUNCTION: CARMA2_SW 0x00462010
void C2_HOOK_FASTCALL DoBlack(tFlic_descriptor* pFlic_info, tU32 pChunk_length) {
    int i;
    int j;
    int the_width;
    tU8* pixel_ptr;
    tU32 the_row_bytes;
    tU32* line_pixel_ptr;

    pixel_ptr = pFlic_info->first_pixel;
    the_row_bytes = pFlic_info->the_pixelmap->row_bytes;
    the_width = pFlic_info->width >> 2;
    for (i = 0; i < pFlic_info->height; i++) {
        line_pixel_ptr = (tU32*)pixel_ptr;
        for (j = 0; j < the_width; j++) {
            *line_pixel_ptr = 0;
            line_pixel_ptr++;
        }
        pixel_ptr += the_row_bytes;
    }
}

// FUNCTION: CARMA2_HW 0x004626a0
// FUNCTION: CARMA2_SW 0x00462130
void C2_HOOK_FASTCALL DoRunLengthTrans(tFlic_descriptor* pFlic_info, tU32 pChunk_length) {
    int i;
    int j;
    int k;
    int number_of_packets;
    int size_count;
    tU8* pixel_ptr;
    tU8* line_pixel_ptr;
    tU8 the_byte;
    tU32 the_row_bytes;

    (void)pChunk_length;
    the_row_bytes = pFlic_info->the_pixelmap->row_bytes;
    pixel_ptr = pFlic_info->first_pixel;
    for (i = 0; i < pFlic_info->height; i++) {
        line_pixel_ptr = pixel_ptr;
        number_of_packets = MemReadU8(&pFlic_info->data);
        for (j = 0; j < number_of_packets; j++) {
            size_count = MemReadS8(&pFlic_info->data);
            if (size_count < 0) {
                for (k = 0; k < -size_count; k++) {
                    the_byte = MemReadU8(&pFlic_info->data);
                    if (the_byte != 0) {
                        *line_pixel_ptr = the_byte;
                        line_pixel_ptr++;
                    } else {
                        line_pixel_ptr++;
                    }
                }
            } else {
                the_byte = MemReadU8(&pFlic_info->data);
                if (the_byte != 0) {
                    for (k = 0; k < size_count; k++) {
                        *line_pixel_ptr = the_byte;
                        line_pixel_ptr++;
                    }
                } else {
                    line_pixel_ptr += size_count;
                }
            }
        }
        pixel_ptr += the_row_bytes;
    }
}

// FUNCTION: CARMA2_HW 0x00462780
// FUNCTION: CARMA2_SW 0x00462210
void C2_HOOK_FASTCALL DoUncompressed(tFlic_descriptor* pFlic_info, tU32 pChunk_length) {
    int i;
    int j;
    int the_width;
    tU8* pixel_ptr;
    tU32 the_row_bytes;
    tU32* line_pixel_ptr;

    (void)pChunk_length;
    pixel_ptr = pFlic_info->first_pixel;
    the_row_bytes = pFlic_info->the_pixelmap->row_bytes;
    the_width = pFlic_info->width >> 2;
    for (i = 0; i < pFlic_info->height; i++) {
        line_pixel_ptr = (tU32*)pixel_ptr;
        for (j = 0; j < the_width; j++) {
            // FIXME: big endian incompatible
            *line_pixel_ptr = MemReadU32(&pFlic_info->data);
            line_pixel_ptr++;
        }
        pixel_ptr += the_row_bytes;
    }
}

// FUNCTION: CARMA2_HW 0x004627f0
// FUNCTION: CARMA2_SW 0x00462280
void C2_HOOK_FASTCALL DoUncompressedTrans(tFlic_descriptor* pFlic_info, tU32 pChunk_length) {
    int i;
    int j;
    int the_width;
    tU8* pixel_ptr;
    tU8* line_pixel_ptr;
    tU8 the_byte;
    tU32 the_row_bytes;

    (void)pChunk_length;
    pixel_ptr = pFlic_info->first_pixel;
    the_row_bytes = pFlic_info->the_pixelmap->row_bytes;
    the_width = pFlic_info->width;
    for (i = 0; i < pFlic_info->height; i++) {
        line_pixel_ptr = pixel_ptr;
        for (j = 0; j < the_width; j++) {
            the_byte = (tU8)MemReadU32(&pFlic_info->data);
            if (the_byte != '\0') {
                *line_pixel_ptr = the_byte;
                line_pixel_ptr++;
            } else {
                line_pixel_ptr++;
            }
        }
        pixel_ptr += the_row_bytes;
    }
}

// DoMini

// DrawTranslations

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x00461da0
int C2_HOOK_FASTCALL PlayNextFlicFrame(tFlic_descriptor* pFlic_info) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00462a40
// FUNCTION: CARMA2_SW 0x004624d0
void C2_HOOK_FASTCALL InitFlics(void) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gMain_flic_list); i++) {
        gMain_flic_list[i].data_ptr = NULL;
    }
}

// FUNCTION: CARMA2_HW 0x00462a60
// FUNCTION: CARMA2_SW 0x004624f0
int C2_HOOK_FASTCALL LoadFlic(int pIndex) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x00462a70
// FUNCTION: CARMA2_SW 0x00462500
void C2_HOOK_FASTCALL UnlockFlic(int pIndex) {
    if (pIndex < 0) {
        return;
    }
    if (gMain_flic_list[pIndex].data_ptr != NULL) {
        MAMSUnlock((void**)&gMain_flic_list[pIndex].data_ptr);
    }
}

// FUNCTION: CARMA2_HW 0x00462a90
// FUNCTION: CARMA2_SW 0x00462520
int C2_HOOK_FASTCALL LoadFlicData(char* pName, tU8** pData, tU32* pData_length) {
    FILE* f;
    tPath_name the_path;

    if (*pData == NULL) {
        if (!gPlay_from_disk) {
            PossibleService();
            PathCat(the_path, gApplication_path, "ANIM");
            PathCat(the_path, the_path, pName);
            f = DRfopen(the_path, "rb");
            if (f == NULL) {
                return 0;
            }
            *pData_length = GetFileLength(f);
            *pData = BrMemAllocate(*pData_length, kMem_misc);
            if (*pData == NULL) {
                PFfclose(f);
                return 0;
            }
            PFfread(*pData, 1, *pData_length, f);
            PFfclose(f);
        }
    } else {
        MAMSLock((void**)pData);
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x00462b80
// FUNCTION: CARMA2_SW 0x00462610
void C2_HOOK_FASTCALL FreeFlic(int pIndex) {

    PossibleService();
    if (gMain_flic_list[pIndex].data_ptr != NULL) {
        BrMemFree(gMain_flic_list[pIndex].data_ptr);
        gMain_flic_list[pIndex].data_ptr = NULL;
    }
}

// FUNCTION: CARMA2_HW 0x00462c20
// FUNCTION: CARMA2_SW 0x004626b0
void C2_HOOK_FASTCALL RunFlic(int pIndex) {
    (void)pIndex;
}

// FUNCTION: CARMA2_HW 0x00462c30
// FUNCTION: CARMA2_SW 0x004626c0
void C2_HOOK_FASTCALL PreloadBunchOfFlics(int pBunch_index) {

}

// FUNCTION: CARMA2_HW 0x00462c40
// FUNCTION: CARMA2_SW 0x004626d0
void C2_HOOK_FASTCALL UnlockBunchOfFlics(int pBunch_index) {
    int i;

    C2_HOOK_BUG_ON(REC2_ASIZE(gFlic_bunch0) != 29);
    C2_HOOK_BUG_ON(REC2_ASIZE(gFlic_bunch1) != 31);
    C2_HOOK_BUG_ON(REC2_ASIZE(gFlic_bunch2) != 8);
    C2_HOOK_BUG_ON(REC2_ASIZE(gFlic_bunch3) != 13);
    C2_HOOK_BUG_ON(REC2_ASIZE(gFlic_bunch4) != 22);
    C2_HOOK_BUG_ON(REC2_ASIZE(gFlic_bunch5) != 5);
    C2_HOOK_BUG_ON(REC2_ASIZE(gFlic_bunch6) != 51);
    C2_HOOK_BUG_ON(REC2_ASIZE(gFlic_bunch7) != 7);
    C2_HOOK_BUG_ON(REC2_ASIZE(gFlic_bunch8) != 16);
    C2_HOOK_BUG_ON(REC2_ASIZE(gFlic_bunch) != 9);

    for (i = 0; i < gFlic_bunch[pBunch_index].count; i++) {
        UnlockFlic(gFlic_bunch[pBunch_index].indexes[i]);
    }
}

// FUNCTION: CARMA2_HW 0x00462cc0
// FUNCTION: CARMA2_SW 0x00462750
void C2_HOOK_FASTCALL InitFlicQueue(void) {

    gFirst_flic = NULL;
}

// FUNCTION: CARMA2_HW 0x00462cd0
int C2_HOOK_FASTCALL FlicQueueFinished(void) {
    tFlic_descriptor* the_flic;

    the_flic = gFirst_flic;
    while (the_flic != NULL) {
        if (the_flic->must_finish) {
            return 0;
        }
        the_flic = the_flic->next;
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x00462d00
void C2_HOOK_FASTCALL ProcessFlicQueue(tU32 pInterval) {
    tFlic_descriptor* the_flic;
    tFlic_descriptor* last_flic;
    tFlic_descriptor* doomed_flic;
    tU32 new_time;
    int finished_playing;

    (void)pInterval;
    DontLetFlicFuckWithPalettes();
    TurnFlicTransparencyOn();
    the_flic = gFirst_flic;
    new_time = PDGetTotalTime();
    last_flic = NULL;
    while (the_flic != NULL) {
        if (new_time - the_flic->last_frame >= the_flic->frame_period) {
            the_flic->last_frame = new_time;
            finished_playing = PlayNextFlicFrame(the_flic);
        } else {
            finished_playing = 0;
        }
        if (finished_playing) {
            EndFlic(the_flic);
            if (last_flic != NULL) {
                last_flic->next = the_flic->next;
            } else {
                gFirst_flic = the_flic->next;
            }
            doomed_flic = the_flic;
            the_flic = the_flic->next;
            BrMemFree(doomed_flic);
        } else {
            last_flic = the_flic;
            the_flic = the_flic->next;
        }
    }
    TurnFlicTransparencyOff();
    LetFlicFuckWithPalettes();
}

// STUB: CARMA2_HW 0x00462dc0
// STUB: CARMA2_SW 0x00462850
void C2_HOOK_FASTCALL FlushFlicQueue(void) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004630b0
// FUNCTION: CARMA2_SW 0x00462b40
void C2_HOOK_FASTCALL DisposeFlicPanel(int pIndex) {

    EndFlic(&gPanel_flic[pIndex]);
    BrMemFree(gPanel_buffer[pIndex]->pixels);
    BrPixelmapFree(gPanel_buffer[pIndex]);
    gPanel_buffer[pIndex] = NULL;
}

// FUNCTION: CARMA2_HW 0x00463340
// FUNCTION: CARMA2_SW 0x00462dd0
void C2_HOOK_FASTCALL LoadInterfaceStrings(void) {
    FILE* f;
    char s[256];
    char s2[256];
    char* str;
    char ch;
    tPath_name the_path;
    int i;
    int j;

    gTranslation_count = 0;
    PathCat(the_path, gApplication_path, "TRNSLATE.TXT");
    f = PFfopen(the_path, "rt");
    if (f == NULL) {
        return;
    }
    while (!PFfeof(f)) {
        GetALineAndDontArgue(f, s);
        gTranslation_count++;
    }
    PFrewind(f);
    gTranslations = BrMemAllocate(gTranslation_count * sizeof(tTranslation_record), kMem_misc);
    for (i = 0; i < gTranslation_count; i++) {
        GetALineAndDontArgue(f, s);
        str = strtok(s, "\t ,/");
        strcpy(s2, str);
        strtok(s2, ".");
        strcat(s2, ".FLI");
        gTranslations[i].flic_index = -1;
        for (j = 0; j < (int)REC2_ASIZE(gMain_flic_list); j++) {
            if (strcmp(gMain_flic_list[j].file_name, s2) == 0) {
                gTranslations[i].flic_index = j;
                break;
            }
        }
        if (gTranslations[i].flic_index < 0) {
            FatalError(kFatalError_CannotFindFlicReferencedTranslation_S, s2);
        }
        str[strlen(str)] = ',';
        strtok(s, "\t ,/");
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &gTranslations[i].x);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &gTranslations[i].y);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%d", &gTranslations[i].font_index);
        str = strtok(NULL, "\t ,/");
        sscanf(str, "%c", &ch);
        switch (ch) {
            case 'L':
            case 'l':
                gTranslations[i].justification = eJust_left;
                break;
            case 'R':
            case 'r':
                gTranslations[i].justification = eJust_right;
                break;
            case 'C':
            case 'c':
                gTranslations[i].justification = eJust_centre;
                break;
        }
        str += strlen(str) + 1;
        gTranslations[i].text = BrMemAllocate(strlen(str) + 1, kMem_misc);
        strcpy(gTranslations[i].text, str);
    }
    LoadFont(1);
    LoadFont(2);
    gTrans_fonts[0] = &gFonts[1];
    gTrans_fonts[1] = &gFonts[2];

}

// FUNCTION: CARMA2_HW 0x00463700
// FUNCTION: CARMA2_SW 0x00463190
void C2_HOOK_FASTCALL SuspendPendingFlic(void) {
    gPending_pending_flic = gPending_flic;
    gPending_flic = -1;
}

// FUNCTION: CARMA2_HW 0x00463720
// FUNCTION: CARMA2_SW 0x004631b0
void C2_HOOK_FASTCALL ResumePendingFlic(void) {
    gPending_flic = gPending_pending_flic;
}

