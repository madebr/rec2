#include "19-font.h"

#include "05-drmem.h"
#include "08-loading1.h"
#include "16-graphics1.h"
#include "18-graphics2.h"
#include "41-utility.h"
#include "52-errors.h"
#include "62-graphics3.h"
#include "70-packfile.h"
#include "globvars.h"
#include "platform.h"
#include "rec2_macros.h"
#include "rec2_types.h"

#include <string.h>

// GLOBAL: CARMA2_HW 0x0076c960
br_pixelmap* gTexture_maps[1024];

// GLOBAL: CARMA2_HW 0x007663e0
tDR_font gFonts[24];

// GLOBAL: CARMA2_HW 0x0059ad30
const char* gFont_names[24] = {
    "TYPEABLE",
    "ORANGHED",
    "BLUEHEAD",
    "GREENHED",
    "MEDIUMHD",
    "TIMER",
    "NEWHITE",
    "NEWRED",
    "NEWBIGGR",
    "GRNDK",
    "GRNLIT",
    "GRYDK",
    "GRYLIT",
    "BUTTIN",
    "BUTTOUT",
    "LITPLAQ",
    "DRKPLAQ",
    "RED1",
    "RED1GLOW",
    "GRN1",
    "GRN1GLOW",
    "RED2",
    "RED2GLOW",
    "OPPOSTAT",
};

// GLOBAL: CARMA2_HW 0x00764f00
br_pixelmap* gPixelmap_buffer[1000];

// GLOBAL: CARMA2_HW 0x007663d0
size_t gPixelmap_buffer_size;

// GLOBAL: CARMA2_HW 0x0076d960
tPolyFont gPoly_fonts[27];

// GLOBAL: CARMA2_HW 0x0068648c
int gSize_font_texture_pages;

// GLOBAL: CARMA2_HW 0x00765ec0
tPolyFontBorderColours gPoly_font_border_colours[27];

// GLOBAL: CARMA2_HW 0x00686488
int gInterface_fonts_loaded;

// GLOBAL: CARMA2_HW 0x00765ea0
int gInitial_count_font_texture_pages;

// GLOBAL: CARMA2_HW 0x00686494
int gInterface_polyfont_texture_pages;

// GLOBAL: CARMA2_HW 0x00686340
br_material* gPoly_font_materials[80];

// GLOBAL: CARMA2_HW 0x0074cf10
br_actor* gString_root_actor;

// GLOBAL: CARMA2_HW 0x0074cae0
br_actor* gPolyfont_glyph_actors[256];

// GLOBAL: CARMA2_HW 0x00686490
int gCount_polyfont_glyph_actors;

// GLOBAL: CARMA2_HW 0x0059ada0
int gDRFont_to_polyfont_mapping[24] = {
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
};

// GLOBAL: CARMA2_HW 0x0059ad28
int gRender_poly_text = 1;

#define POLYFONT_FONT_CHARACTER(FONT, CHARACTER) ((CHARACTER) + ((FONT) << 8))
#define POLYFONT_PACK_USER(FONT_CHARACTER, GENERATION) ((void*)(uintptr_t)((FONT_CHARACTER) + ((GENERATION) << 16)))
#define POLYFONT_MATERIAL_GET_FONT_CHARACTER(MATERIAL) (((uintptr_t)(MATERIAL)->user) & 0xffff)
#define POLYFONT_MATERIAL_GET_GENERATION(MATERIAL) ((((uintptr_t)(MATERIAL)->user) >> 16) & 0xffff)

// FUNCTION: CARMA2_HW 0x00463730
int C2_HOOK_FASTCALL PolyFontHeight(int pFont_index) {

    CheckAvailabilityOfThisFont(pFont_index);
    return gPoly_fonts[pFont_index].fontCharacterHeight;
}

