#include "displays.h"

#include "font.h"
#include "globvars.h"
#include "graphics.h"
#include "polyfont.h"
#include "utility.h"

#include "platform.h"

#include "c2_string.h"

#include "rec2_macros.h"

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tHeadup, gHeadups, 36, 0x0067c500);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(tQueued_headup, gQueued_headups, 4, 0x0067f890);
C2_HOOK_VARIABLE_IMPLEMENT(int, gQueued_headup_count, 0x0067f888);
C2_HOOK_VARIABLE_IMPLEMENT(tU32, gLast_centre_headup, 0x0067fcf8);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(int, gDRFont_to_polyfont_mapping, 24, 0x0059ada0, {
    kPolyfont_ingame_tiny_red,
    kPolyfont_ingame_tiny_yellow,
    kPolyfont_ingame_tiny_blue,
    kPolyfont_ingame_tiny_green,
    kPolyfont_ingame_italic_yellow,
    kPolyfont_ingame_big_timer,
    kPolyfont_ingame_medium_blue,
    kPolyfont_ingame_medium_red,
    kPolyfont_ingame_medium_orange,
    kPolyfont_ingame_tiny_green,
    kPolyfont_ingame_medium_green,
    kPolyfont_ingame_tiny_blue,
    kPolyfont_ingame_tiny_red,
    kPolyfont_ingame_tiny_yellow,
    kPolyfont_ingame_tiny_blue,
    kPolyfont_ingame_tiny_blue,
    kPolyfont_ingame_tiny_blue,
    kPolyfont_ingame_italic_red,
    kPolyfont_ingame_italic_red,
    kPolyfont_ingame_italic_green,
    kPolyfont_ingame_italic_green,
    kPolyfont_ingame_italic_red,
    kPolyfont_ingame_italic_red,
    kPolyfont_ingame_medium_blue,
});

int (C2_HOOK_FASTCALL * DRTextWidth_original)(const tDR_font* pFont, const char* pText);
int C2_HOOK_FASTCALL DRTextWidth(const tDR_font* pFont, const char* pText) {

#if 0//defined(C2_HOOKS_ENABLED)
    return DRTextWidth_original(pFont, pText);
#else

    return GetPolyFontTextWidth(C2V(gDRFont_to_polyfont_mapping)[pFont->id], pText);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00465d50, DRTextWidth, DRTextWidth_original)

int C2_HOOK_FASTCALL DRTextCleverWidth(const tDR_font* pFont, const char* pText) {
    int polyfont;
    int i;
    int len;
    int result;

    polyfont = C2V(gDRFont_to_polyfont_mapping)[pFont->id];
    result = 0;
    len = c2_strlen(pText);

    for (i = 0; i < len; i++) {

        if ((tS8)pText[i] < 0) {
            polyfont = C2V(gDRFont_to_polyfont_mapping)[C2V(gFonts)[-(tS8) pText[i]].id];
        } else {
            int inter = 0;
            if (i < len - 1) {
                inter = C2V(gPolyFonts)[polyfont].interCharacterSpacing;
            }

            result += GetPolyFontCharacterWidthI(pText[i], polyfont) + inter;
        }
    }
    return result;
}
C2_HOOK_FUNCTION(0x00465e10, DRTextCleverWidth)

void C2_HOOK_FASTCALL DRPixelmapCentredText(br_pixelmap* pPixelmap, int pX, int pY, const tDR_font* pFont, const char* pText) {
    int width_over_2;

    width_over_2 = DRTextWidth(pFont, pText) / 2;
    TransDRPixelmapText(pPixelmap, pX - width_over_2, pY, pFont, pText, width_over_2 + pX);
}
C2_HOOK_FUNCTION(0x00465f10, DRPixelmapCentredText)

void C2_HOOK_FASTCALL OoerrIveGotTextInMeBoxMissus(int pFont_index, const char* pText, br_pixelmap* pPixelmap, int pLeft, int pTop, int pRight, int pBottom, int pCentred) {
    int polyfont;

    polyfont = C2V(gDRFont_to_polyfont_mapping)[C2V(gFonts)[pFont_index].id];
    RenderPolyText(polyfont, pText, pLeft, pTop, pRight, pBottom, pCentred ? eJust_centre : eJust_left, C2V(gRender_poly_text));
}
C2_HOOK_FUNCTION(0x00466000, OoerrIveGotTextInMeBoxMissus)

