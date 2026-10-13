#include "38-flicplay.h"

#include "08-loading1.h"
#include "18-graphics2.h"
#include "19-font.h"
#include "41-utility.h"
#include "46-win95sys.h"
#include "52-errors.h"
#include "70-packfile.h"
#include "globvars.h"
#include "platform.h"
#include "rec2_macros.h"

#include "c2_stdio.h"
#include "c2_string.h"

// GLOBAL: CARMA2_HW 0x005964c8
int gPending_flic = -1;

// GLOBAL: CARMA2_HW 0x0068620c
int gPending_pending_flic;

// GLOBAL: CARMA2_HW 0x00686310
int gTransparency_on;

// GLOBAL: CARMA2_HW 0x00686314
int gPanel_flic_disable;

// GLOBAL: CARMA2_HW 0x00686318
int gPlay_from_disk;

// GLOBAL: CARMA2_HW 0x00686330
char gLast_flic_name[14];

// GLOBAL: CARMA2_HW 0x0068b8b4
float gFlic_sound_delay_pre_smack;

// GLOBAL: CARMA2_HW 0x0068b8b8
float gFlic_sound_delay_post_smack;

// GLOBAL: CARMA2_HW 0x0068b8bc
float gFlic_sound_delay_not_in_demo;

// GLOBAL: CARMA2_HW 0x0068b8c0
float gFlic_sound_delay_post_demo;

// GLOBAL: CARMA2_HW 0x006861dc
int gTranslation_count;

// GLOBAL: CARMA2_HW 0x006861e8
tFlic_descriptor* gFirst_flic;

// GLOBAL: CARMA2_HW 0x006861d8
void* gPalette_pixels;

// GLOBAL: CARMA2_HW 0x00686308
int gPalette_fuck_prevention;

// GLOBAL: CARMA2_HW 0x00686208
br_pixelmap* gPalette;

// GLOBAL: CARMA2_HW 0x00686210
tTranslation_record* gTranslations;

// GLOBAL: CARMA2_HW 0x006861e0
tDR_font* gTrans_fonts[2];

// GLOBAL: CARMA2_HW 0x005964d0
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
int C2_HOOK_FASTCALL TranslationMode(void) {

    return gTranslation_count;
}

// FUNCTION: CARMA2_HW 0x004619a0
void C2_HOOK_FASTCALL DontLetFlicFuckWithPalettes(void) {
    gPalette_fuck_prevention = 1;
}

// FUNCTION: CARMA2_HW 0x004619b0
void C2_HOOK_FASTCALL LetFlicFuckWithPalettes(void) {
    gPalette_fuck_prevention = 0;
}

// FUNCTION: CARMA2_HW 0x004619e0
void C2_HOOK_FASTCALL TurnFlicTransparencyOn(void) {
    gTransparency_on = 1;
}

// FUNCTION: CARMA2_HW 0x004619f0
void C2_HOOK_FASTCALL TurnFlicTransparencyOff(void) {
    gTransparency_on = 0;
}

// FUNCTION: CARMA2_HW 0x00461a00
void C2_HOOK_FASTCALL PlayFlicsFromDisk(void) {
    gPlay_from_disk = 1;
}

// FUNCTION: CARMA2_HW 0x00461a10
void C2_HOOK_FASTCALL PlayFlicsFromMemory(void) {
    gPlay_from_disk = 0;
}

// FUNCTION: CARMA2_HW 0x00461a20
void C2_HOOK_FASTCALL TurnOffPanelFlics(void) {
    gPanel_flic_disable = 1;
}

// FUNCTION: CARMA2_HW 0x00461a30
void C2_HOOK_FASTCALL TurnOnPanelFlics(void) {
    gPanel_flic_disable = 0;
}

// FUNCTION: CARMA2_HW 0x00461a60
void C2_HOOK_FASTCALL FlicPaletteAllocate(void) {

    gPalette_pixels = BrMemAllocate(0x400, kMem_misc);
    gPalette = DRPixelmapAllocate(BR_PMT_RGBX_888, 1, 256, gPalette_pixels, 0);
}