// FUNCTION: CARMA2_HW 0x00463760
int C2_HOOK_FASTCALL FindCharacterWidth(br_pixelmap* pMap) {
    int y;
    int x;
    tU16* pixel;
    int height;

    height = pMap->height;
    x = pMap->width - 1;
    for (; x >= 0; x--) {
        pixel = (tU16*)pMap->pixels + x;
        for (y = 0; y < height; y++) {
            if (*(tU16*)pixel != 0) {
                return x + 1;
            }
            pixel =pixel + pMap->row_bytes / 2;
        }
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x004637d0
int C2_HOOK_FASTCALL CharacterWidth(int pFont_index, tU8 pCharacter) {
    tPolyFont* font;

    if ('a' <= pCharacter && pCharacter <= 'z') {
        pCharacter = pCharacter - 'a' + 'A';
    }

    font = &gPoly_fonts[pFont_index];
    if (gPoly_fonts[pFont_index].glyphs[pCharacter].used) {
        return font->glyphs[pCharacter].glyph_width;
    }
    return font->widthOfBlank;
}

// FUNCTION: CARMA2_HW 0x00463830
int C2_HOOK_FASTCALL GetSpacing(int pFont_index) {
    tPolyFont* font;

    font = &gPoly_fonts[pFont_index];
    return font->interCharacterSpacing;
}

// FUNCTION: CARMA2_HW 0x00463850
void C2_HOOK_FASTCALL SolidPolyFontTextInABox(int pFont, const char* pText, int pX, int pY, int pWidth, int pHeight, tJustification pJustification, undefined4 pParam_8) {

    CheckAvailabilityOfThisFont(pFont);
    TransparentPolyFontTextInABox(pFont, pText, pX, pY, pWidth, pHeight, pJustification, pParam_8, 1.0);
}

// PolyFontTextInABox

// FUNCTION: CARMA2_HW 0x00463ad0
void C2_HOOK_FASTCALL TransparentPolyFontTextInABox(int pFont, const char* pText, int pLeft, int pTop, int pRight, int pBottom, tJustification pJust, undefined4 pUnknown, double pBlend) {
    char s[256];
    int s_len;
    int y;
    int in_start_put;
    int in_end_put;
    int str_i;
    int newline;
    int text_x;

    in_start_put = 0;
    text_x = 0;
    if (pText != NULL) {
        str_i = 0;
        s_len = 0;
        s[s_len] = '\0';
        y = pTop;

        while (pText[str_i] != '\0') {
            int chr;

            chr = pText[str_i];
            newline = chr == '\r';
            s[s_len] = chr;
            if (newline) {
                s[s_len] = ' ';
            }
            s[s_len + 1] = '\0';
            if (!newline) {
                text_x += CharacterWidth(pFont, (tU8)chr);
            }
            in_end_put = str_i;
            if (text_x <= pRight - pLeft && !newline) {
                s_len++;
                str_i++;
            } else {
                int len_put;

                /* Avoid breaking words in parts ... */
                while (in_end_put > in_start_put && pText[in_end_put] != ' ') {
                    in_end_put -= 1;
                }
                /* ... but break when the word is too long */
                if (in_end_put == in_start_put) {
                    in_end_put = str_i;
                }
                len_put = in_end_put - in_start_put;
                s[len_put + 1] = '\0';
                if (pText[in_end_put] == ' ') {
                    in_end_put += 1;
                }
                s[len_put] = '\0';
                if (pJust == eJust_centre) {
                    TransparentPolyFontText(s, (pRight + pLeft) / 2, y, pFont, pJust, gRender_poly_text, pBlend);
                } else {
                    TransparentPolyFontText(s, pLeft, y, pFont, pJust, gRender_poly_text, pBlend);
                }
                while (pText[in_end_put] == '\r' || pText[in_end_put] == ' ') {
                    in_end_put++;
                }
                y += (2 * PolyFontHeight(pFont)) / 2;
                text_x = 0;
                in_start_put = in_end_put;
                s_len = 0;
                str_i = in_start_put;
            }
        }
        if (s_len != 0) {

            if (pJust == eJust_centre) {
                TransparentPolyFontText(s, (pRight + pLeft) / 2, y, pFont, pJust, gRender_poly_text, pBlend);
            } else {
                TransparentPolyFontText(s, pLeft, y, pFont, pJust, gRender_poly_text, pBlend);
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x00463d40
void C2_HOOK_FASTCALL InitPolyFonts(void) {
    int i;
    tPath_name the_path;
    FILE* f;
    br_actor* actor;
    int red, green, blue;

    C2_HOOK_BUG_ON(REC2_ASIZE(gPoly_font_materials) != 80);

    PrintMemoryDump(0, "START OF InitPolyFonts()");

    InitPolyFontMaterials();
    C2_HOOK_BUG_ON(sizeof(gPoly_fonts) != 196344);
    memset(&gPoly_fonts, 0, sizeof(gPoly_fonts));
    gString_root_actor = BrActorAllocate(BR_ACTOR_NONE, NULL);
    if (gString_root_actor == NULL) {
#ifdef REC2_FIX_BUGS
        FatalError(kFatalError_OOM_S, "");
#else
        FatalError(kFatalError_OOM_S);
#endif
    }
    gString_root_actor->identifier = "gString_root_actor";
    gString_root_actor->t.type = BR_TRANSFORM_TRANSLATION;
    BrVector3Set(&gString_root_actor->t.t.translate.t, 0.0f, 0.0f, 0.0f);

    C2_HOOK_BUG_ON(REC2_ASIZE(gPolyfont_glyph_actors) != 256);
    for (i = 0; i < (int)REC2_ASIZE(gPolyfont_glyph_actors); i++) {

        actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        gPolyfont_glyph_actors[i] = actor;
        if (actor == NULL) {
#ifdef REC2_FIX_BUGS
            FatalError(kFatalError_OOM_S, "");
#else
            FatalError(kFatalError_OOM_S);
#endif
        }
        actor->t.type = BR_TRANSFORM_TRANSLATION;
        actor->identifier = "character_actor";
        BrVector3Set(&actor->t.t.translate.t, 0.0f, 0.0f, 0.0f);
    }

    PathCat(the_path, gApplication_path, "FontCol.TXT");
    f = DRfopen(the_path, "rb");
    if (f != NULL) {
        /**
         * COLOURS FOR POLYGONAL FONTS
         *
         * Each font has four sets of RGB values in the range 0-255. These specify
         * the colour for each corner of every character in that font.
         */

        C2_HOOK_BUG_ON(REC2_ASIZE(gPoly_font_border_colours) != 27);
        for (i = 0; i < (int)REC2_ASIZE(gPoly_font_border_colours); i++) {

            /* Top/Left */
            GetThreeInts(f, &red, &green, &blue);
            gPoly_font_border_colours[i].tl.r = red;
            gPoly_font_border_colours[i].tl.g = green;
            gPoly_font_border_colours[i].tl.b = blue;

            /* Top/Right */
            GetThreeInts(f, &red, &green, &blue);
            gPoly_font_border_colours[i].tr.r = red;
            gPoly_font_border_colours[i].tr.g = green;
            gPoly_font_border_colours[i].tr.b = blue;

            /* Bottom/Left */
            GetThreeInts(f, &red, &green, &blue);
            gPoly_font_border_colours[i].bl.r = red;
            gPoly_font_border_colours[i].bl.g = green;
            gPoly_font_border_colours[i].bl.b = blue;

            /* Bottom/Right */
            GetThreeInts(f, &red, &green, &blue);
            gPoly_font_border_colours[i].br.r = red;
            gPoly_font_border_colours[i].br.g = green;
            gPoly_font_border_colours[i].br.b = blue;
        }
        fclose(f);
    } else {
        BrFailure("Missing FONTCOL.TXT file");
    }
    ConvertCarIcons(gIcons_pix);

    CreatePolyFont(kPolyfont_ingame_tiny_yellow, "TINYFONT", 1.0f, 8);
    CreatePolyFont(kPolyfont_ingame_tiny_blue, "TINYFONT", 1.0f, 8);
    CreatePolyFont(kPolyfont_ingame_tiny_red, "TINYFONT", 1.0f, 8);
    CreatePolyFont(kPolyfont_ingame_tiny_green, "TINYFONT", 1.0f, 8);
    CreatePolyFont(kPolyfont_ingame_big_timer, "BIGYELLOWTIMER", 1.0f, 32);
    CreatePolyFont(kPolyfont_ingame_medium_red, "MEDIUM_HEADUP", 1.0f, 16);
    CreatePolyFont(kPolyfont_ingame_medium_blue, "MEDIUM_HEADUP", 1.0f, 16);
    CreatePolyFont(kPolyfont_ingame_medium_orange, "MEDIUM_HEADUP", 1.0f, 16);
    CreatePolyFont(kPolyfont_ingame_medium_green, "MEDIUM_HEADUP", 1.0f, 16);
    CreatePolyFont(kPolyfont_ingame_italic_yellow, "BIG_ITALIC", 1.0f, 32);
    CreatePolyFont(kPolyfont_ingame_italic_green, "BIG_ITALIC", 1.0f, 32);
    CreatePolyFont(kPolyfont_ingame_italic_blue, "BIG_ITALIC", 1.0f, 32);
    CreatePolyFont(kPolyfont_ingame_italic_red, "BIG_ITALIC", 1.0f, 32);
    CreatePolyFont(kPolyfont_netpos, "NETPOSFONT", 1.0f, 16);

    gInitial_count_font_texture_pages = gSize_font_texture_pages;
    PrintMemoryDump(0, "END OF InitPolyFonts()");
}

// FUNCTION: CARMA2_HW 0x00464090
void C2_HOOK_FASTCALL CreatePolyFont(int pFont, const char* pName, float pFactor, int pSize) {
    int i;

    LoadPolyFont(pName, pSize, pFont);

    if (pFont == kPolyfont_ingame_big_timer) {
        for (i = 0; i < (int)REC2_ASIZE(gPoly_fonts[kPolyfont_ingame_big_timer].glyphs); i++) {
            gPoly_fonts[kPolyfont_ingame_big_timer].glyphs[i].glyph_width = 27;
        }
        gPoly_fonts[kPolyfont_ingame_big_timer].glyphs[58].glyph_width = 11; /* FIXME: What is gyph 58? */
    }
}

br_model* C2_HOOK_FASTCALL CreateFontCharacterModel(int pIndex, int pAscii, int pWidth, int pHeight, const char* pName) {
    br_model* model;

    model = gPoly_fonts[pIndex].model;
    while (model != NULL) {
        if (model->bounds.max.v[0] == pWidth && model->bounds.min.v[1] == -pHeight) {
            return model;
        }
        model = model->user;
    }
    model = BrModelAllocate("String Model", 4, 2);
    model->user = gPoly_fonts[pIndex].glyphs[pIndex].model;
    gPoly_fonts[pIndex].glyphs[pIndex].model = model;
    if (model == NULL) {
        FatalError(kFatalError_CouldNotCreateTexturesPages_S, pName);
    }
    model->faces[0].vertices[0] = 0;
    model->faces[0].vertices[1] = 1;
    model->faces[0].vertices[2] = 2;
    model->faces[1].vertices[0] = 1;
    model->faces[1].vertices[1] = 3;
    model->faces[1].vertices[2] = 2;
    BrVector3Set(&model->vertices[0].p, 0.0f, 0.0f, -1.2f);
    BrVector3Set(&model->vertices[1].p, (float) pWidth, 0.0f, -1.2f);
    BrVector3Set(&model->vertices[2].p, 0.f, (float) -pHeight, -1.2f);
    BrVector3Set(&model->vertices[3].p, (float) pWidth, (float) -pHeight, -1.2f);
    BrVector2Set(&model->vertices[0].map, 0.0f, 0.0f);
    BrVector2Set(&model->vertices[1].map, 1.0f, 0.0f);
    BrVector2Set(&model->vertices[2].map, 0.0f, 1.0f);
    BrVector2Set(&model->vertices[3].map, 1.0f, 1.0f);
    ColourVertices(model, pIndex);
    model->flags &= ~(BR_MODF_KEEP_ORIGINAL | BR_MODF_UPDATEABLE);
    BrModelAdd(model);
    return model;
}

// FUNCTION: CARMA2_HW 0x00464a70
br_pixelmap* C2_HOOK_FASTCALL GetThisFuckingPixelmap(const char* path, const char* glyph_name, int loadFromDisk) {
    tPath_name pathBuffer;
    FILE* f;
    char* str;
    size_t i;

    PathCat(pathBuffer, path, "PIXIES.P16");
    f = PFfopen(pathBuffer, "rb");
    if (f != NULL) {
        PFfclose(f);
        if (loadFromDisk) {
            gPixelmap_buffer_size = BrPixelmapLoadMany(pathBuffer, gPixelmap_buffer, REC2_ASIZE(gPixelmap_buffer));
        }
        strcpy(pathBuffer, glyph_name);

        str = strchr(pathBuffer, '.');
        *str = '\0';

        for (i = 0; i < gPixelmap_buffer_size; i++) {
            if (gPixelmap_buffer[i] != NULL && DRStricmp(gPixelmap_buffer[i]->identifier, pathBuffer) == 0) {
                return gPixelmap_buffer[i];
            }
        }
        return NULL;
    } else {
        PathCat(pathBuffer, path, glyph_name);
        return DRImageLoad(pathBuffer);
    }
}

void C2_HOOK_FASTCALL KillThePixies(void) {
    int i;

    for (i = 0; i < gPixelmap_buffer_size; i++) {

        if (gPixelmap_buffer[i] != NULL) {
            BrPixelmapFree(gPixelmap_buffer[i]);
            gPixelmap_buffer[i] = NULL;
        }
    }
    gPixelmap_buffer_size = 0;
}

// FUNCTION: CARMA2_HW 0x004640d0
void C2_HOOK_FASTCALL CheckAvailabilityOfThisFont(int pFont) {

    if (!gPoly_fonts[pFont].available) {
        LoadInterfaceFonts();
    }
}

// FUNCTION: CARMA2_HW 0x004640f0
void C2_HOOK_FASTCALL DisposeInterfaceFonts(void) {
    int i;
    br_pixelmap* map;

    if (gInterface_fonts_loaded) {

        RemovePolyFont(kPolyfont_hand_green_15pt_unlit);
        RemovePolyFont(kPolyfont_hand_green_15pt_lit);
        RemovePolyFont(kPolyfont_hand_red_15pt_unlit);
        RemovePolyFont(kPolyfont_hand_red_15pt_lit);
        RemovePolyFont(kPolyfont_hand_green_10pt_unlit);
        RemovePolyFont(kPolyfont_hand_green_10pt_lit);
        RemovePolyFont(kPolyfont_serp_red_15pt_lit);
        RemovePolyFont(kPolyfont_serp_red_30pt_lit);
        RemovePolyFont(kPolyfont_serp_green_30pt_unlit);
        RemovePolyFont(kPolyfont_serp_green_30pt_lit);
        RemovePolyFont(kPolyfont_serp_green_38pt_unlit);
        RemovePolyFont(kPolyfont_serp_green_38pt_lit);
        RemovePolyFont(kPolyfont_highlighter);

        for (i = 0; i < gInterface_polyfont_texture_pages; i++) {
            map = gTexture_maps[gInterface_polyfont_texture_pages + i];

            if (map != NULL) {

                BrMapRemove(map);
                BrPixelmapFree(map);
                gTexture_maps[gInterface_polyfont_texture_pages + i] = NULL;
            }
        }
        gInterface_fonts_loaded = 0;
    }
}

// FUNCTION: CARMA2_HW 0x00464290
void C2_HOOK_FASTCALL RemovePolyFont(int pFont) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gPoly_fonts[pFont].glyphs); i++) {
        tPolyFontGlyph* glyph = &gPoly_fonts[pFont].glyphs[i];

        if (glyph->used) {

            glyph->material = NULL;
            glyph->model = NULL;
            glyph->used = 0;
        }
    }
    gPoly_fonts[pFont].available = 0;
}

// FUNCTION: CARMA2_HW 0x004642d0
void C2_HOOK_FASTCALL LoadInterfaceFonts(void) {

    if (!gInterface_fonts_loaded) {
        gSize_font_texture_pages = gInitial_count_font_texture_pages;
        CreatePolyFont(kPolyfont_hand_green_15pt_unlit, "HAND15U", 1.0f, 16);
        CreatePolyFont(kPolyfont_hand_green_15pt_lit, "HAND15", 1.0f, 16);
        CreatePolyFont(kPolyfont_hand_red_15pt_unlit, "HAND15U", 1.0f, 16);
        CreatePolyFont(kPolyfont_hand_red_15pt_lit, "HAND15", 1.0f, 16);
        CreatePolyFont(kPolyfont_hand_green_10pt_unlit, "HAND10U", 1.0f, 16);
        CreatePolyFont(kPolyfont_hand_green_10pt_lit, "HAND10", 1.0f, 16);
        CreatePolyFont(kPolyfont_serp_red_15pt_lit, "SERP15", 1.0f, 16);
        CreatePolyFont(kPolyfont_serp_red_30pt_lit, "SERP30", 1.0f, 32);
        CreatePolyFont(kPolyfont_serp_green_30pt_unlit, "SERP30U", 1.0f, 32);
        CreatePolyFont(kPolyfont_serp_green_30pt_lit, "SERP30", 1.0f, 32);
        CreatePolyFont(kPolyfont_serp_green_38pt_unlit, "SERP38U", 1.0f, 64);
        CreatePolyFont(kPolyfont_serp_green_38pt_lit, "SERP38", 1.0f, 64);
        CreatePolyFont(kPolyfont_highlighter, "HAND15lo", 1.0f, 16);
        gInterface_polyfont_texture_pages = gSize_font_texture_pages - gInitial_count_font_texture_pages;
        gInterface_fonts_loaded = 1;
    }
}

// FUNCTION: CARMA2_HW 0x004643f0
void C2_HOOK_FASTCALL LoadPolyFont(const char* pName, int pSize, int pIndex) {
    int tex_x;
    int tex_y;
    tPath_name the_path;
    char s[256];
    char s2[256];
    int i;
    tTWTVFS twt;
    FILE* f;
    br_pixelmap *blank_map;
    int count_characters;
    tPolyFontGlyph* glyph;
    int ascii;
    br_pixelmap* map;

    PathCat(the_path, gApplication_path, gGraf_specs[gGraf_spec_index].data_dir_name);
    PathCat(the_path, the_path, "FONTS");
    PathCat(the_path, the_path, pName);

    twt = OpenPackFileAndSetTiffLoading(the_path);

    strcpy(s, the_path);
    PathCat(s, s, "FONT.TXT");
    tex_x = 0;
    tex_y = 0;
    f = PFfopen(s, "rt");
    if (f == NULL) {
        FatalError(kFatalError_CannotLoadFontWidthTable_S, pName);
    }

    strcpy(gPoly_fonts[pIndex].name, pName);
    /* number of characters */
    gPoly_fonts[pIndex].numberOfCharacters = count_characters = GetAnInt(f);
    /* inter-character spacing */
    gPoly_fonts[pIndex].interCharacterSpacing = GetAnInt(f);
    /* ASCII offset (value of first character) */
    gPoly_fonts[pIndex].asciiOffset = GetAnInt(f);
    /* font character height */
    gPoly_fonts[pIndex].fontCharacterHeight = GetAnInt(f);
    /* Blank width */
    gPoly_fonts[pIndex].widthOfBlank = GetAnInt(f);
    gPoly_fonts[pIndex].fontSize = pSize;
    gPoly_fonts[pIndex].available = 1;
    C2_HOOK_BUG_ON(REC2_ASIZE(gPoly_fonts[pIndex].glyphs) != 256);
    for (i = 0; i < REC2_ASIZE(gPoly_fonts[pIndex].glyphs); i++) {
        glyph = &gPoly_fonts[pIndex].glyphs[i];

        glyph->model = NULL;
        glyph->material = NULL;
        glyph->used = 0;
        glyph->index = 0;
        glyph->glyph_width = pSize;
    }
    PFfclose(f);
    strcpy(s, the_path);
    PathCat(s, s, "BLANK.PIX");
    blank_map = GetThisFuckingPixelmap(the_path, "BLANK.PIX", 1);
    if (blank_map == NULL) {
        FatalError(kFatalError_CannotLoadFontImage_S, "BLANK.PIX");
    }

    for (i = 0; i < count_characters; i++) {

        strcpy(s, the_path);
        ascii = gPoly_fonts[pIndex].asciiOffset + i;
        sprintf(s2, "%d.PIX", ascii);
        PathCat(s, s, s2);
        map = GetThisFuckingPixelmap(the_path, s2, 0);
        if (map != NULL) {
            if (gTexture_maps[gSize_font_texture_pages] == NULL) {
                gTexture_maps[gSize_font_texture_pages] = BrPixelmapAllocate(blank_map->type, 64, 64, NULL, 0);
                if (gTexture_maps[gSize_font_texture_pages] == NULL) {
                    FatalError(kFatalError_CouldNotCreateTexturesPages_S, pName);
                }
                BrMapAdd(gTexture_maps[gSize_font_texture_pages]);
            }
            if (map->type != gTexture_maps[gSize_font_texture_pages]->type) {
                printf("FONT:%s  CHAR:%c (%i)\n", pName, ascii, ascii);
                fflush(stdout);
                BrFailure("BLOODY FONTS :(");
            } else {
                DRPixelmapRectangleCopy(
                        gTexture_maps[gSize_font_texture_pages],
                        tex_x, tex_y,
                        map,
                        0, 0,
                        pSize, pSize);
                BrMapUpdate(gTexture_maps[gSize_font_texture_pages], BR_MAPU_ALL);
                gPoly_fonts[pIndex].glyphs[ascii].index = gSize_font_texture_pages;
                gPoly_fonts[pIndex].glyphs[ascii].used = 1;
                BrVector2Set(&gPoly_fonts[pIndex].glyphs[ascii].texCoord, (float) tex_x / 64.0f, (float)tex_y  / 64.0f);
                gPoly_fonts[pIndex].glyphs[ascii].glyph_width = FindCharacterWidth(map);
                gPoly_fonts[pIndex].glyphs[ascii].material = NULL;
                gPoly_fonts[pIndex].glyphs[ascii].model = CreateFontCharacterModel(pIndex, ascii, pSize, pSize, pName);
                tex_x += pSize;
                if (tex_x >= 64) {
                    tex_y += pSize;
                    tex_x = 0;
                    if (tex_y >= 64) {
                        gSize_font_texture_pages += 1;
                        if (gSize_font_texture_pages >= (int)REC2_ASIZE(gTexture_maps)) {
                            FatalError(kFatalError_CouldNotCreateTexturesPages_S, pName);
                        }
                        tex_x = 0;
                        tex_y = 0;
                    }
                }
            }
            if (twt < 0) {
                BrPixelmapFree(map);
            }
        }
    }
    KillThePixies();
    ClosePackFileAndSetTiffLoading(twt);
    gSize_font_texture_pages += 1;
}

// FUNCTION: CARMA2_HW 0x00464b80
br_model* C2_HOOK_FASTCALL CreateCharacterModel(int width, int height, int texture_id_x, int texture_id_y, const char* pageName) {
    br_model* pModel;
    br_scalar f_width;
    br_scalar f_map_x1;
    br_scalar f_map_y1;
    br_scalar f_map_x2;
    br_scalar f_map_y2;

    pModel = BrModelAllocate("String Model", 4, 2);
    if (pModel == NULL) {
        FatalError(kFatalError_CouldNotCreateTexturesPages_S, pageName);
    }
    f_width = (br_scalar)width;
    pModel->faces[0].vertices[0] = 0;
    pModel->faces[0].vertices[1] = 1;
    pModel->faces[0].vertices[2] = 2;
    pModel->faces[1].vertices[0] = 1;
    pModel->faces[1].vertices[1] = 3;
    pModel->faces[1].vertices[2] = 2;
    BrVector3Set(&pModel->vertices[0].p, 0.0f, 0.0f, -1.2f);
    BrVector3Set(&pModel->vertices[1].p, f_width, 0.0f, -1.2f);
    BrVector3Set(&pModel->vertices[2].p, 0.0f, (float)-height, -1.2f);
    BrVector3Set(&pModel->vertices[3].p, f_width, (float)-height, -1.2f);
    f_map_x1 = (br_scalar)texture_id_x / 64.0f;
    f_map_y1 = (br_scalar)texture_id_y / 64.0f;
    f_map_x2 = f_map_x1 + f_width / 64.0f;
    f_map_y2 = f_map_y1 + (br_scalar)height / 64.0f;
    BrVector2Set(&pModel->vertices[0].map, f_map_x1, f_map_y1);
    BrVector2Set(&pModel->vertices[1].map, f_map_x2, f_map_y1);
    BrVector2Set(&pModel->vertices[2].map, f_map_x1, f_map_y2);
    BrVector2Set(&pModel->vertices[3].map, f_map_x2, f_map_y2);
    pModel->flags |= BR_MODF_UPDATEABLE;
    BrModelAdd(pModel);
    return pModel;
}

// FUNCTION: CARMA2_HW 0x00464cf0
br_material* C2_HOOK_FASTCALL CreatePolyMaterial(int pTexture_id) {
    br_material *material;

    material = BrMaterialAllocate("Character Material");
    if (material == NULL) {
        BrFailure("Could not create font material");
    }
    material->flags = BR_MATF_FORCE_FRONT | BR_MATF_ALWAYS_VISIBLE | BR_MATF_SMOOTH | BR_MATF_PRELIT;
    material->colour_map = gTexture_maps[pTexture_id];
    BrMaterialAdd(material);
    return material;
}

// FUNCTION: CARMA2_HW 0x00464d40
void C2_HOOK_FASTCALL ColourVertices(br_model* pModel, int pFont_index) {

    if (pModel == NULL) {
        return;
    }
    pModel->vertices[0].red = gPoly_font_border_colours[pFont_index].tl.r;
    pModel->vertices[0].grn = gPoly_font_border_colours[pFont_index].tl.g;
    pModel->vertices[0].blu = gPoly_font_border_colours[pFont_index].tl.b;
    pModel->vertices[1].red = gPoly_font_border_colours[pFont_index].tr.r;
    pModel->vertices[1].grn = gPoly_font_border_colours[pFont_index].tr.g;
    pModel->vertices[1].blu = gPoly_font_border_colours[pFont_index].tr.b;
    pModel->vertices[2].red = gPoly_font_border_colours[pFont_index].bl.r;
    pModel->vertices[2].grn = gPoly_font_border_colours[pFont_index].bl.g;
    pModel->vertices[2].blu = gPoly_font_border_colours[pFont_index].bl.b;
    pModel->vertices[3].red = gPoly_font_border_colours[pFont_index].br.r;
    pModel->vertices[3].grn = gPoly_font_border_colours[pFont_index].br.g;
    pModel->vertices[3].blu = gPoly_font_border_colours[pFont_index].br.b;
}

// FUNCTION: CARMA2_HW 0x00464df0
void C2_HOOK_FASTCALL SolidPolyFontText(const char* pText, int pX, int pY, int pFont, tJustification pJust, int pRender) {

    CheckAvailabilityOfThisFont(pFont);
    TransparentPolyFontText(pText, pX, pY, pFont, pJust, pRender, 1.0);
}

void C2_HOOK_FASTCALL InitPolyFontMaterials(void) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gPoly_font_materials); i++) {

        gPoly_font_materials[i] = CreatePolyMaterial(0);
    }
}