void (C2_HOOK_FASTCALL *TransDRPixelmapText_original)(br_pixelmap* pPixelmap, int pX, int pY, const tDR_font* pFont, const char* pText, int pRight_edge);
void C2_HOOK_FASTCALL TransDRPixelmapText(br_pixelmap* pPixelmap, int pX, int pY, const tDR_font* pFont, const char* pText, int pRight_edge) {

#if 0//defined(C2_HOOKS_ENABLED)
    TransDRPixelmapText_original(pPixelmap, pX, pY, pFont, pText, pRight_edge);
#else

    RenderPolyTextLine(pText, pX, pY, C2V(gDRFont_to_polyfont_mapping)[pFont->id], eJust_left, C2V(gRender_poly_text));
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00465a70, TransDRPixelmapText, TransDRPixelmapText_original)

void (C2_HOOK_FASTCALL * InitHeadups_original)(void);
void C2_HOOK_FASTCALL InitHeadups(void) {

#if defined(C2_HOOKS_ENABLED)
    InitHeadups_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00449090, InitHeadups, InitHeadups_original)

int (C2_HOOK_FASTCALL * MungeHeadupWidth_original)(tHeadup* pHeadup);
int C2_HOOK_FASTCALL MungeHeadupWidth(tHeadup* pHeadup) {

    C2_HOOK_BUG_ON(sizeof(tHeadup) != 356);

#if defined(C2_HOOKS_ENABLED)
    return MungeHeadupWidth_original(pHeadup);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0044a220, MungeHeadupWidth, MungeHeadupWidth_original)

int C2_HOOK_FASTCALL FindAHeadupHoleWoofBarkSoundsABitRude(int pSlot_index) {
    int i;
    int empty_one;
    tHeadup* the_headup;

    empty_one = -1;
    for (i = 0; i < REC2_ASIZE(C2V(gHeadups)); i++) {
        the_headup = &C2V(gHeadups)[i];
        if (pSlot_index >= 0 && the_headup->slot_index == pSlot_index) {
            return i;
        }
        if (the_headup->type == eHeadup_unused) {
            empty_one = i;
        }
    }
    return empty_one;
}
C2_HOOK_FUNCTION(0x00449fa0, FindAHeadupHoleWoofBarkSoundsABitRude)

void C2_HOOK_FASTCALL KillOldestQueuedHeadup(void) {

    C2V(gQueued_headup_count)--;
    c2_memmove(&C2V(gQueued_headups)[0], &C2V(gQueued_headups)[1], C2V(gQueued_headup_count) * sizeof(tQueued_headup));
}

void C2_HOOK_FASTCALL ClearQueuedHeadups(void) {

    while (C2V(gQueued_headup_count) != 0) {
        KillOldestQueuedHeadup();
    }
}
C2_HOOK_FUNCTION(0x004497b0, ClearQueuedHeadups)

