#include "polyfont.h"

#include "drmem.h"
#include "52-errors.h"
#include "font.h"
#include "globvars.h"
#include "grafdata.h"
#include "graphics.h"
#include "loading.h"
#include "platform.h"
#include "utility.h"

#include "rec2_types.h"
#include "brender/br_types.h"

#include "c2_string.h"

#include "rec2_macros.h"


// GLOBAL: CARMA2_HW 0x00765ea0
int gInitial_count_font_texture_pages;

// GLOBAL: CARMA2_HW 0x0068648c
int gSize_font_texture_pages;

// GLOBAL: CARMA2_HW 0x0074cf10
br_actor* gString_root_actor;

// GLOBAL: CARMA2_HW 0x0074ab84
br_actor* gCar_icons_model_actor;

// GLOBAL: CARMA2_HW 0x0074a780
tCar_icon gCar_icons[128];

// GLOBAL: CARMA2_HW 0x0059ad28
int gRender_poly_text = 1;

// GLOBAL: CARMA2_HW 0x00686488
int gInterface_fonts_loaded;

// GLOBAL: CARMA2_HW 0x00686494
int gInterface_polyfont_texture_pages;

// FUNCTION: CARMA2_HW 0x00463730
int C2_HOOK_FASTCALL PolyFontHeight(int pIndex) {

    CheckAvailabilityOfThisFont(pIndex);
    return gPolyFonts[pIndex].fontCharacterHeight;
}

// FUNCTION: CARMA2_HW 0x004637d0
int C2_HOOK_FASTCALL CharacterWidth(int pIndex, tU8 pCharacter) {

    if ('a' <= pCharacter && pCharacter <= 'z') {
        pCharacter = pCharacter - ('a' - 'A');
    }
    if (gPolyFonts[pIndex].glyphs[pCharacter].used) {
         return gPolyFonts[pIndex].glyphs[pCharacter].glyph_width;
    }
    return gPolyFonts[pIndex].widthOfBlank;
}

// FUNCTION: CARMA2_HW 0x00463830
int C2_HOOK_FASTCALL GetPolyFontInterCharacterSpacing(int pIndex) {

    return gPolyFonts[pIndex].interCharacterSpacing;
}

// FUNCTION: CARMA2_HW 0x00463850
void C2_HOOK_FASTCALL RenderInterfaceBlendedPolyText(int pFont, const char* pText, int pX, int pY, int pWidth, int pHeight, tJustification pJustification, undefined4 pParam_8) {

    CheckAvailabilityOfThisFont(pFont);
    TransparentPolyFontTextInABox(pFont, pText, pX, pY, pWidth, pHeight, pJustification, pParam_8, 1.0);
}

// FUNCTION: CARMA2_HW 0x004638b0
void C2_HOOK_FASTCALL RenderPolyText(int pFont, const char* pText, int pLeft, int pTop, int pRight, int pBottom, tJustification pJust, int pRender) {
    char s[256];
    int s_len;
    int x;
    int y;
    int in_start_put;
    int in_end_put;
    int in_pos;
    int newline;
    int text_x;
    int max_width;

    CheckAvailabilityOfThisFont(pFont);

    in_start_put = 0;
    text_x = 0;
    max_width = pRight - pLeft;
    in_pos = 0;
    s_len = 0;
    s[s_len] = '\0';
    y = pTop;

    while (pText[in_pos] != '\0') {
        int chr;

        chr = pText[in_pos];
        newline = chr == '\r';
        s[s_len] = chr;
        if (newline) {
            s[s_len] = ' ';
        }
        s[s_len + 1] = '\0';
        if (!newline) {
            text_x += CharacterWidth(pFont, (tU8)chr);
        }
        in_end_put = in_pos;
        if (text_x > max_width || newline) {
            int len_put;

            /* Avoid breaking words in parts ... */
            while (in_end_put > in_start_put && pText[in_end_put] != ' ') {
                in_end_put -= 1;
            }
            /* ... but break when the word is too long */
            if (in_end_put == in_start_put) {
                in_end_put = in_pos;
            }
            len_put = in_end_put - in_start_put;
            s[len_put + 1] = '\0';
            if (pText[in_end_put] == ' ') {
                in_end_put += 1;
            }
            s[len_put] = '\0';
            if (pJust == eJust_centre) {
                x = (pRight + pLeft) / 2;
            } else {
                x = pLeft;
            }
            RenderPolyTextLine(s, x, y, pFont, pJust, gRender_poly_text);
            while (pText[in_end_put] == '\r' || pText[in_end_put] == ' ') {
                in_end_put++;
            }
            y += PolyFontHeight(pFont);
            text_x = 0;
            in_start_put = in_end_put;
            s_len = 0;
            in_pos = in_end_put;
        } else {
            s_len++;
            in_pos++;
        }
    }
    if (s_len != 0) {
        int x;
        tJustification justif;

        if (pJust == eJust_centre) {
            x = (pRight + pLeft) / 2;
            justif = eJust_centre;
        } else {
            x = pLeft;
            justif = pJust;
        }
        RenderPolyTextLine(s, x, y, pFont, justif, gRender_poly_text);
    }
}