// FUNCTION: CARMA2_HW 0x00465250
br_material* C2_HOOK_FASTCALL GetPolyFontMaterial(int pFont_index, char pChar) {
    tU8 uchar;
    br_material* material;
    float f_size;
    tU32 packed_font_character;

    // GLOBAL: CARMA2 0x00686480
    static int poly_font_material_index;

    // GLOBAL: CARMA2 0x00686498
    static tU16 poly_font_material_generation;

    C2_HOOK_BUG_ON(sizeof(tPolyFont) != 0x1c68);
    C2_HOOK_BUG_ON(sizeof(tPolyFontGlyph) != 0x1c);

    uchar = pChar;
    material = gPoly_fonts[pFont_index].glyphs[uchar].material;
    packed_font_character = POLYFONT_FONT_CHARACTER(pFont_index, uchar);
    if (material != NULL && POLYFONT_MATERIAL_GET_FONT_CHARACTER(material) == packed_font_character) {
        material->user = POLYFONT_PACK_USER(packed_font_character, poly_font_material_generation);
        return material;
    }
    // Find "oldest" material, and re-use
    do {
        poly_font_material_index += 1;
        poly_font_material_generation += 1;
        if (poly_font_material_index >= (int)REC2_ASIZE(gPoly_font_materials)) {
            poly_font_material_index = 0;
        }
    } while ((poly_font_material_generation - POLYFONT_MATERIAL_GET_GENERATION(gPoly_font_materials[poly_font_material_index])) < (int)REC2_ASIZE(gPoly_font_materials));
    material = gPoly_font_materials[poly_font_material_index];
    gPoly_fonts[pFont_index].glyphs[uchar].material = material;
    material->colour_map = gTexture_maps[gPoly_fonts[pFont_index].glyphs[uchar].index];
    material->user = POLYFONT_PACK_USER(packed_font_character, poly_font_material_generation);;
    f_size = (float)gPoly_fonts[pFont_index].fontSize;
    BrMatrix23Scale(&material->map_transform, f_size / 64.0f, f_size / 64.0f);
    material->map_transform.m[2][0] = gPoly_fonts[pFont_index].glyphs[uchar].texCoord.v[0];
    material->map_transform.m[2][1] = gPoly_fonts[pFont_index].glyphs[uchar].texCoord.v[1];
    material->extra_prim = NULL;
    BrMaterialUpdate(material, BR_MATU_ALL);
    return material;
}