// FUNCTION: CARMA2_HW 0x00461aa0
void C2_HOOK_FASTCALL AssertFlicPixelmap(tFlic_descriptor_ptr pFlic_info, br_pixelmap* pDest_pixelmap) {
    if (pDest_pixelmap != NULL) {
        pFlic_info->first_pixel = (tU8*)pDest_pixelmap->pixels
            + (pFlic_info->x_offset + pFlic_info->y_offset * pDest_pixelmap->row_bytes);
    }
    pFlic_info->the_pixelmap = pDest_pixelmap;
}

// FUNCTION: CARMA2_HW 0x00461ad0
int C2_HOOK_FASTCALL StartFlic(char* pFile_name, int pIndex, tFlic_descriptor_ptr pFlic_info, tU32 pSize, tS8* pData_ptr, br_pixelmap* pDest_pixelmap, int pX_offset, int pY_offset, int pFrame_rate) {
    tU16 claimed_speed;
    tU16 magic_number;
    tPath_name the_path;
    int total_size;

    (void)pSize;
    if (gPlay_from_disk) {
        PathCat(the_path, gApplication_path, "ANIM");
        PathCat(the_path, the_path, pFile_name);
        pFlic_info->f = DRfopen(the_path, "rb");
        if (pFlic_info->f == NULL) {
            FatalError(kFatalError_CouldNotOpenFlicFile_S, pFile_name);
        }
        total_size = GetFileLength(pFlic_info->f);
        if (total_size < 75000) {
            pFlic_info->bytes_in_buffer = total_size;
        } else {
            pFlic_info->bytes_in_buffer = 75000;
        }
        if (pFlic_info->data_start == NULL) {
            pFlic_info->data_start = BrMemAllocate(pFlic_info->bytes_in_buffer, kMem_misc);
        }
        pFlic_info->data = pFlic_info->data_start;
        PFfread(pFlic_info->data_start, 1, pFlic_info->bytes_in_buffer, pFlic_info->f);
        strcpy(gLast_flic_name, pFile_name);
        pFlic_info->bytes_still_to_be_read = total_size - pFlic_info->bytes_in_buffer;
    } else {
        pFlic_info->data = (char*)pData_ptr;
        pFlic_info->f = NULL;
    }
    pFlic_info->bytes_remaining = MemReadU32(&pFlic_info->data);
    magic_number = MemReadU16(&pFlic_info->data);
    if (magic_number == 0xaf11) {
        pFlic_info->new_format = 0;
    } else if (magic_number == 0xaf12) {
        pFlic_info->new_format = 1;
    } else {
        return -1;
    }
    pFlic_info->frames_left = MemReadU16(&pFlic_info->data);
    pFlic_info->current_frame = 0;
    pFlic_info->width = MemReadU16(&pFlic_info->data);
    pFlic_info->height = MemReadU16(&pFlic_info->data);
    if (MemReadU16(&pFlic_info->data) != 8) {
        FatalError(kFatalError_FlicFileWasNot8BitsDeep_S, gLast_flic_name);
    }
    MemSkipBytes(&pFlic_info->data, 2);
    claimed_speed = MemReadU16(&pFlic_info->data);
    MemSkipBytes(&pFlic_info->data, 0x6e);
    pFlic_info->the_pixelmap = pDest_pixelmap;
    if (pX_offset != -1) {
        pFlic_info->x_offset = pX_offset;
    } else if (pDest_pixelmap != NULL) {
        pFlic_info->x_offset = (pDest_pixelmap->width - pFlic_info->width) / 2;
    } else {
        pFlic_info->x_offset = 0;
    }
    if (pY_offset != -1) {
        pFlic_info->y_offset = pY_offset;
    } else if (pDest_pixelmap != NULL) {
        pFlic_info->y_offset = (pDest_pixelmap->height - pFlic_info->height) / 2;
    } else {
        pFlic_info->y_offset = 0;
    }
    if (pFrame_rate != 0) {
        pFlic_info->frame_period = 1000 / pFrame_rate;
    } else if (claimed_speed == 0) {
        FatalError(kFatalError_CouldNotAscertainFrameRateForFlicFile, gLast_flic_name);
    } else if (pFlic_info->new_format != 0) {
        pFlic_info->frame_period = claimed_speed;
    } else {
        pFlic_info->frame_period = 14 * claimed_speed;
    }
    pFlic_info->the_index = pIndex;
    AssertFlicPixelmap(pFlic_info, pDest_pixelmap);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00461d60
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

void C2_HOOK_FASTCALL DoColourMap(tFlic_descriptor_ptr pFlic_info, tU32 chunk_length) {
    int i;
    int j;
    int packet_count;
    int skip_count;
    int change_count;
    int current_colour;
    tU8* palette_pixels;
    tU8 red;
    tU8 green;
    tU8 blue;

    (void)chunk_length;
    current_colour = 0;
    packet_count = MemReadU16(&pFlic_info->data);
    for (i = 0; i < packet_count; i++) {
        skip_count = MemReadU8(&pFlic_info->data);
        change_count = MemReadU8(&pFlic_info->data);
        if (change_count == 0) {
            change_count = 256;
        }
        current_colour += skip_count;
        palette_pixels = (tU8*)gPalette_pixels + current_colour * 4;
        for (j = 0; j < change_count; j++) {
            red = MemReadU8(&pFlic_info->data);
            green = MemReadU8(&pFlic_info->data);
            blue = MemReadU8(&pFlic_info->data);
            palette_pixels[0] = (tU8)(blue * 4);
            palette_pixels[1] = (tU8)(green * 4);
            palette_pixels[2] = (tU8)(red * 4);
            palette_pixels[3] = 0;
            palette_pixels += 4;
        }
        if (!gPalette_fuck_prevention) {
            DRSetPaletteEntries(gPalette, current_colour, change_count);
        }
    }
}

// FUNCTION: CARMA2_HW 0x00462390
void C2_HOOK_FASTCALL DoDifferenceX(tFlic_descriptor* pFlic_info, tU32 chunk_length) {
    int i;
    int j;
    int k;
    int skipped_lines;
    int number_of_lines;
    int number_of_packets;
    int skip_count;
    int size_count;
    int the_row_bytes;
    tU8* pixel_ptr;
    tU8* line_pixel_ptr;
    tU8 the_byte;

    (void)chunk_length;
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
                    *line_pixel_ptr = MemReadU8(&pFlic_info->data);
                    line_pixel_ptr++;
                }
            } else {
                the_byte = MemReadU8(&pFlic_info->data);
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
void C2_HOOK_FASTCALL DoDifferenceTrans(tFlic_descriptor* pFlic_info, tU32 chunk_length) {
    int i;
    int j;
    int k;
    int skipped_lines;
    int number_of_lines;
    int number_of_packets;
    int skip_count;
    int size_count;
    int the_row_bytes;
    tU8* pixel_ptr;
    tU8* line_pixel_ptr;
    tU8 the_byte;

    (void)chunk_length;
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
                    the_byte = MemReadU8(&pFlic_info->data);
                    if (the_byte != 0) {
                        *line_pixel_ptr = the_byte;
                    }
                    line_pixel_ptr++;
                }
            } else {
                the_byte = MemReadU8(&pFlic_info->data);
                if (the_byte != 0) {
                    for (k = 0; k < -size_count; k++) {
                        *line_pixel_ptr = the_byte;
                        line_pixel_ptr++;
                    }
                } else {
                    /* A transparent repeat rewinds the output position by its signed size. */
                    line_pixel_ptr += size_count;
                }
            }
        }
        pixel_ptr += the_row_bytes;
    }
}