// FUNCTION: CARMA2_HW 0x00463ad0
void C2_HOOK_FASTCALL TransparentPolyFontTextInABox(int pFont, const char* pText, int pLeft, int pTop, int pRight, int pBottom, tJustification pJust, undefined4 pUnknown, double pBlend) {
    char s[256];
    int s_len;
    int x;
    int y;
    int in_start_put;
    int in_end_put;
    int in_pos;
    int newline;
    int text_x;
    int max_width;

    if (pText == NULL) {
        return;
    }

    in_start_put = 0;
    text_x = 0;
    max_width = pRight - pLeft;
    in_pos = 0;
    s_len = 0;
    s[s_len] = '\0';
    y = pTop;

    while (pText[in_pos] != '\0') {
        int chr;

        chr = pText[in_pos];
        newline = chr == '\r';
        s[s_len] = chr;
        if (newline) {
            s[s_len] = ' ';
        }
        s[s_len + 1] = '\0';
        if (!newline) {
            text_x += CharacterWidth(pFont, (tU8)chr);
        }
        in_end_put = in_pos;
        if (text_x > max_width || newline) {
            int len_put;

            /* Avoid breaking words in parts ... */
            while (in_end_put > in_start_put && pText[in_end_put] != ' ') {
                in_end_put -= 1;
            }
            /* ... but break when the word is too long */
            if (in_end_put == in_start_put) {
                in_end_put = in_pos;
            }
            len_put = in_end_put - in_start_put;
            s[len_put + 1] = '\0';
            if (pText[in_end_put] == ' ') {
                in_end_put += 1;
            }
            s[len_put] = '\0';
            if (pJust == eJust_centre) {
                x = (pRight + pLeft) / 2;
            } else {
                x = pLeft;
            }
            TransparentPolyFontText(s, x, y, pFont, pJust, gRender_poly_text, pBlend);
            while (pText[in_end_put] == '\r' || pText[in_end_put] == ' ') {
                in_end_put++;
            }
            y += PolyFontHeight(pFont);
            text_x = 0;
            in_start_put = in_end_put;
            s_len = 0;
            in_pos = in_start_put;
        } else {
            s_len++;
            in_pos++;
        }
    }
    if (s_len != 0) {
        int x;
        tJustification justif;

        if (pJust == eJust_centre) {
            x = (pRight + pLeft) / 2;
            justif = eJust_centre;
        } else {
            x = pLeft;
            justif = pJust;
        }
        TransparentPolyFontText(s, x, y, pFont, justif, gRender_poly_text, pBlend);
    }
}

