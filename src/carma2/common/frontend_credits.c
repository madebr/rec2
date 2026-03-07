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
    gAuthor_credits_line_count = GetAnInt(f);

    gAuthor_credits_texts = malloc(sizeof(char*) * gAuthor_credits_line_count);
    gAuthor_credits_fonts = malloc(sizeof(int) * gAuthor_credits_line_count);
    gAuthor_credits_heights = malloc(sizeof(int) * gAuthor_credits_line_count);
    gAuthor_credits_throbs = malloc(sizeof(int) * gAuthor_credits_line_count);

    gAuthor_credits_throbs[0] = 0;
    gAuthor_credits_fonts[0] = kPolyfont_hand_red_15pt_lit;
    gAuthor_credits_heights[0] = 0;
    gAuthor_credits_total_height = 0;
    for (i = 0; i < gAuthor_credits_line_count; i++) {
        char s[256];
        char small_buf[11];

        GetALineAndDontArgue(f, s);
        if (strncmp(s, "FONT", 4) == 0) {
            strncpy(small_buf, &s[5], sizeof(small_buf) - 1);
#ifdef REC2_FIX_BUGS
            small_buf[sizeof(small_buf) - 1] = '\0';
#endif
            gAuthor_credits_fonts[i] = atoi(small_buf);
        } else if (strncmp(s, "SPACE", 5) == 0) {
            double space_height;
            strncpy(small_buf, &s[6], sizeof(small_buf) - 1);
#ifdef REC2_FIX_BUGS
            small_buf[sizeof(small_buf) - 1] = '\0';
#endif
            space_height = (double)PolyFontHeight(gAuthor_credits_fonts[i]) * atof(small_buf);
            gAuthor_credits_heights[i] += (int)space_height;
        } else if (strncmp(s, "THROB", 5) == 0) {
            strncpy(small_buf, &s[6], sizeof(small_buf) - 1);
#ifdef REC2_FIX_BUGS
            small_buf[sizeof(small_buf) - 1] = '\0';
#endif
            if (strncmp(small_buf, "ON", 2) == 0) {
                gAuthor_credits_throbs[i] = 1;
            } else {
                gAuthor_credits_throbs[i] = 0;
            }
        } else {
            gAuthor_credits_texts[i] = malloc(strlen(s) + 1);
            strcpy(gAuthor_credits_texts[i], s);
            gAuthor_credits_heights[i] += PolyFontHeight(gAuthor_credits_fonts[i]);
            gAuthor_credits_total_height += gAuthor_credits_heights[i];
        }
        if (i + 1 < gAuthor_credits_line_count) {
            gAuthor_credits_fonts[i + 1] = gAuthor_credits_fonts[i];
            gAuthor_credits_throbs[i + 1] = gAuthor_credits_throbs[i];
            gAuthor_credits_heights[i + 1] = 0;
        }
    }
#ifdef REC2_FIX_BUGS
    PFfclose(f);
#endif
    gAuthor_credits_total_height += 480;
    gAuthor_credits_scroll_start_time = PDGetTotalTime();
    StartMusicTrack(9998);
    gFrontend_selected_item_index = 0;
    return 1;
}

// FUNCTION: CARMA2_HW 0x0046c090
int C2_HOOK_FASTCALL CreditsScreenOutfunc(tFrontend_spec* pFrontend) {
    int i;

    for (i = 0; i < gAuthor_credits_line_count; i++) {
        free(gAuthor_credits_texts[i]);
    }
    free(gAuthor_credits_texts);
    return 1;
}
