#include "depth.h"

#include "displays.h"
#include "errors.h"
#include "globvars.h"
#include "globvrkm.h"
#include "init.h"
#include "loading.h"
#include "skidmark.h"
#include "spark.h"
#include "world.h"
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
C2_HOOK_VARIABLE_IMPLEMENT(int, gSwap_depth_effect_start, 0x0079ec4c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gSwap_depth_effect_end, 0x0079ec48);
C2_HOOK_VARIABLE_IMPLEMENT(int, gSwap_depth_effect_colour_blue, 0x0079ec34);
C2_HOOK_VARIABLE_IMPLEMENT(int, gSwap_depth_effect_colour_red, 0x0079ec3c);
C2_HOOK_VARIABLE_IMPLEMENT(int, gSwap_depth_effect_colour_green, 0x0079ec40);
C2_HOOK_VARIABLE_IMPLEMENT(float, gSky_width, 0x0067c4c8);
C2_HOOK_VARIABLE_IMPLEMENT(float, gSky_height, 0x0067c4b4);
C2_HOOK_VARIABLE_IMPLEMENT(float, gSky_x_multiplier, 0x0067c4d0);
C2_HOOK_VARIABLE_IMPLEMENT(float, gSky_y_multiplier, 0x0067c4d4);
C2_HOOK_VARIABLE_IMPLEMENT(int, gHas_sky_texture, 0x0067c4a8);


intptr_t C2_HOOK_CDECL SwitchCarModel(br_actor* pActor, void* pData) {

    if (pActor->user != NULL) {
        int* index = (int*)pData;
        tUser_crush_data *crush_data = pActor->user;
        br_model *model = crush_data->models[*index];
        if (model != NULL) {
            pActor->model = model;
            pActor->type = BR_ACTOR_MODEL;
            return 0;
        }
    }
    pActor->model = NULL;
    pActor->type = BR_ACTOR_NONE;
    return 0;
}
C2_HOOK_FUNCTION(0x00413f10, SwitchCarModel)

void C2_HOOK_FASTCALL SwitchCarModels(tCar_spec* pCar, int pIndex) {

    int data = pIndex;
    DRActorEnumRecurse(pCar->car_model_actor, SwitchCarModel, &data);
    pCar->field_0xe18 = data;
}
C2_HOOK_FUNCTION(0x00413f40, SwitchCarModels)

void C2_HOOK_FASTCALL ProcessModelFaceMaterials2(br_model* pModel, material_cbfn* pCallback) {

    if (pModel->faces != NULL) {
        int f;

        for (f = 0; f < pModel->nfaces; f++) {
            if (pModel->faces[f].material != NULL) {
                pCallback(pModel->faces[f].material);
            }
        }
    } else {
        v11model* v11m = pModel->prepared;
        int g;

        for (g = 0; g < v11m->ngroups; g++) {
            int f;
            v11group* v11g = &v11m->groups[g];

            /* FIXME: this inner-loop can be removed. */
            for (f = 0; f < v11g->nfaces; f++) {
#if 0
                v11face* v11f = &v11g->faces[f];
#endif
                if (*v11g->face_colours.materials != NULL) {
                    pCallback(*v11g->face_colours.materials);
                }
            }
        }
    }
}
C2_HOOK_FUNCTION(0x00448fd0, ProcessModelFaceMaterials2)

void C2_HOOK_FASTCALL FogCars(void) {
    int i;

    for (i = 0; i < C2V(gCurrent_race).number_of_racers; i++) {
        int j;
        tCar_spec* car = C2V(gCurrent_race).opponent_list[i].car_spec;

        for (j = 0; j < car->count_detail_levels; j++) {
            SwitchCarModels(car, j);
            ProcessMaterials(car->car_model_actor, FogAccordingToGPSCDE);
        }
        SwitchCarModels(car, 0);
    }
}