// FUNCTION: CARMA2_HW 0x00463760
int C2_HOOK_FASTCALL CalculatePolyFontMapWidth(br_pixelmap* pMap) {
    int y;
    int x;

    for (x = pMap->width - 1; x >= 0; x--) {
        union {
            tU16* pixel;
            tU8* raw;
        } ptr = { &((tU16*)pMap->pixels)[x] };
        for (y = 0; y < pMap->height; y++) {
            if (*ptr.pixel != 0) {
                return x + 1;
            }
            ptr.raw += pMap->row_bytes;
        }
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x004643f0
void C2_HOOK_FASTCALL LoadPolyFont(const char* pName, int pSize, int pIndex) {
    tPath_name the_path;
    tTWTVFS twt;
    char s[256];
    char s2[256];
    FILE* f;
    int i;
    br_model* model;
    br_pixelmap* blank_map;
    int tex_x;
    int tex_y;

    PathCat(the_path, gApplication_path, gGraf_specs[gGraf_spec_index].data_dir_name);
    PathCat(the_path, the_path, "FONTS");
    PathCat(the_path, the_path, pName);

    twt = OpenPackFileAndSetTiffLoading(the_path);

    strcpy(s, the_path);
    PathCat(s, s, "FONT.TXT");
    f = PFfopen(s, "rt");
    if (f == NULL) {
        FatalError(kFatalError_CannotLoadFontWidthTable_S, pName);
    }

    strcpy(gPolyFonts[pIndex].name, pName);
    /* number of characters */
    gPolyFonts[pIndex].numberOfCharacters = GetAnInt(f);
    /* inter-character spacing */
    gPolyFonts[pIndex].interCharacterSpacing = GetAnInt(f);
    /* ASCII offset (value of first character) */
    gPolyFonts[pIndex].asciiOffset = GetAnInt(f);
    /* font character height */
    gPolyFonts[pIndex].fontCharacterHeight = GetAnInt(f);
    /* Blank width */
    gPolyFonts[pIndex].widthOfBlank = GetAnInt(f);
    gPolyFonts[pIndex].fontSize = pSize;
    gPolyFonts[pIndex].available = 1;
    C2_HOOK_BUG_ON(REC2_ASIZE(gPolyFonts[pIndex].glyphs) != 256);
    for (i = 0; i < REC2_ASIZE(gPolyFonts[pIndex].glyphs); i++) {
        tPolyFontGlyph* glyph = &gPolyFonts[pIndex].glyphs[i];

        glyph->model = NULL;
        glyph->material = NULL;
        glyph->used = 0;
        glyph->index = 0;
        glyph->glyph_width = pSize;
    }
    PFfclose(f);
    strcpy(s, the_path);
    PathCat(s, s, "BLANK.PIX");
    blank_map = LoadPolyFontPixiesP16(the_path, "BLANK.PIX", 1);
    if (blank_map == NULL) {
        FatalError(kFatalError_CannotLoadFontImage_S, "BLANK.PIX");
    }

    tex_x = 0;
    tex_y = 0;
    for (i = 0; i < (int)gPolyFonts[pIndex].numberOfCharacters; i++) {
        int ascii;
        br_pixelmap* map;

        strcpy(s, the_path);
        ascii = gPolyFonts[pIndex].asciiOffset + i;
        sprintf(s2, "%d.PIX", ascii);
        PathCat(s, s, s2);
        map = LoadPolyFontPixiesP16(the_path, s2, 0);
        if (map == NULL) {
            continue;
        }
        if (gTextureMaps[gSize_font_texture_pages] == NULL) {
            gTextureMaps[gSize_font_texture_pages] = BrPixelmapAllocate(blank_map->type, 64, 64, NULL, 0);
            if (gTextureMaps[gSize_font_texture_pages] == NULL) {
                FatalError(kFatalError_CouldNotCreateTexturesPages_S, pName);
            }
            BrMapAdd(gTextureMaps[gSize_font_texture_pages]);
        }
        if (map->type != gTextureMaps[gSize_font_texture_pages]->type) {
            printf("FONT:%s  CHAR:%c (%i)\n", pName, ascii, ascii);
            fflush(stdout);
            BrFailure("BLOODY FONTS :(");
        }
        DRPixelmapRectangleCopy(
                gTextureMaps[gSize_font_texture_pages],
                tex_x, tex_y,
                map,
                0, 0,
                pSize, pSize);
        BrMapUpdate(gTextureMaps[gSize_font_texture_pages], BR_MAPU_ALL);
        gPolyFonts[pIndex].glyphs[ascii].index = gSize_font_texture_pages;
        gPolyFonts[pIndex].glyphs[ascii].used = 1;
        BrVector2Set(&gPolyFonts[pIndex].glyphs[ascii].texCoord, (float) tex_x / 64.f, (float)tex_y  / 64.f);
        gPolyFonts[pIndex].glyphs[ascii].glyph_width = CalculatePolyFontMapWidth(map);
        gPolyFonts[pIndex].glyphs[ascii].material = NULL;
        model = gPolyFonts[pIndex].model;
        /* Find model for font size */
        while (model != NULL) {
            if (model->bounds.max.v[0] == pSize && model->bounds.min.v[1] == -pSize) {
                break;
            }
            model = model->user;
        }
        if (model == NULL) {
            model = BrModelAllocate("String Model", 4, 2);
            model->user = gPolyFonts[pIndex].glyphs[pIndex].model;
            gPolyFonts[pIndex].glyphs[pIndex].model = model;
            if (model == NULL) {
                FatalError(kFatalError_CouldNotCreateTexturesPages_S, pName);
            }
            model->faces[0].vertices[0] = 0;
            model->faces[0].vertices[1] = 1;
            model->faces[0].vertices[2] = 2;
            model->faces[1].vertices[0] = 1;
            model->faces[1].vertices[1] = 3;
            model->faces[1].vertices[2] = 2;
            BrVector3Set(&model->vertices[0].p, 0.f, 0.f, -1.2f);
            BrVector3Set(&model->vertices[1].p, (float) pSize, 0.f, -1.2f);
            BrVector3Set(&model->vertices[2].p, 0.f, (float) -pSize, -1.2f);
            BrVector3Set(&model->vertices[3].p, (float) pSize, (float) -pSize, -1.2f);
            BrVector2Set(&model->vertices[0].map, 0.f, 0.f);
            BrVector2Set(&model->vertices[1].map, 1.f, 0.f);
            BrVector2Set(&model->vertices[2].map, 0.f, 1.f);
            BrVector2Set(&model->vertices[3].map, 1.f, 1.f);
            SetPolyFontBorderColours(model, pIndex);
            model->flags &= ~(BR_MODF_KEEP_ORIGINAL | BR_MODF_UPDATEABLE);
            BrModelAdd(model);
        }
        gPolyFonts[pIndex].glyphs[ascii].model = model;
        tex_x += pSize;
        if (tex_x >= 64) {
            tex_y += pSize;
            tex_x = 0;
            if (tex_y >= 64) {
                gSize_font_texture_pages++;
                if (gSize_font_texture_pages >= REC2_ASIZE(gTextureMaps)) {
                    FatalError(kFatalError_CouldNotCreateTexturesPages_S, pName);
                }
                tex_x = 0;
                tex_y = 0;
            }
        }
        if (twt < 0) {
            BrPixelmapFree(map);
        }
    }
    for (i = 0; i < gPixelmapBufferSize; i++) {

        if (gPixelmapBuffer[i] != NULL) {
            BrPixelmapFree(gPixelmapBuffer[i]);
            gPixelmapBuffer[i] = NULL;
        }
    }
    gPixelmapBufferSize = 0;
    ClosePackFileAndSetTiffLoading(twt);
    gSize_font_texture_pages++;
}

static tU32 FindSmallestPowerOf2BiggerThen(tU32 pValue) {
    tU32 power;

    for (power = 1; power < 32; power++) {
        if (pValue <= 1u << power) {
            return 1 << power;
        }
    }
    return 1 << 31;
}

// FUNCTION: CARMA2_HW 0x004973b0
void C2_HOOK_FASTCALL InitCarIcons(br_pixelmap* pMap) {
    int i;
    int count_car_icons;
    int icon_width;
    int icon_height;
    int texture_x;
    int texture_y;

    C2_HOOK_BUG_ON(REC2_ASIZE(gCar_icons) != 128);
    memset(gCar_icons, 0, sizeof(gCar_icons));
    count_car_icons = gIcons_pix->height / gCurrent_graf_data->car_icon_height;

    icon_width = FindSmallestPowerOf2BiggerThen(pMap->width);
    icon_height = FindSmallestPowerOf2BiggerThen(gCurrent_graf_data->car_icon_height);

    texture_x = 0;
    texture_y = 0;
    for (i = 0; i < count_car_icons; i++) {
        int j;
        tCar_icon* icon = &gCar_icons[i];

        if (gTextureMaps[gSize_font_texture_pages] == NULL) {
            gTextureMaps[gSize_font_texture_pages] = BrPixelmapAllocate(pMap->type, 64, 64, NULL, 0);
            if (gTextureMaps[gSize_font_texture_pages] == NULL) {
                FatalError(kFatalError_CouldNotCreateTexturesPages_S, "Car Icons");
            }
            BrMapAdd(gTextureMaps[gSize_font_texture_pages]);
        }
        DRPixelmapRectangleCopy(
            gTextureMaps[gSize_font_texture_pages],
            texture_x,
            texture_y,
            pMap,
            0,
            gCurrent_graf_data->car_icon_height * i,
            icon_width,
            gCurrent_graf_data->car_icon_height);
        BrMapUpdate(gTextureMaps[gSize_font_texture_pages], BR_MAPU_ALL);
        icon->index = gSize_font_texture_pages;
        icon->model = CreateStringModel(icon_width, icon_height, texture_x, texture_y, "CAR ICONS");
        for (j = 0; j < 4; j++) {
            icon->model->vertices[j].red = 0xff;
            icon->model->vertices[j].grn = 0xff;
            icon->model->vertices[j].blu = 0xff;
        }
        icon->model->flags &= ~(BR_MODF_KEEP_ORIGINAL | BR_MODF_UPDATEABLE);
        BrModelUpdate(icon->model, BR_MODU_ALL);
        texture_x += icon_width;
        if (texture_x > 64) {
            texture_x = 0;
            texture_y += icon_height;
            if (texture_y >= 64) {
                gSize_font_texture_pages++;
                if (gSize_font_texture_pages >= REC2_ASIZE(gTextureMaps)) {
                    FatalError(kFatalError_CouldNotCreateTexturesPages_S, "CAR ICONS");
                }
                texture_x = 0;
                texture_y = 0;
            }
        }
    }
    gSize_font_texture_pages++;
    gCar_icons_model_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    gCar_icons_model_actor->material = BrMaterialAllocate("Car Icon Material");
    gCar_icons_model_actor->material->flags = BR_MATF_PRELIT | BR_MATF_SMOOTH | BR_MATF_ALWAYS_VISIBLE;
    BrMaterialAdd(gCar_icons_model_actor->material);
}

// FUNCTION: CARMA2_HW 0x00497610
void C2_HOOK_FASTCALL UpdateMapAndSaveOptions(void) {
    InitMap();
    SaveOptions();
}

// FUNCTION: CARMA2_HW 0x00463d40
void C2_HOOK_FASTCALL InitPolyFonts(void) {
    int i;
    tPath_name the_path;
    FILE* f;

    C2_HOOK_BUG_ON(REC2_ASIZE(gPolyFontMaterials) != 80);

    PrintMemoryDump(0, "START OF InitPolyFonts()");

    for (i = 0; i < REC2_ASIZE(gPolyFontMaterials); i++) {

        gPolyFontMaterials[i] = CreateFontCharacterMaterial(0);
    }
    C2_HOOK_BUG_ON(sizeof(gPolyFonts) != 196344);
    memset(&gPolyFonts, 0, sizeof(gPolyFonts));
    gString_root_actor = BrActorAllocate(BR_ACTOR_NONE, NULL);
    if (gString_root_actor == NULL) {
        FatalError(kFatalError_OOM_S, "");
    }
    gString_root_actor->identifier = "gString_root_actor";
    gString_root_actor->t.type = BR_TRANSFORM_TRANSLATION;
    BrVector3Set(&gString_root_actor->t.t.translate.t, 0.f, 0.f, 0.f);

    C2_HOOK_BUG_ON(REC2_ASIZE(gPolyfont_glyph_actors) != 256);
    for (i = 0; i < REC2_ASIZE(gPolyfont_glyph_actors); i++) {
        br_actor* actor;

        actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        gPolyfont_glyph_actors[i] = actor;
        if (actor == NULL) {
            FatalError(kFatalError_OOM_S, "");
        }
        actor->t.type = BR_TRANSFORM_TRANSLATION;
        BrVector3Set(&actor->t.t.translate.t, 0.f, 0.f, 0.f);
        actor->identifier = "character_actor";
    }

    PathCat(the_path, gApplication_path, "FontCol.TXT");
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

    C2_HOOK_BUG_ON(REC2_ASIZE(gPolyFontBorderColours) != 27);
    for (i = 0; i < REC2_ASIZE(gPolyFontBorderColours); i++) {
        tPolyFontBorderColours *border;

        border = &gPolyFontBorderColours[i];

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
    PFfclose(f);
#else
    fclose(f);
#endif
    InitCarIcons(gIcons_pix);

    LoadPolyFont("TINYFONT", 8, kPolyfont_ingame_tiny_yellow);
    LoadPolyFont("TINYFONT", 8, kPolyfont_ingame_tiny_blue);
    LoadPolyFont("TINYFONT", 8, kPolyfont_ingame_tiny_red);
    LoadPolyFont("TINYFONT", 8, kPolyfont_ingame_tiny_green);

    LoadPolyFont("BIGYELLOWTIMER", 32, kPolyfont_ingame_big_timer);
    for (i = 0; i < REC2_ASIZE(gPolyFonts->glyphs); i++) {
        gPolyFonts[kPolyfont_ingame_big_timer].glyphs[i].glyph_width = 27;
    }
    gPolyFonts[kPolyfont_ingame_big_timer].glyphs[58].glyph_width = 11;

    LoadPolyFont("MEDIUM_HEADUP", 16, kPolyfont_ingame_medium_red);
    LoadPolyFont("MEDIUM_HEADUP", 16, kPolyfont_ingame_medium_blue);
    LoadPolyFont("MEDIUM_HEADUP", 16, kPolyfont_ingame_medium_orange);
    LoadPolyFont("MEDIUM_HEADUP", 16, kPolyfont_ingame_medium_green);

    LoadPolyFont("BIG_ITALIC", 32, kPolyfont_ingame_italic_yellow);
    LoadPolyFont("BIG_ITALIC", 32, kPolyfont_ingame_italic_green);
    LoadPolyFont("BIG_ITALIC", 32, kPolyfont_ingame_italic_blue);
    LoadPolyFont("BIG_ITALIC", 32, kPolyfont_ingame_italic_red);

    LoadPolyFont("NETPOSFONT", 16, kPolyfont_netpos);
    gInitial_count_font_texture_pages = gSize_font_texture_pages;
    PrintMemoryDump(0, "END OF InitPolyFonts()");
}

// FUNCTION: CARMA2_HW 0x00464090
void C2_HOOK_FASTCALL LoadPolyFontWithTimerFix(int pFont, const char* pName, float pFactor, int pSize) {
    int i;

    LoadPolyFont(pName, pSize, pFont);
    if (pFont == kPolyfont_ingame_big_timer) {
        /* 0x10 -> kPolyfont_ingame_big_timer */
        for (i = 0; i < REC2_ASIZE(gPolyFonts[kPolyfont_ingame_big_timer].glyphs); i++) {
            gPolyFonts[kPolyfont_ingame_big_timer].glyphs[i].glyph_width = 27;
        }
        gPolyFonts[kPolyfont_ingame_big_timer].glyphs[58].glyph_width = 11; /* FIXME: What is gyph 58? */
    }
}

// FUNCTION: CARMA2_HW 0x004642d0
void C2_HOOK_FASTCALL LoadInterfacePolyFonts(void) {
    if (!gInterface_fonts_loaded) {
        gSize_font_texture_pages = gInitial_count_font_texture_pages;
        LoadPolyFont("HAND15U", 16, kPolyfont_hand_green_15pt_unlit);
        LoadPolyFont("HAND15", 16, kPolyfont_hand_green_15pt_lit);
        LoadPolyFont("HAND15U", 16, kPolyfont_hand_red_15pt_unlit);
        LoadPolyFont("HAND15", 16, kPolyfont_hand_red_15pt_lit);
        LoadPolyFont("HAND10U", 16, kPolyfont_hand_green_10pt_unlit);
        LoadPolyFont("HAND10", 16, kPolyfont_hand_green_10pt_lit);
        LoadPolyFont("SERP15", 16, kPolyfont_serp_red_15pt_lit);
        LoadPolyFont("SERP30", 32, kPolyfont_serp_red_30pt_lit);
        LoadPolyFont("SERP30U", 32, kPolyfont_serp_green_30pt_unlit);
        LoadPolyFont("SERP30", 32, kPolyfont_serp_green_30pt_lit);
        LoadPolyFont("SERP38U", 64, kPolyfont_serp_green_38pt_unlit);
        LoadPolyFontWithTimerFix(kPolyfont_serp_green_38pt_lit, "SERP38", 1.0f, 64);
        LoadPolyFontWithTimerFix(kPolyfont_highlighter, "HAND15lo", 1.0f, 16);
        gInterface_polyfont_texture_pages = gSize_font_texture_pages - gInitial_count_font_texture_pages;
        gInterface_fonts_loaded = 1;
    }
}

// FUNCTION: CARMA2_HW 0x004640d0
void C2_HOOK_FASTCALL CheckAvailabilityOfThisFont(int pFont) {

    if (!gPolyFonts[pFont].available) {
        LoadInterfacePolyFonts();
    }
}

// FUNCTION: CARMA2_HW 0x004640f0
void C2_HOOK_FASTCALL DisposeInterfaceFonts(void) {

    if (gInterface_fonts_loaded) {
        int i;

        ClearPolyFontGlyphs(kPolyfont_hand_green_15pt_unlit);
        ClearPolyFontGlyphs(kPolyfont_hand_green_15pt_lit);
        ClearPolyFontGlyphs(kPolyfont_hand_red_15pt_unlit);
        ClearPolyFontGlyphs(kPolyfont_hand_red_15pt_lit);
        ClearPolyFontGlyphs(kPolyfont_hand_green_10pt_unlit);
        ClearPolyFontGlyphs(kPolyfont_hand_green_10pt_lit);
        ClearPolyFontGlyphs(kPolyfont_serp_red_15pt_lit);
        ClearPolyFontGlyphs(kPolyfont_serp_red_30pt_lit);
        ClearPolyFontGlyphs(kPolyfont_serp_green_30pt_unlit);
        ClearPolyFontGlyphs(kPolyfont_serp_green_30pt_lit);
        ClearPolyFontGlyphs(kPolyfont_serp_green_38pt_unlit);
        ClearPolyFontGlyphs(kPolyfont_serp_green_38pt_lit);
        ClearPolyFontGlyphs(kPolyfont_highlighter);

        for (i = 0; i < gInterface_polyfont_texture_pages; i++) {
            int map_i = gInterface_polyfont_texture_pages + i;

            if (gTextureMaps[map_i] != NULL) {

                BrMapRemove(gTextureMaps[map_i]);
                BrPixelmapFree(gTextureMaps[map_i]);
                gTextureMaps[map_i] = NULL;
            }
        }
        gInterface_fonts_loaded = 0;
    }
}

// FUNCTION: CARMA2_HW 0x00464290
void C2_HOOK_FASTCALL ClearPolyFontGlyphs(int pFont) {
    int i;

    for (i = 0; i < REC2_ASIZE(gPolyFonts[pFont].glyphs); i++) {
        tPolyFontGlyph* glyph = &gPolyFonts[pFont].glyphs[i];

        if (glyph->used) {

            glyph->material = NULL;
            glyph->model = NULL;
            glyph->used = 0;
        }
    }
    gPolyFonts[pFont].available = 0;
}

// FUNCTION: CARMA2_HW 0x00465ca0
int C2_HOOK_FASTCALL PolyFontTextWidth(int pFont, const char* pText) {
    int len;
    int i;
    int result;

    CheckAvailabilityOfThisFont(pFont);
    len = strlen(pText);
    result = 0;
    for (i = 0; i < len; i++) {
        result += CharacterWidth(pFont, pText[i]);
    }
    if (len > 1) {
        result += (len - 1) * gPolyFonts[pFont].interCharacterSpacing;
    }
    return result;
}

// FUNCTION: CARMA2_HW 0x00464e40
void C2_HOOK_FASTCALL RenderPolyTextLine(const char *pText, int pX, int pY, int pFont, tJustification pJust, int pRender) {
    int text_len;
    int i;
    int draw_x;
    int draw_y;

    CheckAvailabilityOfThisFont(pFont);
    if (pRender) {
        RemovePolyFontActors();
    }
    switch (pJust) {
    case eJust_right:
        pX -= PolyFontTextWidth(pFont, pText);
        break;
    case eJust_centre:
        pX -= PolyFontTextWidth(pFont, pText) / 2;
        break;
    case eJust_left:
        break;
    }

    text_len = strlen(pText);
    if (gCount_polyfont_glyph_actors + text_len >= REC2_ASIZE(gPolyfont_glyph_actors)) {
        /* line is not drawn */
        /* FIXME: log debug message */
        return;
    }

    draw_x = pX;
    draw_y = pY;
    for (i = 0; i < text_len; i++) {
        tU8 c = pText[i];
        br_actor* actor;

        if (c == '\r') {
            draw_x = pX;
            draw_y += gPolyFonts[pFont].fontCharacterHeight;
            continue;
        }
        if (c >= 'a' && c <= 'z') {
            c -= 'a' - 'A';
        }
        if (!gPolyFonts[pFont].glyphs[c].used) {
            draw_x += gPolyFonts[pFont].widthOfBlank;
            continue;
        }
        actor = gPolyfont_glyph_actors[gCount_polyfont_glyph_actors];
        actor->model = gPolyFonts[pFont].glyphs[c].model;
        actor->material = GetPolyFontMaterial(pFont, c);
        BrActorAdd(gString_root_actor, actor);
        BrVector3Set(&actor->t.t.translate.t, (float)draw_x, (float)-draw_y, -1.1f);
        gCount_polyfont_glyph_actors++;
        draw_x += gPolyFonts[pFont].glyphs[c].glyph_width + gPolyFonts[pFont].interCharacterSpacing;
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
        RemovePolyFontActors();
    }
}

// FUNCTION: CARMA2_HW 0x00465380
void C2_HOOK_FASTCALL TransparentPolyFontText(const char* pText, int pX, int pY, int pFont, tJustification pJust, int pRender, double pBlend_factor) {
    int text_len;
    int i;
    int draw_x;
    int draw_y;
    br_token_value tvs[3];

    CheckAvailabilityOfThisFont(pFont);
    if (pRender) {
        RemovePolyFontActors();
    }
    switch (pJust) {
    case eJust_right:
        pX -= PolyFontTextWidth(pFont, pText);
        break;
    case eJust_centre:
        pX -= PolyFontTextWidth(pFont, pText) / 2;
        break;
    case eJust_left:
        break;
    }

    text_len = strlen(pText);
    if (gCount_polyfont_glyph_actors + text_len >= REC2_ASIZE(gPolyfont_glyph_actors)) {
        /* line is not drawn */
        /* FIXME: log debug message */
        return;
    }
    tvs[0].t = BRT_BLEND_B;
    tvs[0].v.b = 1;
    tvs[1].t = BRT_OPACITY_X;
    tvs[1].v.x = BR_FIXED_INT(pBlend_factor * 255.);
    tvs[2].t = BR_NULL_TOKEN;
    tvs[2].v.u32 = 0;

    draw_x = pX;
    draw_y = pY;
    for (i = 0; i < text_len; i++) {
        tU8 c = pText[i];
        br_actor* actor;

        if (c == '\r') {
            draw_x = pX;
            draw_y += gPolyFonts[pFont].fontCharacterHeight;
            continue;
        }
        if (c >= 'a' && c <= 'z') {
            c -= 'a' - 'A';
        }
        if (!gPolyFonts[pFont].glyphs[c].used) {
            draw_x += gPolyFonts[pFont].widthOfBlank;
            continue;
        }
        actor = gPolyfont_glyph_actors[gCount_polyfont_glyph_actors];
        actor->model = gPolyFonts[pFont].glyphs[c].model;
        actor->material = GetPolyFontMaterial(pFont, c);
        actor->material->extra_prim = tvs;
        BrMaterialUpdate(actor->material, BR_MATU_ALL);
        BrActorAdd(gString_root_actor, actor);
        BrVector3Set(&actor->t.t.translate.t, (float)draw_x, (float)-draw_y, -1.1f);
        gCount_polyfont_glyph_actors++;
        draw_x += gPolyFonts[pFont].glyphs[c].glyph_width + gPolyFonts[pFont].interCharacterSpacing;
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
        RemovePolyFontActors();
    }
}

// FUNCTION: CARMA2_HW 0x00470a50
void C2_HOOK_FASTCALL PolyClipName(char *pText, int pFont, int pWidth) {

    while (PolyFontTextWidth(pFont, pText) > pWidth) {
        pText[strlen(pText) + -1] = '\0';
    }
}
