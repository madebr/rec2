#include "19-font.h"

#include "08-loading1.h"
#include "41-utility.h"
#include "52-errors.h"
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


// PolyFontHeight

// FUNCTION: CARMA2_HW 0x00463760
int C2_HOOK_FASTCALL FindCharacterWidth(br_pixelmap* pMap) {
    int y;
    int x;
    // void* column_pointer;
    tU16* pixel;
    int height;

    height = pMap->height;
    x = pMap->width - 1;
    for (; x >= 0; x--) {
        // column_pointer =
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
    if (font->glyphs[pCharacter].used) {
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

// SolidPolyFontTextInABox

// PolyFontTextInABox

// TransparentPolyFontTextInABox

// InitPolyFonts

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
    int i;

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

// DisposeInterfaceFonts

// RemovePolyFont

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

// SolidPolyFontText

// InitPolyFontMaterials

// GetPolyFontMaterial

// PolyFontText

// TransparentPolyFontText

// DrawDigitAt

// DrawNumberAt

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

// DRPixelmapText

// DRPixelmapCleverText

// PolyFontTextWidth

// DRTextWidth

// DRTextCleverWidth

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

// GetPolyFontIndexToReplaceDRfontWith