void C2_HOOK_FASTCALL FrobFog(void) {
    int i;
    br_material* material;

    if (C2V(gTrack_actor) != NULL) {
        ProcessMaterials(C2V(gTrack_actor), FogAccordingToGPSCDE);
    }
    if (C2V(gNon_track_actor) != NULL) {
        ProcessMaterials(C2V(gNon_track_actor), FogAccordingToGPSCDE);
    }
    FogCars();

    material = BrMaterialFind("GIBSLICK");
    if (material != NULL) {
        FogAccordingToGPSCDE(material);
    }
    material = BrMaterialFind("PEDSMEAR");
    if (material != NULL) {
        FogAccordingToGPSCDE(material);
    }
    for (i = 0; i < REC2_ASIZE(C2V(gMaterial)); i++) {
        FogAccordingToGPSCDE(C2V(gMaterial)[i]);
    }
    for (i = 0; i < REC2_ASIZE(C2V(gCurrent_race).material_modifiers); i++) {
        if (C2V(gCurrent_race).material_modifiers[i].skid_mark_material != NULL) {
            FogAccordingToGPSCDE(C2V(gCurrent_race).material_modifiers[i].skid_mark_material);
        }
    }
    FogAccordingToGPSCDE(C2V(gDefault_track_material));
}

void (C2_HOOK_FASTCALL * InstantDepthChange_original)(tDepth_effect_type pType, br_pixelmap* pSky_texture, int pStart, int pEnd, int pRed, int pGreen, int pBlue, int pParam_8);
void C2_HOOK_FASTCALL InstantDepthChange(tDepth_effect_type pType, br_pixelmap* pSky_texture, int pStart, int pEnd, int pRed, int pGreen, int pBlue, int pParam_8) {
#if 0//defined(C2_HOOKS_ENABLED)
    InstantDepthChange_original(pType, pSky_texture, pStart, pEnd, pRed, pGreen, pBlue, pParam_8);
#else

    if (pType == eDepth_effect_none) {
        pStart = 3;
        pEnd = 3;
    }
    C2V(gProgram_state).current_depth_effect.sky_texture = pSky_texture;

    C2V(gHorizon_material)->colour_map = pSky_texture;
    BrMaterialUpdate(C2V(gHorizon_material), BR_MATU_ALL);
    MungeSkyVs(C2V(gSky_model), C2V(gHorizon_material));

    C2V(gProgram_state).current_depth_effect.colour.red = pRed;
    C2V(gProgram_state).current_depth_effect.colour.green = pGreen;
    C2V(gProgram_state).current_depth_effect.colour.blue = pBlue;

    C2V(gProgram_state).default_depth_effect.colour.red = pRed;
    C2V(gProgram_state).default_depth_effect.colour.green = pGreen;
    C2V(gProgram_state).default_depth_effect.colour.blue = pBlue;

    C2V(gProgram_state).current_depth_effect.type = pType;
    C2V(gProgram_state).current_depth_effect.start = pStart;
    C2V(gProgram_state).current_depth_effect.end = pEnd;

    C2V(gProgram_state).default_depth_effect.type = pType;
    C2V(gProgram_state).default_depth_effect.start = pStart;
    C2V(gProgram_state).default_depth_effect.end = pEnd;
    if (C2V(gNo_fog) && pParam_8) {
        FrobFog();
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00445340, InstantDepthChange, InstantDepthChange_original)

int C2_HOOK_FASTCALL GetSkyTextureOn(void) {

    return C2V(gSky_on);
}
C2_HOOK_FUNCTION(0x00446e00, GetSkyTextureOn)

void (C2_HOOK_FASTCALL * SetSkyTextureOn_original)(int skyTextureOn);
void C2_HOOK_FASTCALL SetSkyTextureOn(int skyTextureOn) {
#if 0//defined(C2_HOOKS_ENABLED)
    SetSkyTextureOn_original(skyTextureOn);
#else

    if (C2V(gSky_on) != skyTextureOn) {
        ToggleSkyQuietly();
    }
    C2V(gSky_on) = skyTextureOn;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00446e10, SetSkyTextureOn, SetSkyTextureOn_original)

void C2_HOOK_FASTCALL MungeSkyVs(br_model* pModel, br_material* pMaterial) {
    int i;
    float tex_y;
    int height;

    if (pMaterial->colour_map == NULL) {
        return;
    }
    if (pMaterial->flags & BR_MATF_MAP_INTERPOLATION) {
        height = pMaterial->colour_map->height - 2;
        tex_y = 1.f / (float)pMaterial->colour_map->height;
    } else {
        height = pMaterial->colour_map->height;
        tex_y = 0.f;
    }
    for (i = 0; i < 12; i++) {
        pModel->vertices[i].map.v[1] = 1.f - tex_y;
    }
    for (i = 80; i < 88; i++) {
        pModel->vertices[i].map.v[1] = tex_y;
    }
    for (i = 0; i < 17; i++) {
        int j;
        int idx = 12 + 4 * i;

        pModel->vertices[idx].map.v[1] = tex_y + (float)(17 - i) * (float)height / 18.f / pMaterial->colour_map->height;
        for (j = 1; j < 4; j++) {
            pModel->vertices[idx + j].map.v[1] = pModel->vertices[idx].map.v[1];
        }
    }
}
C2_HOOK_FUNCTION(0x00445500, MungeSkyVs)

void (C2_HOOK_FASTCALL * ToggleSkyQuietly_original)(void);
void C2_HOOK_FASTCALL ToggleSkyQuietly(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    ToggleSkyQuietly_original();
#else
    C2V(gProgram_state).default_depth_effect.sky_texture = C2V(gSky_texture_0079ec1c);
    C2V(gSky_texture_0079ec1c) = C2V(gProgram_state).current_depth_effect.sky_texture;
    C2V(gProgram_state).current_depth_effect.sky_texture = C2V(gProgram_state).default_depth_effect.sky_texture;
    if (C2V(gHorizon_material) != NULL && C2V(gProgram_state).default_depth_effect.sky_texture != NULL) {
        C2V(gHorizon_material)->colour_map = C2V(gProgram_state).current_depth_effect.sky_texture;
        BrMaterialUpdate(C2V(gHorizon_material), BR_MATU_ALL);
        MungeSkyVs(C2V(gSky_model), C2V(gHorizon_material));
    }
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00446e80, ToggleSkyQuietly, ToggleSkyQuietly_original)

int C2_HOOK_FASTCALL GetDepthCueingOn(void) {

    return C2V(gDepth_cueing_on);
}
C2_HOOK_FUNCTION(0x00446f90, GetDepthCueingOn)

void (C2_HOOK_FASTCALL * SetDepthCueingOn_original)(int pOn);
void C2_HOOK_FASTCALL SetDepthCueingOn(int pOn) {

#if 0//defined(C2_HOOKS_ENABLED)
    SetDepthCueingOn_original(pOn);
#else

    if (C2V(gDepth_cueing_on) != pOn && C2V(gHorizon_material) != NULL) {
        ToggleDepthCueingQuietly();
    }
    C2V(gDepth_cueing_on) = pOn;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00446fa0, SetDepthCueingOn, SetDepthCueingOn_original)

void (C2_HOOK_FASTCALL * ToggleDepthCueingQuietly_original)(void);
void C2_HOOK_FASTCALL ToggleDepthCueingQuietly(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    ToggleDepthCueingQuietly_original();
#else
    int temp_red;
    int temp_green;
    int temp_blue;
    int temp_end;
    int temp_start;
    tDepth_effect_type temp_type;

    temp_start = C2V(gProgram_state).current_depth_effect.start;
    temp_end = C2V(gProgram_state).current_depth_effect.end;
    temp_type = C2V(gProgram_state).current_depth_effect.type;
    temp_red = C2V(gProgram_state).current_depth_effect.colour.blue;
    temp_green = C2V(gProgram_state).current_depth_effect.colour.green;
    temp_blue = C2V(gProgram_state).current_depth_effect.colour.blue;
    InstantDepthChange(
        C2V(gSwap_depth_effect_type),
        C2V(gProgram_state).current_depth_effect.sky_texture,
        C2V(gSwap_depth_effect_start),
        C2V(gSwap_depth_effect_end),
        C2V(gSwap_depth_effect_colour_red),
        C2V(gSwap_depth_effect_colour_green),
        C2V(gSwap_depth_effect_colour_blue),
        C2V(gProgram_state).racing);
    C2V(gSwap_depth_effect_start) = temp_start;
    C2V(gSwap_depth_effect_end) = temp_end;
    C2V(gSwap_depth_effect_type) = temp_type;
    C2V(gSwap_depth_effect_colour_red) = temp_red;
    C2V(gSwap_depth_effect_colour_green) = temp_green;
    C2V(gSwap_depth_effect_colour_blue) = temp_blue;
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00447070, ToggleDepthCueingQuietly, ToggleDepthCueingQuietly_original)

void (C2_HOOK_FASTCALL * ToggleDepthCueing_original)(void);
void C2_HOOK_FASTCALL ToggleDepthCueing(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    ToggleDepthCueing_original();
#else
    int enabled;

    enabled = GetDepthCueingOn();
    SetDepthCueingOn(!enabled);
    if (C2V(gProgram_state).current_depth_effect.type != eDepth_effect_none) {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_depth_cueing_on));
    } else if (C2V(gSwap_depth_effect_type) != eDepth_effect_none) {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_depth_cueing_off));
    } else {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_there_is_no_depth_cueing_for_this_race));
    }
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

