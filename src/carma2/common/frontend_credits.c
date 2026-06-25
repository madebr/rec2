#include "frontend_credits.h"

#include "52-errors.h"
#include "frontend.h"
#include "frontend_main.h"
#include "globvars.h"
#include "intrface.h"
#include "loading.h"
#include "platform.h"
#include "polyfont.h"
#include "sound.h"
#include "utility.h"

#include "c2_stdlib.h"
#include "c2_string.h"


// GLOBAL: CARMA2_HW 0x005cab48
tFrontend_spec gFrontend_CREDITS = {
    "Credits",
    0,
    1,
    CreditsScreenInfunc,
    CreditsScreenOutfunc,
    NULL,
    &gFrontend_MAIN,
    0,
    0,
    0,
    11,
    0,
    0,
    {
        { 0x401, temp, &gFrontend_MAIN, 0, 17, 18, 0, 0, 0, 0, 1, 1 },
    },
};


// FUNCTION: CARMA2_HW 0x0046bd60
int C2_HOOK_FASTCALL CreditsScreenInfunc(tFrontend_spec* pFrontend) {
    tPath_name interface_path;
    tPath_name credits_path;
    FILE* f;
    int i;

    if (!pFrontend->loaded) {
        LoadMenuSettings(pFrontend);
    }
    FuckWithWidths(pFrontend);
    PathCat(interface_path, gApplication_path, "INTRFACE");
    PathCat(credits_path, interface_path, "creditslist.txt");
    f = DRfopen(credits_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_Mysterious_SS, interface_path, interface_path);
    }

    /* Number of lines */
    gCredits_line_count = GetAnInt(f);

    gCredits_texts = malloc(sizeof(char*) * gCredits_line_count);
    gCredits_fonts = malloc(sizeof(int) * gCredits_line_count);
    gCredits_heights = malloc(sizeof(int) * gCredits_line_count);
    gCredits_throbs = malloc(sizeof(int) * gCredits_line_count);

    gCredits_throbs[0] = 0;
    gCredits_fonts[0] = kPolyfont_hand_red_15pt_lit;
    gCredits_heights[0] = 0;
    gCredits_total_height = 0;
    for (i = 0; i < gCredits_line_count; i++) {
        char s[256];
        char small_buf[11];

        GetALineAndDontArgue(f, s);
        if (strncmp(s, "FONT", 4) == 0) {
            strncpy(small_buf, &s[5], sizeof(small_buf) - 1);
#ifdef REC2_FIX_BUGS
            small_buf[sizeof(small_buf) - 1] = '\0';
#endif
            gCredits_fonts[i] = atoi(small_buf);
        } else if (strncmp(s, "SPACE", 5) == 0) {
            double space_height;
            strncpy(small_buf, &s[6], sizeof(small_buf) - 1);
#ifdef REC2_FIX_BUGS
            small_buf[sizeof(small_buf) - 1] = '\0';
#endif
            space_height = (double)PolyFontHeight(gCredits_fonts[i]) * atof(small_buf);
            gCredits_heights[i] += (int)space_height;
        } else if (strncmp(s, "THROB", 5) == 0) {
            strncpy(small_buf, &s[6], sizeof(small_buf) - 1);
#ifdef REC2_FIX_BUGS
            small_buf[sizeof(small_buf) - 1] = '\0';
#endif
            if (strncmp(small_buf, "ON", 2) == 0) {
                gCredits_throbs[i] = 1;
            } else {
                gCredits_throbs[i] = 0;
            }
        } else {
            gCredits_texts[i] = malloc(strlen(s) + 1);
            strcpy(gCredits_texts[i], s);
            gCredits_heights[i] += PolyFontHeight(gCredits_fonts[i]);
            gCredits_total_height += gCredits_heights[i];
        }
        if (i + 1 < gCredits_line_count) {
            gCredits_fonts[i + 1] = gCredits_fonts[i];
            gCredits_throbs[i + 1] = gCredits_throbs[i];
            gCredits_heights[i + 1] = 0;
        }
    }
#ifdef REC2_FIX_BUGS
    PFfclose(f);
#endif
    gCredits_total_height += 480;
    gCredits_scroll_start = PDGetTotalTime();
    StartMusicTrack(9998);
    gFrontend_selected_item_index = 0;
    return 1;
}

// FUNCTION: CARMA2_HW 0x0046c090
int C2_HOOK_FASTCALL CreditsScreenOutfunc(tFrontend_spec* pFrontend) {
    int i;

    for (i = 0; i < gCredits_line_count; i++) {
        free(gCredits_texts[i]);
    }
    free(gCredits_texts);
    return 1;
}
