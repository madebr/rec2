#include "41-utility.h"

#include "01-network.h"
#include "02-init.h"
#include "28-world3.h"
#include "18-graphics2.h"
#include "40-main.h"
#include "42-input.h"
#include "44-mainmenu.h"
#include "52-errors.h"
#include "63-loading3.h"
#include "69-sound.h"
#include "70-packfile.h"
#include "globvars.h"
#include "platform.h"
#include "rec2_macros.h"

#include "c2_string.h"

#include <ctype.h>


// GLOBAL: CARMA2_HW 0x006abef8
tU32 last_service;

// GLOBAL: CARMA2_HW 0x006b5f40
char* gMisc_strings[300];

// GLOBAL: CARMA2_HW 0x0074ca04
tMaterial_exception* gMaterial_exceptions;

// FUNCTION: CARMA2_HW 0x00513400
br_error C2_HOOK_FASTCALL DRBrEnd(void) {
    br_device *dev;

    if (!gBr_initialized) {
        return 0x1006;
    }

    _BrEndHook();
    gBr_initialized = 0;
    while (BrDevFind(&dev, NULL) == 0) {
        if (dev != NULL) {
            BrDevRemove(dev);
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00513460
void C2_HOOK_FASTCALL Uppercaseificate(char* dest, const char* src) {
    int len;
    int i;

    len = (int)strlen(src);
    for (i = 0; i < (int)len; i++) {
        dest[i] = toupper(src[i]);
    }
    dest[len] = '\0';
}

// FUNCTION: CARMA2_HW 0x005134b0
int C2_HOOK_FASTCALL CheckQuit(void) {
    int result;

    // GLOBAL: CARMA2_HW 0x006abee0
    static int active;

    result = 0;
    if (!active) {
        if (KeyIsDown(1) && KeyIsDown(7)) {
            active = 1;
            do {
            } while (AnyKeyDown());
            result = 1;
            if (DoVerifyQuit(1)) {
                QuitGame();
            }
            active = 0;
        }
    }
    return result;
}

// FUNCTION: CARMA2_HW 0x00513510
double C2_HOOK_FASTCALL sqr(double pN) {

    return pN * pN;
}

// FUNCTION: CARMA2_HW 0x00513520
int C2_HOOK_FASTCALL IRandomBetween(int pA, int pB) {
    int num;

#if RAND_MAX == 0x7fff
    num = (pB + 1 - pA) * rand() / (RAND_MAX + 1) + pA;
    return num;
#else
    num = (pB + 1 - pA) * (rand() % 0x8000) / (0x7fff + 1) + pA;
    return num;
#endif
}

// FUNCTION: CARMA2_HW 0x00513550
int C2_HOOK_FASTCALL PercentageChance(int pC) {

    if (pC == 0) {
        return 0;
    }
    return IRandomBetween(0, 99) < pC;
}

// FUNCTION: CARMA2_HW 0x00513580
int C2_HOOK_FASTCALL IRandomPosNeg(int pN) {

    return IRandomBetween(-pN, pN);
}

// FUNCTION: CARMA2_HW 0x005135b0
float C2_HOOK_STDCALL FRandomBetween(float pA, float pB) {

#if RAND_MAX == 0x7fff
    return (float)rand() * (pB - pA) / (float)RAND_MAX + pA;
#else
    return (float)(rand() % 0x10000) * (pB - pA) / (float)(0x7fff + 1) + pA;
#endif
}

// FUNCTION: CARMA2_HW 0x005135e0
float C2_HOOK_STDCALL FRandomPosNeg(float pN) {

    return FRandomBetween(-pN, pN);
}

// FUNCTION: CARMA2_HW 0x00513620
br_scalar C2_HOOK_STDCALL SRandomBetween(br_scalar pA, br_scalar pB) {

    return FRandomBetween(pA, pB);
}

// FUNCTION: CARMA2_HW 0x00513650
br_scalar C2_HOOK_STDCALL SRandomPosNeg(br_scalar pN) {

    return SRandomBetween(-pN, pN);
}

// FUNCTION: CARMA2_HW 0x00513690
void C2_HOOK_FASTCALL PathCat(char* pDestn_str, const char* pStr_1, const char* pStr_2) {

#ifdef REC2_FIX_BUGS
    if (pDestn_str != pStr_1) { // Added to avoid strcpy overlap checks
        strcpy(pDestn_str, pStr_1);
    }
#else
    strcpy(pDestn_str, pStr_1);
#endif

    if (strlen(pStr_2) != 0) {
        strcat(pDestn_str, gDir_separator);
        strcat(pDestn_str, pStr_2);
    }
}

// tandeg

// FUNCTION: CARMA2_HW 0x00513790
tU32 C2_HOOK_FASTCALL GetFileLength(FILE* pF) {
    tU32 the_size;

    PFfseek(pF, 0, SEEK_END);
    the_size = PFftell(pF);
    PFrewind(pF);
    return the_size;
}

// FUNCTION: CARMA2_HW 0x005137d0
br_pixelmap* C2_HOOK_FASTCALL DRPixelmapAllocate(br_uint_8 pType, br_uint_16 pW, br_uint_16 pH, void* pPixels, int pFlags) {
    br_pixelmap* pm;

    pm = BrPixelmapAllocate(pType, pW, pH, pPixels, pFlags);
    if (pm != NULL) {
        pm->origin_x = 0;
        pm->origin_y = 0;
    }
    return pm;
}

// FUNCTION: CARMA2_HW 0x00513820
br_pixelmap* C2_HOOK_FASTCALL DRPixelmapAllocateSub(br_pixelmap* pPm, br_uint_16 pX, br_uint_16 pY, br_uint_16 pW, br_uint_16 pH) {
    br_pixelmap* the_map;

    the_map = BrPixelmapAllocateSub(pPm, pX, pY, pW, pH);
    if (the_map != NULL) {
        the_map->origin_x = 0;
        the_map->origin_y = 0;
    }
    return the_map;
}

// DRImageLoad

// DRPixelmapLoad

// FUNCTION: CARMA2_HW 0x005139a0
void C2_HOOK_FASTCALL SepDirAndFilename(const char* path, char* dirPath, char* stemPath) {
    size_t pathLen;
    size_t dirLen;
    size_t i;

    pathLen = strlen(path);
    dirLen = 0;
    for (i = pathLen - 1; ; i--) {
        if (i == 0) {
            break;
        }
        if (path[i] == gDir_separator[0]) {
            dirLen = i;
            break;
        }
    }
    memcpy(dirPath, path, dirLen);
    dirPath[dirLen] = '\0';
    if (*dirPath != '\0') {
        dirLen += 1;
    }
    for (i = 0; path[dirLen + i] != '.' && dirLen + i != pathLen; i++) {
        stemPath[i] = path[dirLen + i];
    }
    stemPath[i] = '\0';
}

// DRLoadMultiplePix

// FUNCTION: CARMA2_HW 0x00514570
int C2_HOOK_FASTCALL DRPixelmapLoadMany(const char* texturePathNoExt, br_pixelmap** pixelmaps, size_t capacity) {
    tPath_name texturePath;
    tPath_name texturePathDir;
    tPath_name texturePathStem;
    int errorCode;

    strcpy(texturePath, texturePathNoExt);
    strcat(texturePath, ".TIF");
    SepDirAndFilename(texturePath, texturePathDir, texturePathStem);
    pixelmaps[0] = DRLdImg(texturePathDir, texturePathStem, gRender_palette, gPixelFlags, &errorCode);
    if (pixelmaps[0] == NULL || errorCode != 0) {
        return 0;
    }
    return 1;
}

// WaitFor

// FUNCTION: CARMA2_HW 0x005146f0
intptr_t C2_HOOK_FASTCALL DRActorEnumRecurse(br_actor* pActor, br_actor_enum_cbfn* callback, void* arg) {
    intptr_t result;

    result = callback(pActor, arg);
    if (result != 0) {
        return result;
    }
    for (pActor = pActor->children; pActor != NULL; pActor = pActor->next) {
        result = DRActorEnumRecurse(pActor, callback, arg);
        if (result != 0) {
            return result;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x005147b0
intptr_t C2_HOOK_CDECL CompareActorID(br_actor* pActor, void* pArg) {

    if (pActor->identifier != NULL && strcmp(pActor->identifier, (const char*)pArg) == 0) {
        return (intptr_t)pActor;
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x00514730
br_actor* C2_HOOK_FASTCALL DRActorFindRecurse(br_actor* pSearch_root, const char* pName) {

    return (br_actor*)DRActorEnumRecurse(pSearch_root, CompareActorID, (void*)pName);
}

// FUNCTION: CARMA2_HW 0x00514800
br_uint_32 C2_HOOK_FASTCALL DRActorEnumRecurseWithMat(br_actor* pActor, br_material* pMat, recurse_with_mat_cbfn* pCall_back, void* pArg) {
    br_uint_32 result;

    if (pActor->material != NULL) {
        pMat = pActor->material;
    }
    result = pCall_back(pActor, pMat, pArg);
    if (result != 0) {
        return result;
    }
    for (pActor = pActor->children; pActor != NULL; pActor = pActor->next) {
        result = DRActorEnumRecurseWithMat(pActor, pMat, pCall_back, pArg);
        if (result != 0) {
            return result;
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x00514850
br_uint_32 C2_HOOK_FASTCALL DRActorEnumRecurseWithTrans(br_actor* pActor, br_matrix34* pMatrix, recurse_with_trans_cbfn* pCall_back, void* pArg) {
    br_uint_32 result;
    br_matrix34 combined_transform;

    if (pMatrix == NULL) {
        BrMatrix34Copy(&combined_transform, &pActor->t.t.mat);
    } else {
        BrMatrix34Mul(&combined_transform, pMatrix, &pActor->t.t.mat);
    }
    result = pCall_back(pActor, &combined_transform, pArg);
    if (result != 0) {
        return result;
    }
    for (pActor = pActor->children; pActor != NULL; pActor = pActor->next) {
        result = DRActorEnumRecurseWithTrans(pActor, &combined_transform, pCall_back, pArg);
        if (result != 0) {
            return result;
        }
    }
    return 0;
}

// DRActorEnumRecurseWithSnart

// sign

// OpenUniqueFileB

// PrintScreenFile

// PrintScreenFile16

// GetTotalTime

// GetRaceTime

// AddLostTime

// AssertThisTimeAsCurrentTime

// TimerString

// FUNCTION: CARMA2_HW 0x00514d70
const char* C2_HOOK_FASTCALL GetMiscString(int pIndex) {

    return gMisc_strings[pIndex];
}

// Flash

// RGBDifferenceSqr

// FindBestMatch

// BuildShadeTablePath

// LoadGeneratedShadeTable

// SaveGeneratedShadeTable

// GenerateShadeTable

// GenerateDarkenedShadeTable

// FUNCTION: CARMA2_HW 0x005155d0
void C2_HOOK_FASTCALL PossibleService(void) {
    tU32 time;

    time = PDGetTotalTime();
    if (time - last_service > 200 && !gProgram_state.racing) {
        SoundService();
        NetService(gProgram_state.racing);
        last_service = time;
    }
}

// FUNCTION: CARMA2_HW 0x00515610
void C2_HOOK_FASTCALL DRMatrix34TApplyP(br_vector3* pA, const br_vector3* pB, const br_matrix34* pC) {
    br_scalar t1;
    br_scalar t2;
    br_scalar t3;

    t1 = pB->v[0] - pC->m[3][0];
    t2 = pB->v[1] - pC->m[3][1];
    t3 = pB->v[2] - pC->m[3][2];

    pA->v[0] = pC->m[0][2] * t3 + pC->m[0][0] * t1 + pC->m[0][1] * t2;
    pA->v[1] = pC->m[1][2] * t3 + pC->m[1][0] * t1 + pC->m[1][1] * t2;
    pA->v[2] = pC->m[2][2] * t3 + pC->m[2][0] * t1 + pC->m[2][1] * t2;
}

// DRPixelmapRectangleCopy

// NormalSideOfPlane

// FUNCTION: CARMA2_HW 0x00515780
br_material* C2_HOOK_FASTCALL DRMaterialClone(br_material* pMaterial, int pSet_identifier) {
    br_material* the_material;
    char s[256];
    int version;

    // GLOBAL: CARMA2_HW 0x006abefc
    static int gVersion_suffix;

    the_material = BrMaterialAllocate(NULL);
    the_material->flags = pMaterial->flags;
    the_material->ka = pMaterial->ka;
    the_material->kd = pMaterial->kd;
    the_material->ks = pMaterial->ks;
    the_material->power = pMaterial->power;
    the_material->colour = pMaterial->colour;
    the_material->index_base = pMaterial->index_base;
    the_material->index_range = pMaterial->index_range;
    the_material->index_shade = pMaterial->index_shade;
    the_material->index_blend = pMaterial->index_blend;
    the_material->colour_map = pMaterial->colour_map;
    memcpy(&the_material->map_transform, &pMaterial->map_transform, sizeof(the_material->map_transform));
    if (pSet_identifier) {
        version = gVersion_suffix++;
        sprintf(s, "%s(%d)", pMaterial->identifier, version);
        the_material->identifier = BrResAllocate(the_material, strlen(s) + 1, BR_MEMORY_STRING);
        strcpy(the_material->identifier, s);
    }
    BrMaterialAdd(the_material);
    return the_material;
}

// FUNCTION: CARMA2_HW 0x00515870
int C2_HOOK_FASTCALL DRStricmp(const char* p1, const char* p2) {
    int val;

    val = tolower(*p1);
    val -= tolower(*p2);
    while (val == 0) {
        if (*p1++ == '\0') {
            break;
        }
        if (*p2++ == '\0') {
            break;
        }
        val = tolower(*p1);
        val -= tolower(*p2);
    }
    return val;
}

// FUNCTION: CARMA2_HW 0x005158d0
void C2_HOOK_FASTCALL DRstrlwr(char* s) {
    int l = (int)strlen(s);
    int i;

    for (i = 0; i < l; i++) {
        s[i] = toupper(s[i]);
    }
}

// FUNCTION: CARMA2_HW 0x00515950
int C2_HOOK_FASTCALL PDCheckDriveExists(const char* pThe_path) {

    return PDCheckDriveExists2(pThe_path, NULL, 0);
}

br_actor* C2_HOOK_FASTCALL CloneActor(br_actor* pActor) {
    br_actor *clone;
    br_actor *child;

    clone = BrActorAllocate(pActor->type, pActor->type_data);
    clone->model = pActor->model;
    clone->material = pActor->material;
    if (pActor->identifier != NULL) {
        if (clone->identifier != NULL) {
            BrResFree(clone->identifier);
        }
        clone->identifier = BrResStrDup(clone, pActor->identifier);;
    }
    clone->t = pActor->t;
    for (child = pActor->children; child != NULL; child = child->next) {
        BrActorAdd(clone, CloneActor(child));
    }
    return clone;
}

// FUNCTION: CARMA2_HW 0x00515b80
void C2_HOOK_FASTCALL CalcActorGlobalPos(br_vector3* pResult, br_actor* pActor) {
    br_vector3 tv;

    BrVector3Set(pResult, 0.0f, 0.0f, 0.0f);
    for (;pActor != NULL && pActor != gNon_track_actor; pActor = pActor->parent) {
        if (pActor->t.t.mat.m[0][0] == 1.0f) {
            BrVector3Accumulate(pResult, &pActor->t.t.translate.t);
        } else {
            BrMatrix34ApplyP(&tv, pResult, &pActor->t.t.mat);
            BrVector3Accumulate(pResult, &tv);
        }
    }
}

// FUNCTION: CARMA2_HW 0x00515c20
float C2_HOOK_STDCALL frac(float pN) {

    return pN - (float)(int)pN;
}

// FUNCTION: CARMA2_HW 0x00515d90
intptr_t C2_HOOK_CDECL FindMaterialCB(br_actor* pActor, void* data) {
    const char* name = data;
    br_model* model = pActor->model;
    br_face *face;
    int face_i;

    if (model != NULL) {

        face = model->faces;
        for (face_i = 0; face_i < model->nfaces; face_i++, face++) {

            if (face->material != NULL
                    && face->material->identifier != NULL
                    && strcmp(face->material->identifier, name) == 0) {
                return (intptr_t)face->material;
            }
        }
    }
    return (intptr_t)NULL;
}

// FUNCTION: CARMA2_HW 0x00515c40
br_material* C2_HOOK_FASTCALL FindMaterial(const char* pName, br_actor* pActor, int pRecursive) {

    if (pRecursive) {
        return (br_material*)DRActorEnumRecurse(pActor, FindMaterialCB, (void*)pName);
    } else {
        return (br_material*)FindMaterialCB(pActor, (void*)pName);
    }
}

void C2_HOOK_FASTCALL BlendifyMaterialTablishly(br_material* pMaterial, int pPercent) {
    char* s;

    if (pPercent != 0 && pPercent != 100) {
        switch (pPercent) {
        case 25:
            s = "BLEND25.TAB";
            break;
        case 50:
            s = "BLEND50.TAB";
            break;
        case 75:
            s = "BLEND75.TAB";
            break;
        default:
            return;
        }
        pMaterial->index_blend = BrTableFind(s);
        if (pMaterial->index_blend == NULL) {
            pMaterial->index_blend = LoadSingleShadeTable(&gTrack_storage_space, s);
        }
    } else {
        pMaterial->index_blend = NULL;
    }
}

void C2_HOOK_FASTCALL BlendifyMaterialPrimitively(br_material* pMaterial, int pPercent) {

    // GLOBAL: CARMA2_HW 0x00661688
    static br_token_value alpha25[] = {
        { BRT_BLEND_B, { /*.b = */ 1 } },
        { BRT_OPACITY_X, { /*.x = */ BR_FIXED_INT(0x40) } },
        { 0 },
    };
    // GLOBAL: CARMA2_HW 0x006616a0
    static br_token_value alpha50[] = {
        { BRT_BLEND_B, { /*.b = */ 1 } },
        { BRT_OPACITY_X, { /*.x = */ BR_FIXED_INT(0x80) } },
        { 0 },
    };
    // GLOBAL: CARMA2_HW 0x006616b8
    static br_token_value alpha75[] = {
        { BRT_BLEND_B, { /*.b = */ 1 } },
        { BRT_OPACITY_X, { /*.x = */ BR_FIXED_INT(0xc0) } },
        { 0 },
    };

    switch (pPercent) {
    case 25:
        pMaterial->extra_prim = alpha25;
        break;
    case 50:
        pMaterial->extra_prim = alpha50;
        break;
    case 75:
        pMaterial->extra_prim = alpha75;
        break;
    case 0:
    case 100:
        pMaterial->extra_prim = NULL;
        break;
    }
}

// FUNCTION: CARMA2_HW 0x00515e70
void C2_HOOK_FASTCALL BlendifyMaterial(br_material* pMaterial, int pPercent) {
    br_uint_8 pixel_format = gScreen->type;

    if (pixel_format == BR_PMT_INDEX_8) {
        BlendifyMaterialTablishly(pMaterial, pPercent);
    } else {
        BlendifyMaterialPrimitively(pMaterial, pPercent);
    }
}

// FUNCTION: CARMA2_HW 0x00515fa0
void C2_HOOK_FASTCALL DRModelUpdateAndKevificateMaterials(br_model* pModel, br_uint_16 pFlags) {
    int i;
    v11group* v11g;

    C2_HOOK_BUG_ON(sizeof(v11group) != 0x24);

    if (pModel->nvertices != 0 && pModel->nfaces != 0) {

        BrModelUpdate(pModel, pFlags);
        for (i = 0; i < V11MODEL(pModel)->ngroups; i++) {

            v11g = &V11MODEL(pModel)->groups[i];
            *v11g->face_colours.materials = pModel->faces[*v11g->face_user].material;
        }
    }
}

// DRModelUpdateDeluxTurbo

// DistanceFromFace

// DRBoundsCopy

// TestForNan

// DRVector3TestForNan

// DRScaleModel

// DistanceFromFaceND

// DRVector3NonZero

// DRVector3Diminish

// DRScalarToU16

// DRU16ToScalar

// CompressVector3

// ExpandVector3

// CompressMatrix34

// ExpandMatrix34

// PossibleLock

// STUB: CARMA2_HW 0x00516c30
int C2_HOOK_FASTCALL PossibleUnlock(int pValue) {
    NOT_IMPLEMENTED();
    return 0;
}

// FUNCTION: CARMA2_HW 0x00516fd0
tU16 C2_HOOK_FASTCALL PaletteEntry16Bit(br_pixelmap* pPal, int pEntry) {
    tU32* src_entry;
    int red;
    int green;
    int blue;

    src_entry = pPal->pixels;
    switch (gBack_screen->type) {
    default:
        BrFailure("Unsupported back buffer type.");
        return 0;
    case BR_PMT_RGB_565:
        red = (src_entry[pEntry] >> 8) & 0xf800;
        green = (src_entry[pEntry] >> 5) & 0x07e0;
        blue = (src_entry[pEntry] >> 3) & 0x001f;
        return red | green | blue;
    case BR_PMT_RGB_555:
        red = (src_entry[pEntry] >> 9) & 0x7c00;
        green = (src_entry[pEntry] >> 6) & 0x03e0;
        blue = (src_entry[pEntry] >> 3) & 0x001f;
        return red | green | blue;
    }
}

// Colour24BitTo16Bit

// PaletteOf16Bits

// Copy8BitTo16Bit

// DRPixelmapCopy

static tMaterial_exception* C2_HOOK_FASTCALL FindExceptionInList(const char* pIdentifier, tMaterial_exception* pList) {

    for (; pList != NULL; pList = pList->next) {
        if (DRStricmp(pIdentifier, pList->texture_name) == 0) {
            break;
        }
    }
    return pList;
}

// FUNCTION: CARMA2_HW 0x00517fa0
void C2_HOOK_FASTCALL NobbleNonzeroBlacks(br_pixelmap* pPalette) {
    int modified;
    int i;
    br_colour *pixels;
    br_colour c;
    int r;
    int g;
    int b;

    pixels= pPalette->pixels;
    modified = 0;
    if (*pixels != BR_COLOUR_RGBA(0, 0, 0, 0)) {
        *pixels = BR_COLOUR_RGBA(0, 0, 0, 0);
        modified = 1;
    }
    pixels++;
    for (i = 0; i < 255; i++ ) {
        c = *pixels;
        r = BR_COLOUR_RED(c);
        g = BR_COLOUR_GRN(c);
        b = BR_COLOUR_BLU(c);
        if (r == 0 && g == 0 && b == 0) {
            *pixels = BR_COLOUR_RGB(1, 1, 1);
            modified = 1;
        }
        pixels++;
    }
    if (modified) {
        BrMapUpdate(pPalette, BR_MAPU_ALL);
    }
}

// FUNCTION: CARMA2_HW 0x005182f0
void C2_HOOK_FASTCALL GlorifyMaterial(br_material** pMaterials, int pCount, tRendererShadingType pShading) {
    int i;
    tMaterial_exception *material_exception;

    for (i = 0; i < pCount; i++) {

        if (pMaterials[i]->colour_map != NULL) {
            material_exception = FindExceptionInList(pMaterials[i]->colour_map->identifier, gMaterial_exceptions);
            if (gEnable_texture_interpolation && (material_exception == NULL || !(material_exception->flags & 0x1))) {
                pMaterials[i]->flags |= BR_MATF_MAP_INTERPOLATION;
            }
            if (gEnable_texture_interpolation && material_exception != NULL && material_exception->flags & 0x8) {
                pMaterials[i]->map_transform.m[2][0] = 0.02f;
                pMaterials[i]->map_transform.m[2][1] = 0.02f;
            }
            if (gEnable_texture_antialiasing) {
                pMaterials[i]->flags |= BR_MATF_MAP_ANTIALIASING;
            }
            if (gEnable_perspective_maps) {
                pMaterials[i]->flags |= BR_MATF_PERSPECTIVE;
            }
        }
        switch (pShading) {
        case kRendererShadingType_AmbientOnly:
            pMaterials[i]->ka = 1.0f;
            pMaterials[i]->kd = 0.0f;
            pMaterials[i]->ks = 0.0f;
            pMaterials[i]->flags &= ~BR_MATF_PRELIT;
            pMaterials[i]->flags |= BR_MATF_LIGHT;
            pMaterials[i]->flags |= BR_MATF_SMOOTH;
            break;
        case kRendererShadingType_Diffuse1:
            pMaterials[i]->ka = 0.4f;
            pMaterials[i]->kd = 0.8f;
            pMaterials[i]->ks = 0.0f;
            pMaterials[i]->flags &= ~BR_MATF_PRELIT;
            pMaterials[i]->flags |= BR_MATF_LIGHT;
            pMaterials[i]->flags |= BR_MATF_SMOOTH;
            break;
        case kRendererShadingType_Specular:
            pMaterials[i]->ka = 0.6f;
            pMaterials[i]->kd = 0.2f;
            pMaterials[i]->ks = 0.8f;
            pMaterials[i]->flags &= ~BR_MATF_PRELIT;
            pMaterials[i]->flags |= BR_MATF_LIGHT;
            pMaterials[i]->flags |= BR_MATF_SMOOTH;
            break;
        case kRendererShadingType_Default:
            pMaterials[i]->ka = 0.2f;
            pMaterials[i]->kd = 0.8f;
            pMaterials[i]->ks = 0.0f;
            pMaterials[i]->flags &= ~BR_MATF_PRELIT;
            pMaterials[i]->flags |= BR_MATF_LIGHT;
            pMaterials[i]->flags |= BR_MATF_SMOOTH;
            break;
        default:
            pMaterials[i]->ka = 1.0f;
            pMaterials[i]->kd = 0.0f;
            pMaterials[i]->ks = 0.0f;
            break;
        }
    }
}

// FindBestColourMatch

// FUNCTION: CARMA2_HW 0x00518690
void C2_HOOK_FASTCALL WhitenVertexRGB(br_model** pModels, int pCount) {
    int i;
    br_vertex* vertex;

    if (gScreen == NULL) {
        return;
    }
    if (gScreen->type == BR_PMT_INDEX_8) {
        return;
    }
    for (i = 0; i < pCount; i++) {
        int j;

        for (j = 0; j < pModels[i]->nvertices; j++) {
            vertex = &pModels[i]->vertices[j];

            vertex->red = 0xff;
            vertex->grn = 0xff;
            vertex->blu = 0xff;
        }
    }
}

// ArenaOpenFile

// BRPM_convert

// PrintScreen

// FudgeBRenderIntoTheNinetiesWithSomeProperFuckingColourSupport

// GetBlenficatiousnessOfMaterialTablishly

// GetBlenficatiousnessOfMaterialPrimitively

// GetBlenficatiousnessOfMaterial

// FUNCTION: CARMA2_HW 0x00518f20
char* C2_HOOK_FASTCALL MungeCommas(int pValue) {
    // GLOBAL: CARMA2_HW 0x006b5f20
    static char result_buffer[32];
    char buffer[32];
    int len;
    int get_pos;
    int put_pos;
    int remaining;

    sprintf(buffer, "%i", pValue);
    len = (int)strlen(buffer);
    for (get_pos = 0, put_pos = 0, remaining = len; get_pos < len; get_pos++, put_pos++, remaining--) {

        if (remaining % 3 == 0 && get_pos != 0) {
            result_buffer[put_pos] = gMisc_strings[294][0];
            put_pos++;
        }
        result_buffer[put_pos] = buffer[get_pos];
    }
    result_buffer[put_pos] = '\0';
    return result_buffer;
}

// FUNCTION: CARMA2_HW 0x00518fa0
void C2_HOOK_FASTCALL MungeMetaCharacters(char* pText, char pMeta, const char* pRepl) {
    int len_text;
    int len_repl;
    int i;

    len_text = strlen(pText);
    len_repl = strlen(pRepl);

    for (i = 0; i < len_text; i++) {
        if (pText[i] == '@' && pText[i + 1] == pMeta) {
            memmove(&pText[i + len_repl], &pText[i + 2], len_text - i - 1);
            memcpy(&pText[i], pRepl, len_repl);
            i += len_repl;
            len_text += len_repl - 2;
        }
    }
}

// FUNCTION: CARMA2_HW 0x00519040
void C2_HOOK_FASTCALL MungeMetaCharactersChar(char* pText, char pMeta, char pChar) {
    char repl[2];

    repl[1] = '\0';
    repl[0] = pChar;
    MungeMetaCharacters(pText, pMeta, repl);
}

// FUNCTION: CARMA2_HW 0x005190f0
void C2_HOOK_FASTCALL MungeMetaCharactersNum(char* pText, char pMeta, int pNum) {
    char text[16];

    sprintf(text, "%d", pNum);
    MungeMetaCharacters(pText, pMeta, text);
}

void C2_HOOK_FASTCALL DrPixelmapRectangleCopyPossibleLock(br_pixelmap* dst, br_int_32 dx, br_int_32 dy, br_pixelmap* src, br_int_32 sx, br_int_32 sy, br_int_32 w, br_int_32 h) {

    if (gLock_often) {
        PossibleUnlock(0);
    }
    BrPixelmapRectangleCopy(dst, dx, dy, src, sx, sy, w, h);
    if (gLock_often) {
        PossibleUnlock(0);
    }
}

// FUNCTION: CARMA2_HW 0x005191f0
void C2_HOOK_FASTCALL PixelmapSwapByteOrder(br_pixelmap* pMap) {

#if 0 // FIXME: introduce endian.h-like rec2_endian.h header
    br_uint_16 y;
    br_uint_8* row_ptr;
    br_colour* ptr;
    int w;

    row_ptr = pMap->pixels;
    for (y = 0; y < pMap->height; y++) {
        w = (pMap->width * 2) / sizeof(br_colour);
        ptr = (br_colour*) row_ptr;
        while (w) {
            *ptr = (*ptr >> 0x18) + (*ptr << 0x18) + ((*ptr >> 0x8) & 0xff00) + ((*ptr & 0xff00) <<0x8);
            ptr++;
        }
        row_ptr += pMap->row_bytes;
    }
#endif
}

// FUNCTION: CARMA2_HW 0x005193f0
void C2_HOOK_FASTCALL EnsurePixelmapAllowed(br_pixelmap* pMap, undefined4 pArg2) {

}