// FUNCTION: CARMA2_HW 0x00464e40
void C2_HOOK_FASTCALL PolyFontText(const char *pText, int pX, int pY, int pFont, tJustification pJust, int pRender) {
    int text_len;
    int i;
    float draw_x;
    int draw_y;

    CheckAvailabilityOfThisFont(pFont);
    if (pRender) {
        CleanPolyFontDanglers();
    }
    switch (pJust) {
#ifdef REC2_FIX_BUGS
    case eJust_left:
        break;
#endif
    case eJust_centre:
        pX -= PolyFontTextWidth(pFont, pText) / 2;
        break;
    case eJust_right:
        pX -= PolyFontTextWidth(pFont, pText);
        break;
    }

    text_len = strlen(pText);
    if (gCount_polyfont_glyph_actors + text_len < (int)REC2_ASIZE(gPolyfont_glyph_actors)) {
        draw_x = (float)pX;
        draw_y = pY;
        for (i = 0; i < text_len; i++) {
            tU8 c = pText[i];

            if (c == '\r') {
                draw_x = (float)pX;;
                draw_y += gPoly_fonts[pFont].fontCharacterHeight;
                continue;
            }
            if (c >= 'a' && c <= 'z') {
                c = c - 'a' + 'A';
            }
            if (!gPoly_fonts[pFont].glyphs[c].used) {
                draw_x += (float)gPoly_fonts[pFont].widthOfBlank;
                continue;
            }
            gPolyfont_glyph_actors[gCount_polyfont_glyph_actors]->model = gPoly_fonts[pFont].glyphs[c].model;
            gPolyfont_glyph_actors[gCount_polyfont_glyph_actors]->material = GetPolyFontMaterial(pFont, c);
            BrActorAdd(gString_root_actor, gPolyfont_glyph_actors[gCount_polyfont_glyph_actors]);
            BrVector3Set(&gPolyfont_glyph_actors[gCount_polyfont_glyph_actors]->t.t.translate.t, (float)draw_x, (float)-draw_y, -1.1f);
            gCount_polyfont_glyph_actors += 1;
            draw_x += gPoly_fonts[pFont].glyphs[c].glyph_width + gPoly_fonts[pFont].interCharacterSpacing;
        }
        if (pRender) {
            int original_origin_x;
            int original_origin_y;
            int original_width;
            int original_height;
            int original_base_x;
            int original_base_y;

            BrActorAdd(gHUD_root, gString_root_actor);
            original_origin_x = gBack_screen->origin_x;
            original_origin_y = gBack_screen->origin_y;
            original_width = gBack_screen->width;
            original_height = gBack_screen->height;
            original_base_x = gBack_screen->base_x;
            original_base_y = gBack_screen->base_y;
            gBack_screen->origin_x = 0;
            gBack_screen->origin_y = 0;
            gBack_screen->base_x = 0;
            gBack_screen->base_y = 0;
            gBack_screen->width = 640;
            gBack_screen->height = 480;
            BrZbSceneRender(gHUD_root, gHUD_camera, gBack_screen, gDepth_buffer);
            gBack_screen->origin_x = original_origin_x;
            gBack_screen->origin_y = original_origin_y;
            gBack_screen->width = original_width;
            gBack_screen->height = original_height;
            gBack_screen->base_x = original_base_x;
            gBack_screen->base_y = original_base_y;
            BrActorRemove(gString_root_actor);
            CleanPolyFontDanglers();
        }
    }
}