void C2_HOOK_FASTCALL DoColour256(tFlic_descriptor* pFlic_info, tU32 chunk_length) {
    int i;
    int j;
    int packet_count;
    int skip_count;
    int change_count;
    int current_colour;
    tU8* palette_pixels;
    tU8 red;
    tU8 green;
    tU8 blue;

    (void)chunk_length;
    current_colour = 0;
    packet_count = MemReadU16(&pFlic_info->data);
    for (i = 0; i < packet_count; i++) {
        skip_count = MemReadU8(&pFlic_info->data);
        change_count = MemReadU8(&pFlic_info->data);
        if (change_count == 0) {
            change_count = 256;
        }
        current_colour += skip_count;
        palette_pixels = (tU8*)gPalette_pixels + current_colour * 4;
        for (j = 0; j < change_count; j++) {
            red = MemReadU8(&pFlic_info->data);
            green = MemReadU8(&pFlic_info->data);
            blue = MemReadU8(&pFlic_info->data);
            palette_pixels[0] = blue;
            palette_pixels[1] = green;
            palette_pixels[2] = red;
            palette_pixels[3] = 0;
            palette_pixels += 4;
        }
        if (!gPalette_fuck_prevention) {
            DRSetPaletteEntries(gPalette, current_colour, change_count);
        }
    }
}

