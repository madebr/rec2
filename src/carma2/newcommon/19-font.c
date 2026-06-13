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


// PolyFontHeight

// FindCharacterWidth

// CharacterWidth

// GetSpacing

// SolidPolyFontTextInABox

// PolyFontTextInABox

// TransparentPolyFontTextInABox

// InitPolyFonts

// CreatePolyFont

// GetThisFuckingPixelmap

// KillThePixies

// CheckAvailabilityOfThisFont

// DisposeInterfaceFonts

// RemovePolyFont

// LoadInterfaceFonts

// LoadPolyFont

// CreateFontCharacterModel

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

// CreatePolyMaterial

// ColourVertices

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

