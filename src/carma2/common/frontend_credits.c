#include "frontend_credits.h"

#include "errors.h"
#include "frontend.h"
#include "globvars.h"
#include "intrface.h"
#include "loading.h"
#include "platform.h"
#include "polyfont.h"
#include "sound.h"
#include "utility.h"

#include "c2_stdlib.h"
#include "c2_string.h"

C2_HOOK_VARIABLE_IMPLEMENT_INIT(tFrontend_spec, gFrontend_CREDITS, 0x005cab48, {
    FIXME TODO
});


int C2_HOOK_FASTCALL CreditsScreenInfunc(tFrontend_spec* pFrontend) {
    tPath_name interface_path;
    tPath_name credits_path;
    FILE* f;
    int i;

    if (!pFrontend->loaded) {
        LoadMenuSettings(pFrontend);
    }
    FuckWithWidths(pFrontend);
    PathCat(interface_path, C2V(gApplication_path), "INTRFACE");
    PathCat(credits_path, interface_path, "creditslist.txt");
    f = DRfopen(credits_path, "rt");
    if (f == NULL) {
        FatalError(kFatalError_Mysterious_SS, interface_path, interface_path);
    }

    /* Number of lines */
    C2V(gAuthor_credits_line_count) = GetAnInt(f);

    C2V(gAuthor_credits_texts) = c2_malloc(sizeof(char*) * C2V(gAuthor_credits_line_count));
    C2V(gAuthor_credits_fonts) = c2_malloc(sizeof(int) * C2V(gAuthor_credits_line_count));
    C2V(gAuthor_credits_heights) = c2_malloc(sizeof(int) * C2V(gAuthor_credits_line_count));
    C2V(gAuthor_credits_throbs) = c2_malloc(sizeof(int) * C2V(gAuthor_credits_line_count));

    C2V(gAuthor_credits_throbs)[0] = 0;
    C2V(gAuthor_credits_fonts)[0] = kPolyfont_hand_red_15pt_lit;
    C2V(gAuthor_credits_heights)[0] = 0;
    C2V(gAuthor_credits_total_height) = 0;
    for (i = 0; i < C2V(gAuthor_credits_line_count); i++) {
        char s[256];
        char small_buf[11];

        GetALineAndDontArgue(f, s);
        if (c2_strncmp(s, "FONT", 4) == 0) {
            c2_strncpy(small_buf, &s[5], sizeof(small_buf) - 1);
            C2V(gAuthor_credits_fonts)[i] = c2_atoi(small_buf);
        } else if (c2_strncmp(s, "SPACE", 5) == 0) {
            double space_height;
            c2_strncpy(small_buf, &s[6], sizeof(small_buf) - 1);
            space_height = (double)PolyFontHeight(C2V(gAuthor_credits_fonts)[i]) * c2_atof(small_buf);
            C2V(gAuthor_credits_heights)[i] += (int)space_height;
        } else if (c2_strncmp(s, "THROB", 5) == 0) {
            c2_strncpy(small_buf, &s[6], sizeof(small_buf) - 1);
            if (c2_strncmp(small_buf, "ON", 2) == 0) {
                C2V(gAuthor_credits_throbs)[i] = 1;
            } else {
                C2V(gAuthor_credits_throbs)[i] = 0;
            }
        } else {
            C2V(gAuthor_credits_texts)[i] = c2_malloc(c2_strlen(s) + 1);
            c2_strcpy(C2V(gAuthor_credits_texts)[i], s);
            C2V(gAuthor_credits_heights)[i] += PolyFontHeight(C2V(gAuthor_credits_fonts)[i]);
            C2V(gAuthor_credits_total_height) += C2V(gAuthor_credits_heights)[i];
        }
        if (i + 1 < C2V(gAuthor_credits_line_count)) {
            C2V(gAuthor_credits_fonts)[i + 1] = C2V(gAuthor_credits_fonts)[i];
            C2V(gAuthor_credits_throbs)[i + 1] = C2V(gAuthor_credits_throbs)[i];
            C2V(gAuthor_credits_heights)[i + 1] = 0;
        }
    }
#ifdef REC2_FIX_BUGS
    PFfclose(f);
#endif
    C2V(gAuthor_credits_total_height) += 480;
    C2V(gAuthor_credits_scroll_start_time) = PDGetTotalTime();
    StartMusicTrack(9998);
    C2V(gFrontend_selected_item_index) = 0;
    return 1;
}
C2_HOOK_FUNCTION(0x0046bd60, CreditsScreenInfunc)

int C2_HOOK_FASTCALL CreditsScreenOutfunc(tFrontend_spec* pFrontend) {
    int i;

    for (i = 0; i < C2V(gAuthor_credits_line_count); i++) {
        c2_free(C2V(gAuthor_credits_texts)[i]);
    }
    c2_free(C2V(gAuthor_credits_texts));
    return 1;
}
C2_HOOK_FUNCTION(0x0046c090, CreditsScreenOutfunc)