// FUNCTION: CARMA2_HW 0x00465380
void C2_HOOK_FASTCALL TransparentPolyFontText(const char *pText, int pX, int pY, int pFont, tJustification pJust, int pRender, double pOpacity_factor) {
    int text_len;
    int i;
    float draw_x;
    int draw_y;
    br_token_value tvs[3];

    CheckAvailabilityOfThisFont(pFont);
    if (pRender) {
        CleanPolyFontDanglers();
    }
    switch (pJust) {
#ifdef REC2_FIX_BUGS
    case eJust_left:
        break;
#endif
    case eJust_centre:
        pX -= PolyFontTextWidth(pFont, pText) / 2;
        break;
    case eJust_right:
        pX -= PolyFontTextWidth(pFont, pText);
        break;
    }

    text_len = strlen(pText);
    if (gCount_polyfont_glyph_actors + text_len < (int)REC2_ASIZE(gPolyfont_glyph_actors)) {
        tvs[0].t = BRT_BLEND_B;
        tvs[0].v.b = 1;
        tvs[1].t = BRT_OPACITY_X;
        tvs[1].v.x = BR_FIXED_INT(255.0 * pOpacity_factor);
        tvs[2].t = BR_NULL_TOKEN;
        tvs[2].v.u32 = 0;

        draw_x = (float)pX;
        draw_y = pY;
        for (i = 0; i < text_len; i++) {
            tU8 c = pText[i];

            if (c == '\r') {
                draw_x = (float)pX;
                draw_y += gPoly_fonts[pFont].fontCharacterHeight;
                continue;
            }
            if (c >= 'a' && c <= 'z') {
                c = c - 'a' + 'A';
            }
            if (!gPoly_fonts[pFont].glyphs[c].used) {
                draw_x += (float)gPoly_fonts[pFont].widthOfBlank;
                continue;
            }
            gPolyfont_glyph_actors[gCount_polyfont_glyph_actors]->model = gPoly_fonts[pFont].glyphs[c].model;
            gPolyfont_glyph_actors[gCount_polyfont_glyph_actors]->material = GetPolyFontMaterial(pFont, c);
            gPolyfont_glyph_actors[gCount_polyfont_glyph_actors]->material->extra_prim = tvs;
            BrMaterialUpdate(gPolyfont_glyph_actors[gCount_polyfont_glyph_actors]->material, BR_MATU_ALL);
            BrActorAdd(gString_root_actor, gPolyfont_glyph_actors[gCount_polyfont_glyph_actors]);
            BrVector3Set(&gPolyfont_glyph_actors[gCount_polyfont_glyph_actors]->t.t.translate.t, draw_x, (float)-draw_y, -1.1f);
            gCount_polyfont_glyph_actors += 1;
            draw_x += gPoly_fonts[pFont].glyphs[c].glyph_width + gPoly_fonts[pFont].interCharacterSpacing;
        }
        if (pRender) {
            int original_origin_x;
            int original_origin_y;
            int original_base_x;
            int original_base_y;

            BrActorAdd(gHUD_root, gString_root_actor);
            original_origin_x = gBack_screen->origin_x;
            original_origin_y = gBack_screen->origin_y;
            original_base_x = gBack_screen->base_x;
            original_base_y = gBack_screen->base_y;
            gBack_screen->origin_x = 0;
            gBack_screen->origin_y = 0;
            gBack_screen->base_x = 0;
            gBack_screen->base_y = 0;
            BrZbSceneRender(gHUD_root, gHUD_camera, gBack_screen, gDepth_buffer);
            gBack_screen->origin_x = original_origin_x;
            gBack_screen->origin_y = original_origin_y;
            gBack_screen->base_x = original_base_x;
            gBack_screen->base_y = original_base_y;
            BrActorRemove(gString_root_actor);
            CleanPolyFontDanglers();
        }
    }
}