int C2_HOOK_FASTCALL NewTextHeadupSlot2(int pSlot_index, int pFlash_rate, int pLifetime, int pFont_index, char* pText, int pQueue_it) {
    int index;
    tHeadup* the_headup;
    tHeadup_slot* headup_slot;
    tU32 time;

    time = PDGetTotalTime();
    if (pQueue_it && pSlot_index == 4 && time - C2V(gLast_centre_headup) < 1000) {
        if (C2V(gQueued_headup_count) == 4) {
            KillOldestQueuedHeadup();
        }
        C2V(gQueued_headups)[C2V(gQueued_headup_count)].flash_rate = pFlash_rate;
        C2V(gQueued_headups)[C2V(gQueued_headup_count)].lifetime = pLifetime;
        C2V(gQueued_headups)[C2V(gQueued_headup_count)].font_index = pFont_index;
        c2_strcpy(C2V(gQueued_headups)[C2V(gQueued_headup_count)].text, pText);
        C2V(gQueued_headup_count)++;
        return -1;
    }
    index = FindAHeadupHoleWoofBarkSoundsABitRude(pSlot_index);
    if (index < 0) {
        return index;
    }
    if (pSlot_index == 4) {
        C2V(gLast_centre_headup) = time;
    }
    headup_slot = &C2V(gProgram_state).current_car.headup_slots[C2V(gProgram_state).cockpit_on][pSlot_index];
    the_headup = &C2V(gHeadups)[index];
    the_headup->data.coloured_text_info.coloured_font = &C2V(gFonts)[-pFont_index];
    if (pSlot_index == 4) {
        the_headup->type = eHeadup_box_text;
    } else {
        the_headup->type = eHeadup_coloured_text;
    }
    c2_strcpy(the_headup->data.coloured_text_info.text, pText);

    the_headup->slot_index = pSlot_index;
    the_headup->justification = headup_slot->justification;
    if (pSlot_index < 0) {
        the_headup->cockpit_anchored = 0;
    } else {
        the_headup->cockpit_anchored = headup_slot->cockpit_anchored;
    }
    the_headup->field_0x3c = headup_slot->field_0x28;
    the_headup->dimmed_background = headup_slot->dimmed_background;
    the_headup->dim_left = headup_slot->dim_left;
    the_headup->dim_top = headup_slot->dim_top;
    the_headup->dim_right = headup_slot->dim_right;
    the_headup->dim_bottom = headup_slot->dim_bottom;
    the_headup->original_x = headup_slot->x;
    the_headup->right_edge = MungeHeadupWidth(the_headup) + the_headup->x;
    the_headup->y = headup_slot->y;
    if (pFlash_rate) {
        the_headup->flash_period = 1000 / pFlash_rate;
    } else {
        the_headup->flash_period = 0;
    }
    the_headup->last_flash = 0;
    the_headup->flash_state = 0;
    if (pLifetime) {
        the_headup->end_time = GetTotalTime() + pLifetime;
    } else {
        the_headup->end_time = 0;
    }
    return index;
}
C2_HOOK_FUNCTION(0x00449fd0, NewTextHeadupSlot2)

int C2_HOOK_FASTCALL NewTextHeadupSlot(int pSlot_index, int pFlash_rate, int pLifetime, int pFont_index, char* pText) {

    return NewTextHeadupSlot2(pSlot_index, pFlash_rate, pLifetime, pFont_index, pText, 1);
}
C2_HOOK_FUNCTION(0x0044a380, NewTextHeadupSlot)

void C2_HOOK_FASTCALL TransDRPixelmapCleverText(br_pixelmap* pPixelmap, int pX, int pY, tDR_font* pFont, char* pText, int pRight_edge) {
    int i;
    char s[512];
    int s_end;
    int x;
    int next_x;
    int len;

    len = c2_strlen(pText);
    s_end = -1;
    x = pX;
    next_x = pX;
    for (i = 0; i < len; i++) {
        tS8 ch = pText[i];

        if (ch < 0) {

            if (s_end >= 0) {
                s[s_end + 1] = '\0';
                RenderPolyTextLine(s, x, pY, C2V(gDRFont_to_polyfont_mapping)[pFont->id], eJust_left, C2V(gRender_poly_text));
                s_end = -1;
                x = next_x;
            }
            /* Change font */
            pY -= (GetPolyFontHeight(C2V(gDRFont_to_polyfont_mapping)[C2V(gFonts)[-ch].id]) - GetPolyFontHeight(C2V(gDRFont_to_polyfont_mapping)[pFont->id])) / 2;
            pFont = &C2V(gFonts)[-ch];
        } else {
            s_end += 1;
            s[s_end] = pText[i];
            next_x += GetPolyFontCharacterWidthI(C2V(gDRFont_to_polyfont_mapping)[pFont->id], pText[i]);
        }
    }
    if (s_end >= 0) {
        s[s_end + 1] = '\0';
        RenderPolyTextLine(s, x, pY, C2V(gDRFont_to_polyfont_mapping)[pFont->id], eJust_left, C2V(gRender_poly_text));
    }
}
C2_HOOK_FUNCTION(0x00465aa0, TransDRPixelmapCleverText)