void C2_HOOK_FASTCALL DoDeltaTrans(tFlic_descriptor* pFlic_info, tU32 chunk_length) {
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
    tU8 the_byte;
    tU8 the_byte2;

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
                    the_byte = MemReadU8(&pFlic_info->data);
                    the_byte2 = MemReadU8(&pFlic_info->data);
                    for (k = 0; k < -size_count; k++) {
                        if (the_byte != 0) {
                            *line_pixel_ptr = the_byte;
                        }
                        line_pixel_ptr++;
                        if (the_byte2 != 0) {
                            *line_pixel_ptr = the_byte2;
                        }
                        line_pixel_ptr++;
                    }
                } else {
                    for (k = 0; k < size_count; k++) {
                        the_byte = MemReadU8(&pFlic_info->data);
                        if (the_byte != 0) {
                            *line_pixel_ptr = the_byte;
                        }
                        line_pixel_ptr++;
                        the_byte2 = MemReadU8(&pFlic_info->data);
                        if (the_byte2 != 0) {
                            *line_pixel_ptr = the_byte2;
                        }
                        line_pixel_ptr++;
                    }
                }
            }
            pixel_ptr += the_row_bytes;
            i++;
        }
    }
}

// FUNCTION: CARMA2_HW 0x00462580
void C2_HOOK_FASTCALL DoBlack(tFlic_descriptor* pFlic_info, tU32 chunk_length) {
    int i;
    int words_per_row;
    int the_row_bytes;
    tU8* pixel_ptr;

    (void)chunk_length;
    the_row_bytes = pFlic_info->the_pixelmap->row_bytes;
    pixel_ptr = pFlic_info->first_pixel;
    words_per_row = pFlic_info->width / 4;
    for (i = 0; i < pFlic_info->height; i++) {
        if (words_per_row > 0) {
            memset(pixel_ptr, 0, words_per_row * sizeof(tU32));
        }
        pixel_ptr += the_row_bytes;
    }
}

// FUNCTION: CARMA2_HW 0x004626a0
void C2_HOOK_FASTCALL DoRunLengthTrans(tFlic_descriptor* pFlic_info, tU32 chunk_length) {
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
                    if (the_byte != 0) {
                        *line_pixel_ptr = the_byte;
                    }
                    line_pixel_ptr++;
                }
            } else {
                for (k = 0; k < -size_count; k++) {
                    the_byte = MemReadU8(&pFlic_info->data);
                    if (the_byte != 0) {
                        *line_pixel_ptr = the_byte;
                    }
                    line_pixel_ptr++;
                }
            }
        }
        pixel_ptr += the_row_bytes;
    }
}

// FUNCTION: CARMA2_HW 0x00462780
void C2_HOOK_FASTCALL DoUncompressed(tFlic_descriptor* pFlic_info, tU32 chunk_length) {
    int i;
    int j;
    int words_per_row;
    int the_row_bytes;
    tU8* pixel_ptr;
    tU8* line_pixel_ptr;
    tU32 the_long;

    (void)chunk_length;
    the_row_bytes = pFlic_info->the_pixelmap->row_bytes;
    pixel_ptr = pFlic_info->first_pixel;
    words_per_row = pFlic_info->width / 4;
    for (i = 0; i < pFlic_info->height; i++) {
        line_pixel_ptr = pixel_ptr;
        for (j = 0; j < words_per_row; j++) {
            the_long = MemReadU32(&pFlic_info->data);
            memcpy(line_pixel_ptr, &the_long, sizeof(the_long));
            line_pixel_ptr += sizeof(the_long);
        }
        pixel_ptr += the_row_bytes;
    }
}