void C2_HOOK_FASTCALL DrawDigitAt(const br_pixelmap* pSource, br_pixelmap* pDest, br_uint_16 pX, br_uint_16 pY, br_uint_16 pDigit_height, int pDigit) {

    DRPixelmapRectangleMaskedCopy(pDest, pX, pY, pSource, 0, pDigit * pDigit_height ,pSource->width, pDigit_height);
}

// FUNCTION: CARMA2_HW 0x004657c0
void C2_HOOK_FASTCALL DrawNumberAt(const br_pixelmap* pSrc, br_pixelmap* pDest, int pX, int pY, int pX_pitch, int pY_pitch, int pValue, int pDigit_count, int pLeading_zeroes) {
    int i;
    int digit;

    for (i = pDigit_count - 1; i >= 0; i--) {
        digit = pValue % 10;
        pValue /= 10;
        if (pValue != 0 || pLeading_zeroes || i == pDigit_count - 1) {
            DrawDigitAt(pSrc, pDest, pX + pX_pitch * i, pY, pY_pitch, digit);
        }
    }
}

// FUNCTION: CARMA2_HW 0x00465850
void C2_HOOK_FASTCALL LoadFont(int pFont_ID) {
    int i;
    FILE* file;
    tPath_name the_path;

    if (gFonts[pFont_ID].images != NULL) {
        return;
    }
    PathCat(the_path, gApplication_path, gGraf_specs[gGraf_spec_index].data_dir_name);
    PathCat(the_path, the_path, "FONTS");
    PathCat(the_path, the_path, gFont_names[pFont_ID]);
    strcat(the_path, ".PIX");
    if (gFonts[pFont_ID].file_read_once) {
        return;
    }
    the_path[strlen(the_path) - 3] = '\0';
    strcat(the_path, "TXT");
    file = DRfopen(the_path, "rt");
    if (file == NULL) {
        FatalError(kFatalError_CannotLoadFontWidthTable_S, gFont_names[pFont_ID]);
    }
    gFonts[pFont_ID].height = GetAnInt(file);
    gFonts[pFont_ID].width = GetAnInt(file);
    gFonts[pFont_ID].spacing = GetAnInt(file);
    gFonts[pFont_ID].offset = GetAnInt(file);
    gFonts[pFont_ID].num_entries = GetAnInt(file);
    gFonts[pFont_ID].id = pFont_ID;
    if (gFonts[pFont_ID].width <= 0) {
        for (i = 0; i < gFonts[pFont_ID].num_entries; i++) {
            gFonts[pFont_ID].width_table[i] = GetAnInt(file);
        }
    }
    PFfclose(file);
    gFonts[pFont_ID].file_read_once = 1;
}

