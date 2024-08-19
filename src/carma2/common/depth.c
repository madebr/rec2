#include "depth.h"

#include "errors.h"
#include "globvars.h"
#include "globvrkm.h"
#include "loading.h"
#include "spark.h"
#include "utility.h"

#include "rec2_macros.h"

C2_HOOK_VARIABLE_IMPLEMENT(tSpecial_volume*, gDAT_0079ec44, 0x0079ec44);

C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gSky_on, 0x00591188, 1);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(int, gDepth_cueing_on, 0x0059118c, 1);

C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gDepth_shade_table, 0x0079ec20);
C2_HOOK_VARIABLE_IMPLEMENT(int, gDepth_shade_table_power, 0x0067c4bc);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gFog_shade_table, 0x0079ec38);
C2_HOOK_VARIABLE_IMPLEMENT(int, gFog_shade_table_power, 0x0067c4a4);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gAcid_shade_table, 0x0079ec24);
C2_HOOK_VARIABLE_IMPLEMENT(int, gAcid_shade_table_power, 0x0067c4dc);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gWater_shade_table, 0x0079ec28);
C2_HOOK_VARIABLE_IMPLEMENT(int, gWater_shade_table_power, 0x0067c4cc);

C2_HOOK_VARIABLE_IMPLEMENT(br_material*, gHorizon_material, 0x0067c4e0);
C2_HOOK_VARIABLE_IMPLEMENT(br_actor*, gSky_actor, 0x0067c4d8);
C2_HOOK_VARIABLE_IMPLEMENT(br_model*, gSky_model, 0x0067c4a0);
C2_HOOK_VARIABLE_IMPLEMENT_INIT(tDepth_effect_type, gSwap_depth_effect_type, 0x00591190, eDepth_effect_none);
C2_HOOK_VARIABLE_IMPLEMENT(br_pixelmap*, gSky_texture_0079ec1c, 0x0079ec1c);


void (C2_HOOK_FASTCALL * InstantDepthChange_original)(tDepth_effect_type pType, br_pixelmap* pSky_texture, int pStart, int pEnd, int pRed, int pGreen, int pBlue, int pParam_8);
void C2_HOOK_FASTCALL InstantDepthChange(tDepth_effect_type pType, br_pixelmap* pSky_texture, int pStart, int pEnd, int pRed, int pGreen, int pBlue, int pParam_8) {
#if defined(C2_HOOKS_ENABLED)
    InstantDepthChange_original(pType, pSky_texture, pStart, pEnd, pRed, pGreen, pBlue, pParam_8);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00445340, InstantDepthChange, InstantDepthChange_original)

int C2_HOOK_FASTCALL GetSkyTextureOn(void) {

    return C2V(gSky_on);
}
C2_HOOK_FUNCTION(0x00446e00, GetSkyTextureOn)

void (C2_HOOK_FASTCALL * SetSkyTextureOn_original)(int skyTextureOn);
void C2_HOOK_FASTCALL SetSkyTextureOn(int skyTextureOn) {
#if defined(C2_HOOKS_ENABLED)
    SetSkyTextureOn_original(skyTextureOn);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00446e10, SetSkyTextureOn, SetSkyTextureOn_original)

void (C2_HOOK_FASTCALL * ToggleSkyQuietly_original)(void);
void C2_HOOK_FASTCALL ToggleSkyQuietly(void) {

#if defined(C2_HOOKS_ENABLED)
    ToggleSkyQuietly_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00446e80, ToggleSkyQuietly, ToggleSkyQuietly_original)

int C2_HOOK_FASTCALL GetDepthCueingOn(void) {

    return C2V(gDepth_cueing_on);
}
C2_HOOK_FUNCTION(0x00446f90, GetDepthCueingOn)

void (C2_HOOK_FASTCALL * SetDepthCueingOn_original)(int pOn);
void C2_HOOK_FASTCALL SetDepthCueingOn(int pOn) {

#if defined(C2_HOOKS_ENABLED)
    SetDepthCueingOn_original(pOn);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00446fa0, SetDepthCueingOn, SetDepthCueingOn_original)

void (C2_HOOK_FASTCALL * ToggleDepthCueingQuietly_original)(void);
void C2_HOOK_FASTCALL ToggleDepthCueingQuietly(void) {

#if defined(C2_HOOKS_ENABLED)
    ToggleDepthCueingQuietly_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00447070, ToggleDepthCueingQuietly, ToggleDepthCueingQuietly_original)

void (C2_HOOK_FASTCALL * ToggleDepthCueing_original)(void);
void C2_HOOK_FASTCALL ToggleDepthCueing(void) {

#if defined(C2_HOOKS_ENABLED)
    ToggleDepthCueing_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00447110, ToggleDepthCueing, ToggleDepthCueing_original)

