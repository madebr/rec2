#include "38-flicplay.h"

#include "08-loading1.h"
#include "19-font.h"
#include "41-utility.h"
#include "52-errors.h"
#include "70-packfile.h"
#include "globvars.h"
#include "rec2_macros.h"

#include "c2_stdio.h"
#include "c2_string.h"

// GLOBAL: CARMA2_HW 0x005964c8
int gPending_flic = -1;

// GLOBAL: CARMA2_HW 0x0068620c
int gPending_pending_flic;

// GLOBAL: CARMA2_HW 0x00686314
int gPanel_flic_disable;

// GLOBAL: CARMA2_HW 0x00686318
int gPlay_from_disk;

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

// DontLetFlicFuckWithPalettes

// LetFlicFuckWithPalettes

// TurnFlicTransparencyOn

// TurnFlicTransparencyOff

// FUNCTION: CARMA2_HW 0x00461a00
void C2_HOOK_FASTCALL PlayFlicsFromDisk(void) {
    *(volatile int*)&gPlay_from_disk = 1;
}

// FUNCTION: CARMA2_HW 0x00461a10
void C2_HOOK_FASTCALL PlayFlicsFromMemory(void) {
    *(volatile int*)&gPlay_from_disk = 0;
}

// FUNCTION: CARMA2_HW 0x00461a20
void C2_HOOK_FASTCALL TurnOffPanelFlics(void) {
    *(volatile int*)&gPanel_flic_disable = 1;
}

// FUNCTION: CARMA2_HW 0x00461a30
void C2_HOOK_FASTCALL TurnOnPanelFlics(void) {
    *(volatile int*)&gPanel_flic_disable = 0;
}

// FUNCTION: CARMA2_HW 0x00461a60
void C2_HOOK_FASTCALL FlicPaletteAllocate(void) {

    gPalette_pixels = BrMemAllocate(0x400, kMem_misc);
    gPalette = DRPixelmapAllocate(BR_PMT_RGBX_888, 1, 256, gPalette_pixels, 0);
}

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

// STUB: CARMA2_HW 0x00462a70
void C2_HOOK_FASTCALL UnlockFlic(int pIndex) {
    NOT_IMPLEMENTED();
}

// LoadFlicData

// FUNCTION: CARMA2_HW 0x00462cc0
void C2_HOOK_FASTCALL InitFlicQueue(void) {

    gFirst_flic = NULL;
}

// FlicQueueFinished

// ProcessFlicQueue

// STUB: CARMA2_HW 0x00462dc0
void C2_HOOK_FASTCALL FlushFlicQueue(void) {
    NOT_IMPLEMENTED();
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
    int pending = *(volatile int*)&gPending_flic;
    *(volatile int*)&gPending_flic = -1;
    *(volatile int*)&gPending_pending_flic = pending;
}

// FUNCTION: CARMA2_HW 0x00463720
void C2_HOOK_FASTCALL ResumePendingFlic(void) {
    int pending = *(volatile int*)&gPending_pending_flic;
    *(volatile int*)&gPending_flic = pending;
}