// FUNCTION: CARMA2_HW 0x00465a00
void C2_HOOK_FASTCALL DisposeFont(int pFont_ID) {

    if (gFonts[pFont_ID].images != NULL) {
        BrPixelmapFree(gFonts[pFont_ID].images);
        gFonts[pFont_ID].images = NULL;
        gFonts[pFont_ID].file_read_once = 0;
    }
}

// FUNCTION: CARMA2_HW 0x00465a40
void C2_HOOK_FASTCALL InitDRFonts(void) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(gFonts); i++) {
        gFonts[i].images = NULL;
        gFonts[i].file_read_once = 0;
        gFonts[i].id = 11;
    }
}

// FUNCTION: CARMA2_HW 0x00465a70
void C2_HOOK_FASTCALL DRPixelmapText(br_pixelmap* pPixelmap, int pX, int pY, const tDR_font* pFont, const char* pText, int pRight_edge) {

    PolyFontText(pText, pX, pY, GetPolyFontIndexToReplaceDRfontWith(pFont), eJust_left, gRender_poly_text);
}

// FUNCTION: CARMA2_HW 0x00465aa0
void C2_HOOK_FASTCALL DRPixelmapCleverText(br_pixelmap* pPixelmap, int pX, int pY, tDR_font* pFont, const char* pText, int pRight_edge) {
    int i;
    char s[512];
    int s_end;
    int x_text;
    int x_cursor;
    int len_text;

    s_end = -1;
    x_text = pX;
    x_cursor = pX;
    len_text = strlen(pText);
    for (i = 0; i < len_text; i++) {

        if (pText[i] < 0) {
            if (s_end >= 0) {
                s[s_end + 1] = '\0';
                PolyFontText(s, x_text, pY, GetPolyFontIndexToReplaceDRfontWith(pFont), eJust_left, gRender_poly_text);
                s_end = -1;
                x_text = x_cursor;
            }
            pY -= (PolyFontHeight(GetPolyFontIndexToReplaceDRfontWith(&gFonts[-pText[i]])) - PolyFontHeight(GetPolyFontIndexToReplaceDRfontWith(pFont))) / 2;
            pFont = &gFonts[-pText[i]];
        } else {
            s_end += 1;
            s[s_end] = pText[i];
            x_cursor += CharacterWidth(GetPolyFontIndexToReplaceDRfontWith(pFont), pText[i]) + GetSpacing(GetPolyFontIndexToReplaceDRfontWith(pFont));
        }
    }
    if (s_end >= 0) {
        s[s_end + 1] = '\0';
        PolyFontText(s, x_text, pY, GetPolyFontIndexToReplaceDRfontWith(pFont), eJust_left, gRender_poly_text);
    }
}