void C2_HOOK_STDCALL SetYon(br_scalar pYon) {
    int i;
    br_camera* camera_ptr;

    if (pYon < 5.0f) {
        pYon = 5.0f;
    }

    for (i = 0; i < REC2_ASIZE(C2V(gCamera_list)); i++) {
        if (C2V(gCamera_list)[i] != NULL) {
            camera_ptr = C2V(gCamera_list)[i]->type_data;
            camera_ptr->yon_z = pYon;
        }
    }
    C2V(gCamera_yon) = pYon;
}
C2_HOOK_FUNCTION(0x00446b70, SetYon)

br_scalar C2_HOOK_STDCALL GetYon(void) {

    return C2V(gCamera_yon);
}
C2_HOOK_FUNCTION(0x00446bb0, GetYon)

br_model* C2_HOOK_FASTCALL CreateHorizonModel(br_actor* pCamera) {
    tU8 nbands;
    tU8 band;
    tU8 vertex;
    tU8 stripe;
    br_model* model;

    nbands = 21;
    model = BrModelAllocate(NULL, 88, 6 * nbands);
    model->flags |= BR_MODF_KEEP_ORIGINAL;
    for (band = 0; band < nbands; band++) {
        for (stripe = 0; stripe < 3; stripe++) {
            vertex = 6 * band + 2 * stripe;
            model->faces[vertex + 0].vertices[0] = stripe + 4 * band + 0;
            model->faces[vertex + 0].vertices[1] = stripe + 4 * band + 1;
            model->faces[vertex + 0].vertices[2] = stripe + 4 * band + 5;
            model->faces[vertex + 1].vertices[0] = stripe + 4 * band + 0;
            model->faces[vertex + 1].vertices[1] = stripe + 4 * band + 5;
            model->faces[vertex + 1].vertices[2] = stripe + 4 * band + 4;

            model->faces[vertex + 0].smoothing = 1;
            model->faces[vertex + 1].smoothing = 1;

            model->faces[vertex + 0].material = NULL;
            model->faces[vertex + 1].material = NULL;
        }
    }
    return model;
}

int Log2(int pNumber) {
    int i;
    int bits[16];

    bits[0] = 1;
    bits[1] = 2;
    bits[2] = 4;
    bits[3] = 8;
    bits[4] = 16;
    bits[5] = 32;
    bits[6] = 64;
    bits[7] = 128;
    bits[8] = 256;
    bits[9] = 512;
    bits[10] = 1024;
    bits[11] = 2048;
    bits[12] = 4096;
    bits[13] = 0x2000;
    bits[14] = 0x4000;
    bits[15] = 0x8000;
    for (i = REC2_ASIZE(bits) - 1; i >= 0; --i) {
        if (bits[i] & pNumber) {
            return i;
        }
    }
    return 0;
}

void C2_HOOK_FASTCALL LoadDepthTable(char* pName, br_pixelmap** pTable, int* pPower) {
    tPath_name the_path;
    int i;
    int j;
    tU8 temp;

#define PTABLE_PIXEL_AT(X, Y) ((tU8*)((*pTable)->pixels))[(X) + (Y) * (*pTable)->row_bytes]

    PathCat(the_path, C2V(gApplication_path), "SHADETAB");
    PathCat(the_path, the_path, pName);
    *pTable = DRPixelmapLoad(the_path);
    if (*pTable == NULL) {
        FatalError(kFatalError_CannotLoadDepthCueShadeTable);
    }
    *pPower = Log2((*pTable)->height);
    for (i = 0; i < (*pTable)->width; i++) {
        for (j = 0; j < (*pTable)->height / 2; j++) {
            temp = PTABLE_PIXEL_AT(i, j);
            PTABLE_PIXEL_AT(i, j) = PTABLE_PIXEL_AT(i, ((*pTable)->height - j - 1));
            PTABLE_PIXEL_AT(i, ((*pTable)->height - j - 1)) = temp;
        }
    }

#undef PTABLE_PIXEL_AT
}
C2_HOOK_FUNCTION(0x00445860, LoadDepthTable)

void (C2_HOOK_FASTCALL * InitDepthEffects_original)(void);
void C2_HOOK_FASTCALL InitDepthEffects(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    InitDepthEffects_original();
#else
    LoadDepthTable("DEPTHCUE.TAB", &C2V(gDepth_shade_table), &C2V(gDepth_shade_table_power));
    LoadDepthTable("FOG.TAB", &C2V(gFog_shade_table), &C2V(gFog_shade_table_power));
    LoadDepthTable("ACIDFOG.TAB", &C2V(gAcid_shade_table), &C2V(gAcid_shade_table_power));
    LoadDepthTable("BLUEGIT.TAB", &C2V(gWater_shade_table), &C2V(gWater_shade_table_power));
    GenerateSmokeShades();
    C2V(gHorizon_material) = BrMaterialFind("HORIZON.MAT");
    if (C2V(gHorizon_material) == NULL) {
        FatalError(kFatalError_CannotFindSkyMaterial_S);
    }
    if (C2V(gScreen)->type == BR_PMT_INDEX_8 && !C2V(gNo_fog)) {
        int i, j;
        br_uint_8* pixels;
        C2V(gHorizon_material)->index_blend = BrPixelmapAllocate(BR_PMT_INDEX_8, 256, 256, NULL, 0);
        BrTableAdd(C2V(gHorizon_material)->index_blend);
        pixels = C2V(gHorizon_material)->index_blend->pixels;
        for (i = 0; i < 256; i++) {
            for (j = 0; j < 256; j++) {
                pixels[256*i + j] = j;
            }
        }
        C2V(gHorizon_material)->flags |= BR_MATF_PERSPECTIVE;
    }
    C2V(gHorizon_material)->flags |= BR_MATF_MAP_INTERPOLATION;
    C2V(gSky_model) = CreateHorizonModel(C2V(gCamera));
    C2V(gSky_actor) = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    C2V(gSky_actor)->model = C2V(gSky_model);
    C2V(gSky_actor)->material = C2V(gHorizon_material);
    C2V(gSky_actor)->render_style = BR_RSTYLE_NONE;
    BrActorAdd(C2V(gUniverse_actor), C2V(gSky_actor));
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00445620, InitDepthEffects, InitDepthEffects_original)