// FUNCTION: CARMA2_HW 0x004627f0
void C2_HOOK_FASTCALL DoUncompressedTrans(tFlic_descriptor* pFlic_info, tU32 chunk_length) {
    int i;
    int j;
    int the_row_bytes;
    int width;
    tU8* pixel_ptr;
    tU8* line_pixel_ptr;
    tU8 the_byte;

    (void)chunk_length;
    the_row_bytes = pFlic_info->the_pixelmap->row_bytes;
    pixel_ptr = pFlic_info->first_pixel;
    width = pFlic_info->width;
    for (i = 0; i < pFlic_info->height; i++) {
        line_pixel_ptr = pixel_ptr;
        for (j = 0; j < width; j++) {
            /* Transparent pixels use the low byte of each four-byte sample. */
            the_byte = (tU8)MemReadU32(&pFlic_info->data);
            if (the_byte != 0) {
                *line_pixel_ptr = the_byte;
            }
            line_pixel_ptr++;
        }
        pixel_ptr += the_row_bytes;
    }
}

// DoMini

// FUNCTION: CARMA2_HW 0x00462860
void C2_HOOK_FASTCALL DrawTranslations(tFlic_descriptor* pFlic_info) {
    tTranslation_record* trans;
    int i;
    int x;
    int width;
    int right_edge;

    x = 0;
    right_edge = 0;
    for (i = 0, trans = gTranslations; i < gTranslation_count; i++, trans++) {
        if (trans->flic_index == pFlic_info->the_index) {
            width = DRTextWidth(gTrans_fonts[trans->font_index], trans->text);
            switch (trans->justification) {
                case eJust_left:
                    x = pFlic_info->x_offset + trans->x;
                    right_edge = x + width;
                    break;
                case eJust_right:
                    x = pFlic_info->x_offset - width + trans->x;
                    right_edge = x;
                    break;
                case eJust_centre:
                    x = pFlic_info->x_offset + trans->x - width / 2;
                    right_edge = x + width / 2;
                    break;
            }
            DRPixelmapText(pFlic_info->the_pixelmap, x, pFlic_info->y_offset + trans->y, gTrans_fonts[trans->font_index], trans->text, right_edge);
        }
    }
}

// FUNCTION: CARMA2_HW 0x00461da0
int C2_HOOK_FASTCALL PlayNextFlicFrame(tFlic_descriptor* pFlic_info) {
    tU32 frame_length;
    tU32 chunk_length;
    int magic_bytes;
    int chunk_count;
    int chunk_counter;
    int chunk_type;
    int finished;
    int data_knocked_off;
    int read_amount;

    PossibleService();
    frame_length = MemReadU32(&pFlic_info->data);
    magic_bytes = MemReadU16(&pFlic_info->data);
    chunk_count = MemReadU16(&pFlic_info->data);
    MemSkipBytes(&pFlic_info->data, 8);
    if (magic_bytes == 0xf1fa) {
        for (chunk_counter = 0; chunk_counter < chunk_count; chunk_counter++) {
            chunk_length = MemReadU32(&pFlic_info->data);
            chunk_type = MemReadU16(&pFlic_info->data);
            switch (chunk_type) {
                case 4:
                    DoColour256(pFlic_info, chunk_length);
                    break;
                case 7:
                    if (gTransparency_on) {
                        DoDeltaTrans(pFlic_info, chunk_length);
                    } else {
                        DoDeltaX(pFlic_info, chunk_length);
                    }
                    break;
                case 11:
                    DoColourMap(pFlic_info, chunk_length);
                    break;
                case 12:
                    if (gTransparency_on) {
                        DoDifferenceTrans(pFlic_info, chunk_length);
                    } else {
                        DoDifferenceX(pFlic_info, chunk_length);
                    }
                    break;
                case 13:
                    DoBlack(pFlic_info, chunk_length);
                    break;
                case 15:
                    if (gTransparency_on) {
                        DoRunLengthTrans(pFlic_info, chunk_length);
                    } else {
                        DoRunLengthX(pFlic_info, chunk_length);
                    }
                    break;
                case 16:
                    if (gTransparency_on) {
                        DoUncompressedTrans(pFlic_info, chunk_length);
                    } else {
                        DoUncompressed(pFlic_info, chunk_length);
                    }
                    break;
                default:
                    MemSkipBytes(&pFlic_info->data, (int)(chunk_length - 6));
                    break;
            }
            /* Advance to the next two-byte chunk boundary. */
            pFlic_info->data = (char*)(((uintptr_t)pFlic_info->data + 1) & ~(uintptr_t)1);
        }
    } else {
        MemSkipBytes(&pFlic_info->data, (int)(frame_length - 16));
        pFlic_info->frames_left++;
        pFlic_info->current_frame--;
    }
    pFlic_info->current_frame++;
    pFlic_info->frames_left--;
    finished = pFlic_info->frames_left == 0;
    if (finished && gTranslation_count != 0) {
        DrawTranslations(pFlic_info);
    }
    if (pFlic_info->f != NULL && pFlic_info->bytes_still_to_be_read != 0) {
        data_knocked_off = (int)(pFlic_info->data - pFlic_info->data_start);
        memmove(pFlic_info->data_start, pFlic_info->data, pFlic_info->bytes_in_buffer - data_knocked_off);
        pFlic_info->data = pFlic_info->data_start;
        pFlic_info->bytes_in_buffer -= data_knocked_off;
        if (data_knocked_off < pFlic_info->bytes_still_to_be_read) {
            read_amount = data_knocked_off;
        } else {
            read_amount = pFlic_info->bytes_still_to_be_read;
        }
        if (read_amount != 0) {
            PFfread(pFlic_info->data_start + pFlic_info->bytes_in_buffer, 1, read_amount, pFlic_info->f);
        }
        pFlic_info->bytes_in_buffer += read_amount;
        pFlic_info->bytes_still_to_be_read -= read_amount;
    }
    return finished;
}