br_scalar C2_HOOK_FASTCALL Tan(br_scalar pAngle) {

    return sinf(BrAngleToRadian(pAngle)) / cosf(BrAngleToRadian(pAngle));
}

br_scalar C2_HOOK_FASTCALL EdgeU(br_angle pSky, br_angle pView, br_angle pPerfect) {
    br_scalar a;
    br_scalar b;
    br_scalar c;

    a = cosf(BrAngleToRadian(pPerfect));
    b = sinf(BrAngleToRadian(pView));
    c = cosf(BrAngleToRadian(pView));
    return b * a * a / (BrAngleToRadian(pSky) * (1.f + c));
}

br_scalar C2_HOOK_FASTCALL CalculateWrappingMultiplier(br_scalar pValue, br_scalar pYon) {
    br_scalar k;
    br_scalar trunc_k;
    int int_k;

    k = (float)(pYon * 1.3f * REC2_TAU / pValue);
    int_k = (int)k;
    if (k - (br_scalar)int_k <= .5f) {
        trunc_k = (br_scalar)int_k;
    } else {
        trunc_k = (br_scalar)int_k + 1.f;
    }
    return (float)(trunc_k / REC2_TAU * pValue);
}

void C2_HOOK_FASTCALL MungeSkyModel(br_actor* pCamera, br_model* pModel) {
    br_camera* camera_data;
    br_scalar horizon_half_height;
    br_scalar horizon_half_width;
    br_scalar horizon_half_diag;
    br_scalar tan_half_fov;
    br_scalar sky_distance;
    br_angle half_diag_fov;
    tU8 nbands;
    tU8 band;
    tU8 vertex;
    tU8 stripe;
    br_scalar edge_u;
    br_scalar narrow_u;
    br_angle min_angle;
    br_angle angle_range;
    br_angle angle;

    camera_data = pCamera->type_data;
    tan_half_fov = Tan((float)camera_data->field_of_view / 2);
    sky_distance = camera_data->yon_z - 1.f;
    horizon_half_width = sky_distance * tan_half_fov;
    horizon_half_height = horizon_half_width * camera_data->aspect;
    horizon_half_diag = sqrtf(horizon_half_height * horizon_half_height + horizon_half_width * horizon_half_width);
    half_diag_fov = BrRadianToAngle(atan2f(horizon_half_diag, sky_distance));
    edge_u = EdgeU(C2V(gSky_image_width), 2 * half_diag_fov, BR_ANGLE_DEG(10));
    narrow_u = edge_u / 2.f;
    C2V(gSky_width) = horizon_half_width * 2.f;
    C2V(gSky_height) = horizon_half_height * 2.f;
    C2V(gSky_x_multiplier) = CalculateWrappingMultiplier(C2V(gSky_width), camera_data->yon_z);
    C2V(gSky_y_multiplier) = CalculateWrappingMultiplier(C2V(gSky_height), camera_data->yon_z);

    for (vertex = 0; vertex < 88; vertex += 4) {
        pModel->vertices[vertex].map.v[0] = -edge_u;
    }
    for (vertex = 1; vertex < 88; vertex += 4) {
        pModel->vertices[vertex].map.v[0] = -narrow_u;
    }
    for (vertex = 2; vertex < 88; vertex += 4) {
        pModel->vertices[vertex].map.v[0] = narrow_u;
    }
    for (vertex = 3; vertex < 88; vertex += 4) {
        pModel->vertices[vertex].map.v[0] = edge_u;
    }
    for (vertex = 0; vertex < 88; vertex += 4) {
        pModel->vertices[vertex].p.v[0] = -horizon_half_diag;
    }
    for (vertex = 1; vertex < 88; vertex += 4) {
        pModel->vertices[vertex].p.v[0] = -(horizon_half_diag / 2.f);
    }
    for (vertex = 2; vertex < 88; vertex += 4) {
        pModel->vertices[vertex].p.v[0] = horizon_half_diag / 2.f;
    }
    for (vertex = 3; vertex < 88; vertex += 4) {
        pModel->vertices[vertex].p.v[0] = horizon_half_diag;
    }
    PossibleService();
    angle_range = -C2V(gSky_image_underground) - (-BR_ANGLE_DEG(90) - half_diag_fov);
    for (band = 0; band < 2u; band++) {
        vertex = 4 * band;
        angle = -BR_ANGLE_DEG(90) - half_diag_fov + angle_range * band / 2;
        pModel->vertices[vertex].p.v[1] = sinf(BrAngleToRadian(angle)) * sky_distance;
        pModel->vertices[vertex].p.v[2] = -cosf(BrAngleToRadian(angle)) * sky_distance;
    }
    min_angle = -C2V(gSky_image_underground);
    angle_range = C2V(gSky_image_height);
    nbands = 18;
    for (band = 0; band < nbands; band++) {
        vertex = 4 * band + 8;
        pModel->vertices[vertex].p.v[1] = sinf(BrAngleToRadian(min_angle + angle_range * band / nbands)) * sky_distance;
        pModel->vertices[vertex].p.v[2] = -cosf(BrAngleToRadian(min_angle + angle_range * band / nbands)) * sky_distance;
    }
    min_angle = C2V(gSky_image_height) - C2V(gSky_image_underground);
    angle_range = half_diag_fov + BR_ANGLE_DEG(90) - (C2V(gSky_image_height) - C2V(gSky_image_underground));
    for (band = 0; band <= 1u; band++) {
        vertex = 4 * band + 80;
        angle = min_angle + angle_range * band;
        pModel->vertices[vertex].p.v[1] = sinf(BrAngleToRadian(angle)) * sky_distance;
        pModel->vertices[vertex].p.v[2] = -cosf(BrAngleToRadian(angle)) * sky_distance;
    }
    PossibleService();
    for (band = 0; band <= 21u; ++band) {
        vertex = 4 * band;
        for (stripe = 1; stripe < 4u; ++stripe) {
            pModel->vertices[vertex + stripe].p.v[1] = pModel->vertices[vertex].p.v[1];
            pModel->vertices[vertex + stripe].p.v[2] = pModel->vertices[vertex].p.v[2];
        }
    }

    BrModelUpdate(pModel, BR_MODU_ALL & ~BR_MODU_FACES);
}
C2_HOOK_FUNCTION(0x00445e20, MungeSkyModel)

void (C2_HOOK_FASTCALL * MungeForwardSky_original)(void);
void C2_HOOK_FASTCALL MungeForwardSky(void) {

#if 0//defined(C2_HOOKS_ENABLED)
    MungeForwardSky_original();
#else
    MungeSkyModel(C2V(gCamera), C2V(gSky_model));
    C2V(gHas_sky_texture) = C2V(gProgram_state).default_depth_effect.sky_texture != NULL;
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

void C2_HOOK_FASTCALL SkyTextureChanged(void) {

    BrMaterialUpdate(C2V(gHorizon_material), BR_MATU_COLOURMAP);
}
C2_HOOK_FUNCTION(0x00447290, SkyTextureChanged)

void C2_HOOK_FASTCALL SetSkyColour(br_colour c) {

    C2V(gHorizon_material)->colour = c;
}
C2_HOOK_FUNCTION(0x004472b0, SetSkyColour)

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