void (C2_HOOK_FASTCALL * MungeForwardSky_original)(void);
void C2_HOOK_FASTCALL MungeForwardSky(void) {

#if defined(C2_HOOKS_ENABLED)
    MungeForwardSky_original();
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00447250, MungeForwardSky, MungeForwardSky_original)

void C2_HOOK_FASTCALL AssertYons(void) {
    br_camera* camera_ptr;
    int i;

    for (i = 0; i < REC2_ASIZE(C2V(gCamera_list)); i++) {
        camera_ptr = C2V(gCamera_list)[i]->type_data;
        camera_ptr->yon_z = C2V(gYon_multiplier) * C2V(gCamera_yon);
    }
}
C2_HOOK_FUNCTION(0x00446a10, AssertYons)

void C2_HOOK_FASTCALL MungeRearviewSky(void) {

}
C2_HOOK_FUNCTION(0x00447280, MungeRearviewSky)

void (C2_HOOK_FASTCALL * FogAccordingToGPSCDE_original)(br_material* pMaterial);
void C2_HOOK_FASTCALL FogAccordingToGPSCDE(br_material* pMaterial) {

#if 0//defined(C2_HOOKS_ENABLED)
    FogAccordingToGPSCDE_original(pMaterial);
#else
    int end;
    int start;

    end = C2V(gProgram_state).current_depth_effect.end;
    start = C2V(gProgram_state).current_depth_effect.start;

    switch (C2V(gProgram_state).current_depth_effect.type) {
    case eDepth_effect_none:
        pMaterial->flags = pMaterial->flags & ~BR_MATF_FOG_LOCAL;
        break;
    case eDepth_effect_darkness:
        pMaterial->fog_min = C2V(gCamera_yon) * powf(-0.1f * (float)start, 10.0f);
        pMaterial->fog_max = C2V(gCamera_yon) * powf(0.1f * (float)end, 10.0f);
        pMaterial->fog_colour = 0;
        pMaterial->flags |= BR_MATF_FOG_LOCAL;
        break;
    case eDepth_effect_fog:
        pMaterial->fog_min = C2V(gCamera_yon) * powf(-0.1f * (float)start, 10.0f);
        pMaterial->fog_max = C2V(gCamera_yon) * powf(0.1f * (float)end, 10.0f);
        pMaterial->fog_colour = BR_COLOUR_RGB(0xf8, 0xf8, 0xf8);
        pMaterial->flags |= BR_MATF_FOG_LOCAL;
        break;
    case eDepth_effect_colour:
        pMaterial->fog_min = C2V(gCamera_yon) * powf(-0.1f * (float)start, 10.0f);
        pMaterial->fog_max = powf(0.1f * end, 10.0f) * C2V(gCamera_yon);
        pMaterial->fog_colour = BR_COLOUR_RGB(
                C2V(gProgram_state).current_depth_effect.colour.red,
                C2V(gProgram_state).current_depth_effect.colour.green,
                C2V(gProgram_state).current_depth_effect.colour.blue);
        pMaterial->flags |= BR_MATF_FOG_LOCAL;
        break;
    }
    BrMaterialUpdate(pMaterial, BR_MATU_ALL);
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x004451a0, FogAccordingToGPSCDE, FogAccordingToGPSCDE_original)

void C2_HOOK_FASTCALL ChangeDepthEffect(void) {

    C2V(gProgram_state).current_depth_effect.type = C2V(gProgram_state).default_depth_effect.type;
    C2V(gProgram_state).current_depth_effect.sky_texture = C2V(gProgram_state).default_depth_effect.sky_texture;
    C2V(gProgram_state).current_depth_effect.start = C2V(gProgram_state).default_depth_effect.start;
    C2V(gProgram_state).current_depth_effect.end = C2V(gProgram_state).default_depth_effect.end;
}
C2_HOOK_FUNCTION(0x00447220, ChangeDepthEffect)