// FUNCTION: CARMA2_HW 0x00462a40
void C2_HOOK_FASTCALL InitFlics(void) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gMain_flic_list); i++) {
        gMain_flic_list[i].data_ptr = NULL;
    }
}

// FUNCTION: CARMA2_HW 0x00462a60
int C2_HOOK_FASTCALL LoadFlic(int pIndex) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x00462a70
void C2_HOOK_FASTCALL UnlockFlic(int pIndex) {
    if (pIndex >= 0 && gMain_flic_list[pIndex].data_ptr != NULL) {
        MAMSUnlock((void**)&gMain_flic_list[pIndex].data_ptr);
    }
}

// FUNCTION: CARMA2_HW 0x00462a90
int C2_HOOK_FASTCALL LoadFlicData(char* pName, tU8** pData, tU32* pData_length) {
    FILE* f;
    tPath_name the_path;

    if (*pData != NULL) {
        MAMSLock((void**)pData);
        return 1;
    }
    if (gPlay_from_disk) {
        return 1;
    }
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
    return 1;
}

// FUNCTION: CARMA2_HW 0x00462cc0
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
    last_flic = NULL;
    new_time = PDGetTotalTime();
    while (the_flic != NULL) {
        if (new_time - the_flic->last_frame < the_flic->frame_period) {
            finished_playing = 0;
        } else {
            the_flic->last_frame = new_time;
            finished_playing = PlayNextFlicFrame(the_flic);
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

// FUNCTION: CARMA2_HW 0x00462dc0
void C2_HOOK_FASTCALL FlushFlicQueue(void) {
    tFlic_descriptor* the_flic;
    tFlic_descriptor* doomed_flic;

    while (!FlicQueueFinished()) {
        RemoveTransientBitmaps(1);
        ProcessFlicQueue(gFrame_period);
        DoMouseCursor();
        PDScreenBufferSwap(0);
    }
    /* Optional animations can remain after all required ones have finished. */
    the_flic = gFirst_flic;
    while (the_flic != NULL) {
        EndFlic(the_flic);
        doomed_flic = the_flic;
        the_flic = the_flic->next;
        BrMemFree(doomed_flic);
    }
    gFirst_flic = NULL;
}

// FUNCTION: CARMA2_HW 0x00463340
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
void C2_HOOK_FASTCALL SuspendPendingFlic(void) {
    int pending = gPending_flic;
    gPending_flic = -1;
    gPending_pending_flic = pending;
}

// FUNCTION: CARMA2_HW 0x00463720
void C2_HOOK_FASTCALL ResumePendingFlic(void) {
    gPending_flic = gPending_pending_flic;
}