// FUNCTION: CARMA2_HW 0x00465ca0
int C2_HOOK_FASTCALL PolyFontTextWidth(int pFont, const char* pText) {
    int len;
    int i;
    int text_width;
    int spacing;

    text_width = 0;
    spacing = 0;
    CheckAvailabilityOfThisFont(pFont);
    len = (int)strlen(pText);
    for (i = 0; i < len; i++) {
        text_width += CharacterWidth(pFont, pText[i]);
    }
    if (len > 1) {
        spacing = (len - 1) * gPoly_fonts[pFont].interCharacterSpacing;
    }
    return text_width + spacing;
}

// FUNCTION: CARMA2_HW 0x00465d50
int C2_HOOK_FASTCALL DRTextWidth(const tDR_font* pFont, const char* pText) {

    return PolyFontTextWidth(GetPolyFontIndexToReplaceDRfontWith(pFont), pText);
}

// FUNCTION: CARMA2_HW 0x00465e10
int C2_HOOK_FASTCALL DRTextCleverWidth(const tDR_font* pFont, const char* pText) {
    int polyfont_index;
    int i;
    int len;
    int result;
    int spacing;

    polyfont_index = GetPolyFontIndexToReplaceDRfontWith(pFont);
    len = strlen(pText);

    i = 0;
    result = 0;
    for (; i < len; i++) {
        tS8 c;

        c = pText[i];
        if (c < 0) {
            polyfont_index = GetPolyFontIndexToReplaceDRfontWith(&gFonts[-(int)c]);
        } else {
            if (i < len - 1) {
                spacing = gPoly_fonts[polyfont_index].interCharacterSpacing;
            } else {
                spacing = 0;
            }
            result += spacing + CharacterWidth(polyfont_index, c);
        }
    }
    return result;
}

// DRPixelmapCentredText

// OoerrIveGotTextInMeBoxMissus

// FUNCTION: CARMA2_HW 0x00466050
br_font* C2_HOOK_FASTCALL LoadBRFont(const char* pName) {
    FILE* f;
    tPath_name the_path;
    br_font* the_font;
    tU32 data_size;
    int i;

    C2_HOOK_BUG_ON(sizeof(br_font) != 24);

    PathCat(the_path, gApplication_path, gGraf_specs[gGraf_spec_index].data_dir_name);
    PathCat(the_path, the_path, "FONTS");
    PathCat(the_path, the_path, pName);
    f = DRfopen(the_path, "rb");
    PossibleService();
    the_font = BrMemAllocate(sizeof(br_font), kMem_br_font);

    // we read 0x18 bytes as that is the size of the struct in 32 bit code.
    PFfread(the_font, 0x18, 1, f);
#if !defined(C2_BIG_ENDIAN)
    the_font->flags = BrSwap32(the_font->flags);

    // swap endianness
    the_font->glyph_x = the_font->glyph_x >> 8 | the_font->glyph_x << 8;
    the_font->glyph_y = the_font->glyph_y >> 8 | the_font->glyph_y << 8;
    the_font->spacing_x = the_font->spacing_x >> 8 | the_font->spacing_x << 8;
    the_font->spacing_y = the_font->spacing_y >> 8 | the_font->spacing_y << 8;
#endif

    // data_size = 256 * sizeof(br_int_8);
    the_font->width = BrMemAllocate(256 * sizeof(br_int_8), kMem_br_font_wid);
    PFfread(the_font->width, 256 * sizeof(br_int_8), 1, f);
    // data_size = 256 * sizeof(br_uint_16);
    the_font->encoding = BrMemAllocate(256 * sizeof(br_uint_16), kMem_br_font_enc);
    PFfread(the_font->encoding, 256 * sizeof(br_uint_16), 1, f);
#if !defined(C2_BIG_ENDIAN)
    for (i = 0; i < 256; i++) {
        the_font->encoding[i] = the_font->encoding[i] >> 8 | the_font->encoding[i] << 8;
    }
#endif
    PossibleService();
    PFfread(&data_size, sizeof(tU32), 1, f);
#if !defined(C2_BIG_ENDIAN)
    data_size = BrSwap32(data_size);
#endif
    PossibleService();
    the_font->glyphs = BrMemAllocate(data_size, kMem_br_font_glyphs);
    PFfread(the_font->glyphs, data_size, 1u, f);
    PFfclose(f);
    return the_font;
}

// FUNCTION: CARMA2_HW 0x00466210
int C2_HOOK_FASTCALL GetPolyFontIndexToReplaceDRfontWith(const tDR_font* pFont) {

    return gDRFont_to_polyfont_mapping[pFont->id];
}
