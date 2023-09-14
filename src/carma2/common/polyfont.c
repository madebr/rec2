#include "polyfont.h"

#include "drmem.h"
#include "errors.h"
#include "font.h"
#include "globvars.h"
#include "loading.h"
#include "utility.h"

#include "rec2_types.h"
#include "brender/br_types.h"

#include "c2_string.h"

#include "rec2_macros.h"

C2_HOOK_VARIABLE_IMPLEMENT(int, gInitial_count_font_texture_pages, 0x00765ea0);
C2_HOOK_VARIABLE_IMPLEMENT(int, gSize_font_texture_pages, 0x0068648c);
C2_HOOK_VARIABLE_IMPLEMENT(br_actor*, gString_root_actor, 0x0074cf10);
C2_HOOK_VARIABLE_IMPLEMENT_ARRAY(br_actor*, gPolyFont_glyph_actors, 256, 0x0074cae0);

void (C2_HOOK_FASTCALL * LoadPolyFont_original)(const char* pName, int pSize, int pIndex);
void C2_HOOK_FASTCALL LoadPolyFont(const char* pName, int pSize, int pIndex) {

#if defined(C2_HOOKS_ENABLED)
    LoadPolyFont_original(pName, pSize, pIndex);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004643f0, LoadPolyFont, LoadPolyFont_original)

void (C2_HOOK_FASTCALL * InitCarIcons_original)(br_pixelmap* pMap);
void C2_HOOK_FASTCALL InitCarIcons(br_pixelmap* pMap) {

#if defined(C2_HOOKS_ENABLED)
    InitCarIcons_original(pMap);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004973b0, InitCarIcons, InitCarIcons_original)

void (C2_HOOK_FASTCALL * InitPolyFonts_original)(void);
void C2_HOOK_FASTCALL InitPolyFonts(void) {
#if 0//defined(C2_HOOKS_ENABLED)
    InitPolyFonts_original();
#else
    int i;
    tPath_name the_path;
    FILE* f;

    C2_HOOK_BUG_ON(REC2_ASIZE(C2V(gPolyFontMaterials)) != 80);

    PrintMemoryDump(0, "START OF InitPolyFonts()");

    for (i = 0; i < REC2_ASIZE(C2V(gPolyFontMaterials)); i++) {

        C2V(gPolyFontMaterials)[i] = CreateFontCharacterMaterial(0);
    }
    C2_HOOK_BUG_ON(sizeof(C2V(gPolyFonts)) != 196344);
    c2_memset(&C2V(gPolyFonts), 0, sizeof(C2V(gPolyFonts)));
    C2V(gString_root_actor) = BrActorAllocate(BR_ACTOR_NONE, NULL);
    if (C2V(gString_root_actor) == NULL) {
        FatalError(kFatalError_OOM_S, "");
    }
    C2V(gString_root_actor)->identifier = "gString_root_actor";
    C2V(gString_root_actor)->t.type = BR_TRANSFORM_TRANSLATION;
    BrVector3Set(&C2V(gString_root_actor)->t.t.translate.t, 0.f, 0.f, 0.f);

    C2_HOOK_BUG_ON(REC2_ASIZE(C2V(gPolyFont_glyph_actors)) != 256);
    for (i = 0; i < REC2_ASIZE(C2V(gPolyFont_glyph_actors)); i++) {
        br_actor* actor;

        actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        C2V(gPolyFont_glyph_actors)[i] = actor;
        if (actor == NULL) {
            FatalError(kFatalError_OOM_S, "");
        }
        actor->t.type = BR_TRANSFORM_TRANSLATION;
        BrVector3Set(&actor->t.t.translate.t, 0.f, 0.f, 0.f);
        actor->identifier = "character_actor";
    }

    PathCat(the_path, C2V(gApplication_path), "FontCol.TXT");
    f = DRfopen(the_path, "rb");
    if (f == NULL) {
        BrFailure("Missing FONTCOL.TXT file");
    }
    /**
     * COLOURS FOR POLYGONAL FONTS
     *
     * Each font has four sets of RGB values in the range 0-255. These specify
     * the colour for each corner of every character in that font.
     */

    C2_HOOK_BUG_ON(REC2_ASIZE(C2V(gPolyFontBorderColours)) != 27);
    for (i = 0; i < REC2_ASIZE(C2V(gPolyFontBorderColours)); i++) {
        tPolyFontBorderColours *border;

        border = &C2V(gPolyFontBorderColours)[i];

        /* Top/Left */
        GetThreeInts(f, &border->tl.r, &border->tl.g, &border->tl.b);
        /* Top/Right */
        GetThreeInts(f, &border->tr.r, &border->tr.g, &border->tr.b);
        /* Bottom/Left */
        GetThreeInts(f, &border->bl.r, &border->bl.g, &border->bl.b);
        /* Bottom/Right */
        GetThreeInts(f, &border->br.r, &border->br.g, &border->br.b);
    }
#if defined(REC2_FIX_BUGS)
    DRfclose(f);
#else
    fclose(f);
#endif
    InitCarIcons(C2V(gIcons_pix));

    LoadPolyFont("TINYFONT", 8, kPolyfont_ingame_tiny_yellow);
    LoadPolyFont("TINYFONT", 8, kPolyfont_ingame_tiny_blue);
    LoadPolyFont("TINYFONT", 8, kPolyfont_ingame_tiny_red);
    LoadPolyFont("TINYFONT", 8, kPolyfont_ingame_tiny_green);

    LoadPolyFont("BIGYELLOWTIMER", 32, kPolyfont_ingame_big_timer);
    for (i = 0; i < REC2_ASIZE(C2V(gPolyFonts)->glyphs); i++) {
        C2V(gPolyFonts)[kPolyfont_ingame_big_timer].glyphs[i].glyph_width = 27;
    }
    C2V(gPolyFonts)[kPolyfont_ingame_big_timer].glyphs[58].glyph_width = 11;

    LoadPolyFont("MEDIUM_HEADUP", 16, kPolyfont_ingame_medium_red);
    LoadPolyFont("MEDIUM_HEADUP", 16, kPolyfont_ingame_medium_blue);
    LoadPolyFont("MEDIUM_HEADUP", 16, kPolyfont_ingame_medium_orange);
    LoadPolyFont("MEDIUM_HEADUP", 16, kPolyfont_ingame_medium_green);

    LoadPolyFont("BIG_ITALIC", 32, kPolyfont_ingame_italic_yellow);
    LoadPolyFont("BIG_ITALIC", 32, kPolyfont_ingame_italic_green);
    LoadPolyFont("BIG_ITALIC", 32, kPolyfont_ingame_italic_blue);
    LoadPolyFont("BIG_ITALIC", 32, kPolyfont_ingame_italic_red);

    LoadPolyFont("NETPOSFONT", 16, kPolyfont_netpos);
    C2V(gInitial_count_font_texture_pages) = C2V(gSize_font_texture_pages);
    PrintMemoryDump(0, "END OF InitPolyFonts()");
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00463d40, InitPolyFonts, InitPolyFonts_original)
