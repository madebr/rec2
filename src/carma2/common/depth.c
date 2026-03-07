#include "depth.h"

#include "displays.h"
#include "errors.h"
#include "globvars.h"
#include "globvrkm.h"
#include "globvrpb.h"
#include "init.h"
#include "loading.h"
#include "platform.h"
#include "replay.h"
#include "skidmark.h"
#include "spark.h"
#include "tinted.h"
#include "trig.h"
#include "world.h"
#include "utility.h"

#include "rec2_macros.h"


// GLOBAL: CARMA2_HW 0x0079ec44
tSpecial_volume* gDAT_0079ec44;


// GLOBAL: CARMA2_HW 0x00591188
int gSky_on = 1;

// GLOBAL: CARMA2_HW 0x0059118c
int gDepth_cueing_on = 1;


// GLOBAL: CARMA2_HW 0x0079ec20
br_pixelmap* gDepth_shade_table;

// GLOBAL: CARMA2_HW 0x0067c4bc
int gDepth_shade_table_power;

// GLOBAL: CARMA2_HW 0x0079ec38
br_pixelmap* gFog_shade_table;

// GLOBAL: CARMA2_HW 0x0067c4a4
int gFog_shade_table_power;

// GLOBAL: CARMA2_HW 0x0079ec24
br_pixelmap* gAcid_shade_table;

// GLOBAL: CARMA2_HW 0x0067c4dc
int gAcid_shade_table_power;

// GLOBAL: CARMA2_HW 0x0079ec28
br_pixelmap* gWater_shade_table;

// GLOBAL: CARMA2_HW 0x0067c4cc
int gWater_shade_table_power;


// GLOBAL: CARMA2_HW 0x0067c4e0
br_material* gHorizon_material;

// GLOBAL: CARMA2_HW 0x0067c4d8
br_actor* gForward_sky_actor;

// GLOBAL: CARMA2_HW 0x0067c4a0
br_model* gForward_sky_model;

// GLOBAL: CARMA2_HW 0x00591190
tDepth_effect_type gSwap_depth_effect_type = eDepth_effect_none;

// GLOBAL: CARMA2_HW 0x0079ec1c
br_pixelmap* gSky_texture_0079ec1c;

// GLOBAL: CARMA2_HW 0x0079ec4c
int gSwap_depth_effect_start;

// GLOBAL: CARMA2_HW 0x0079ec48
int gSwap_depth_effect_end;

// GLOBAL: CARMA2_HW 0x0079ec34
int gSwap_depth_effect_colour_blue;

// GLOBAL: CARMA2_HW 0x0079ec3c
int gSwap_depth_effect_colour_red;

// GLOBAL: CARMA2_HW 0x0079ec40
int gSwap_depth_effect_colour_green;

// GLOBAL: CARMA2_HW 0x0067c4c8
float gSky_width;

// GLOBAL: CARMA2_HW 0x0067c4b4
float gSky_height;

// GLOBAL: CARMA2_HW 0x0067c4d0
float gSky_x_multiplier;

// GLOBAL: CARMA2_HW 0x0067c4d4
float gSky_y_multiplier;

// GLOBAL: CARMA2_HW 0x0067c4a8
int gHas_sky_texture;

// GLOBAL: CARMA2_HW 0x0067c4c0
br_actor* gRearview_sky_actor;

// GLOBAL: CARMA2_HW 0x0067c4ac
br_angle gOld_fov;

// GLOBAL: CARMA2_HW 0x0067c4b8
br_scalar gOld_yon;

// GLOBAL: CARMA2_HW 0x0079ec44
tSpecial_volume* gLast_camera_special_volume;

#define ACTOR_CAMERA(ACTOR) ((br_camera*)((ACTOR)->type_data))


// FUNCTION: CARMA2_HW 0x00413f10
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

// FUNCTION: CARMA2_HW 0x00413f40
void C2_HOOK_FASTCALL SwitchCarModels(tCar_spec* pCar, int pIndex) {

    int data = pIndex;
    DRActorEnumRecurse(pCar->car_model_actor, SwitchCarModel, &data);
    pCar->field_0xe18 = data;
}

// FUNCTION: CARMA2_HW 0x00448fd0
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

void C2_HOOK_FASTCALL FogCars(void) {
    int i;

    for (i = 0; i < gCurrent_race.number_of_racers; i++) {
        int j;
        tCar_spec* car = gCurrent_race.opponent_list[i].car_spec;

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

    if (gTrack_actor != NULL) {
        ProcessMaterials(gTrack_actor, FogAccordingToGPSCDE);
    }
    if (gNon_track_actor != NULL) {
        ProcessMaterials(gNon_track_actor, FogAccordingToGPSCDE);
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
    for (i = 0; i < REC2_ASIZE(gMaterial); i++) {
        FogAccordingToGPSCDE(gMaterial[i]);
    }
    for (i = 0; i < REC2_ASIZE(gCurrent_race.material_modifiers); i++) {
        if (gCurrent_race.material_modifiers[i].skid_mark_material != NULL) {
            FogAccordingToGPSCDE(gCurrent_race.material_modifiers[i].skid_mark_material);
        }
    }
    FogAccordingToGPSCDE(gDefault_track_material);
}

// FUNCTION: CARMA2_HW 0x00445340
void C2_HOOK_FASTCALL InstantDepthChange(tDepth_effect_type pType, br_pixelmap* pSky_texture, int pStart, int pEnd, int pRed, int pGreen, int pBlue, int pParam_8) {

    if (pType == eDepth_effect_none) {
        pStart = 3;
        pEnd = 3;
    }
    gProgram_state.current_depth_effect.sky_texture = pSky_texture;

    gHorizon_material->colour_map = pSky_texture;
    BrMaterialUpdate(gHorizon_material, BR_MATU_ALL);
    MungeSkyVs(gForward_sky_model, gHorizon_material);

    gProgram_state.current_depth_effect.colour.red = pRed;
    gProgram_state.current_depth_effect.colour.green = pGreen;
    gProgram_state.current_depth_effect.colour.blue = pBlue;

    gProgram_state.default_depth_effect.colour.red = pRed;
    gProgram_state.default_depth_effect.colour.green = pGreen;
    gProgram_state.default_depth_effect.colour.blue = pBlue;

    gProgram_state.current_depth_effect.type = pType;
    gProgram_state.current_depth_effect.start = pStart;
    gProgram_state.current_depth_effect.end = pEnd;

    gProgram_state.default_depth_effect.type = pType;
    gProgram_state.default_depth_effect.start = pStart;
    gProgram_state.default_depth_effect.end = pEnd;
    if (gNo_fog && pParam_8) {
        FrobFog();
    }
}

// FUNCTION: CARMA2_HW 0x00446e00
int C2_HOOK_FASTCALL GetSkyTextureOn(void) {

    return gSky_on;
}

// FUNCTION: CARMA2_HW 0x00446e10
void C2_HOOK_FASTCALL SetSkyTextureOn(int skyTextureOn) {

    if (gSky_on != skyTextureOn) {
        ToggleSkyQuietly();
    }
    gSky_on = skyTextureOn;
}

// FUNCTION: CARMA2_HW 0x00445500
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

// FUNCTION: CARMA2_HW 0x00446e80
void C2_HOOK_FASTCALL ToggleSkyQuietly(void) {
    gProgram_state.default_depth_effect.sky_texture = gSky_texture_0079ec1c;
    gSky_texture_0079ec1c = gProgram_state.current_depth_effect.sky_texture;
    gProgram_state.current_depth_effect.sky_texture = gProgram_state.default_depth_effect.sky_texture;
    if (gHorizon_material != NULL && gProgram_state.default_depth_effect.sky_texture != NULL) {
        gHorizon_material->colour_map = gProgram_state.current_depth_effect.sky_texture;
        BrMaterialUpdate(gHorizon_material, BR_MATU_ALL);
        MungeSkyVs(gForward_sky_model, gHorizon_material);
    }
}

// FUNCTION: CARMA2_HW 0x00446f90
int C2_HOOK_FASTCALL GetDepthCueingOn(void) {

    return gDepth_cueing_on;
}

// FUNCTION: CARMA2_HW 0x00446fa0
void C2_HOOK_FASTCALL SetDepthCueingOn(int pOn) {

    if (gDepth_cueing_on != pOn && gHorizon_material != NULL) {
        ToggleDepthCueingQuietly();
    }
    gDepth_cueing_on = pOn;
}

// FUNCTION: CARMA2_HW 0x00447070
void C2_HOOK_FASTCALL ToggleDepthCueingQuietly(void) {
    int temp_red;
    int temp_green;
    int temp_blue;
    int temp_end;
    int temp_start;
    tDepth_effect_type temp_type;

    temp_start = gProgram_state.current_depth_effect.start;
    temp_end = gProgram_state.current_depth_effect.end;
    temp_type = gProgram_state.current_depth_effect.type;
    temp_red = gProgram_state.current_depth_effect.colour.blue;
    temp_green = gProgram_state.current_depth_effect.colour.green;
    temp_blue = gProgram_state.current_depth_effect.colour.blue;
    InstantDepthChange(
        gSwap_depth_effect_type,
        gProgram_state.current_depth_effect.sky_texture,
        gSwap_depth_effect_start,
        gSwap_depth_effect_end,
        gSwap_depth_effect_colour_red,
        gSwap_depth_effect_colour_green,
        gSwap_depth_effect_colour_blue,
        gProgram_state.racing);
    gSwap_depth_effect_start = temp_start;
    gSwap_depth_effect_end = temp_end;
    gSwap_depth_effect_type = temp_type;
    gSwap_depth_effect_colour_red = temp_red;
    gSwap_depth_effect_colour_green = temp_green;
    gSwap_depth_effect_colour_blue = temp_blue;
}

// FUNCTION: CARMA2_HW 0x00447110
void C2_HOOK_FASTCALL ToggleDepthCueing(void) {
    int enabled;

    enabled = GetDepthCueingOn();
    SetDepthCueingOn(!enabled);
    if (gProgram_state.current_depth_effect.type != eDepth_effect_none) {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_depth_cueing_on));
    } else if (gSwap_depth_effect_type != eDepth_effect_none) {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_depth_cueing_off));
    } else {
        NewTextHeadupSlot(4, 0, 2000, -4, GetMiscString(eMiscString_there_is_no_depth_cueing_for_this_race));
    }
}

// FUNCTION: CARMA2_HW 0x00446b70
void C2_HOOK_STDCALL SetYon(br_scalar pYon) {
    int i;
    br_camera* camera_ptr;

    if (pYon < 5.0f) {
        pYon = 5.0f;
    }

    for (i = 0; i < REC2_ASIZE(gCamera_list); i++) {
        if (gCamera_list[i] != NULL) {
            camera_ptr = gCamera_list[i]->type_data;
            camera_ptr->yon_z = pYon;
        }
    }
    gCamera_yon = pYon;
}

// FUNCTION: CARMA2_HW 0x00446bb0
br_scalar C2_HOOK_STDCALL GetYon(void) {

    return gCamera_yon;
}

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

// FUNCTION: CARMA2_HW 0x00445860
void C2_HOOK_FASTCALL LoadDepthTable(char* pName, br_pixelmap** pTable, int* pPower) {
    tPath_name the_path;
    int i;
    int j;
    tU8 temp;

#define PTABLE_PIXEL_AT(X, Y) ((tU8*)((*pTable)->pixels))[(X) + (Y) * (*pTable)->row_bytes]

    PathCat(the_path, gApplication_path, "SHADETAB");
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

// FUNCTION: CARMA2_HW 0x00445620
void C2_HOOK_FASTCALL InitDepthEffects(void) {

    LoadDepthTable("DEPTHCUE.TAB", &gDepth_shade_table, &gDepth_shade_table_power);
    LoadDepthTable("FOG.TAB", &gFog_shade_table, &gFog_shade_table_power);
    LoadDepthTable("ACIDFOG.TAB", &gAcid_shade_table, &gAcid_shade_table_power);
    LoadDepthTable("BLUEGIT.TAB", &gWater_shade_table, &gWater_shade_table_power);
    GenerateSmokeShades();
    gHorizon_material = BrMaterialFind("HORIZON.MAT");
    if (gHorizon_material == NULL) {
        FatalError(kFatalError_CannotFindSkyMaterial_S);
    }
    if (gScreen->type == BR_PMT_INDEX_8 && !gNo_fog) {
        int i, j;
        br_uint_8* pixels;
        gHorizon_material->index_blend = BrPixelmapAllocate(BR_PMT_INDEX_8, 256, 256, NULL, 0);
        BrTableAdd(gHorizon_material->index_blend);
        pixels = gHorizon_material->index_blend->pixels;
        for (i = 0; i < 256; i++) {
            for (j = 0; j < 256; j++) {
                pixels[256*i + j] = j;
            }
        }
        gHorizon_material->flags |= BR_MATF_PERSPECTIVE;
    }
    gHorizon_material->flags |= BR_MATF_MAP_INTERPOLATION;
    gForward_sky_model = CreateHorizonModel(gCamera);
    BrModelAdd(gForward_sky_model);
    gForward_sky_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
    gForward_sky_actor->model = gForward_sky_model;
    gForward_sky_actor->material = gHorizon_material;
    gForward_sky_actor->render_style = BR_RSTYLE_NONE;
    BrActorAdd(gUniverse_actor, gForward_sky_actor);
}

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

// FUNCTION: CARMA2_HW 0x00445e20
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
    edge_u = EdgeU(gSky_image_width, 2 * half_diag_fov, BR_ANGLE_DEG(10));
    narrow_u = edge_u / 2.f;
    gSky_width = horizon_half_width * 2.f;
    gSky_height = horizon_half_height * 2.f;
    gSky_x_multiplier = CalculateWrappingMultiplier(gSky_width, camera_data->yon_z);
    gSky_y_multiplier = CalculateWrappingMultiplier(gSky_height, camera_data->yon_z);

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
    angle_range = -gSky_image_underground - (-BR_ANGLE_DEG(90) - half_diag_fov);
    for (band = 0; band < 2u; band++) {
        vertex = 4 * band;
        angle = -BR_ANGLE_DEG(90) - half_diag_fov + angle_range * band / 2;
        pModel->vertices[vertex].p.v[1] = sinf(BrAngleToRadian(angle)) * sky_distance;
        pModel->vertices[vertex].p.v[2] = -cosf(BrAngleToRadian(angle)) * sky_distance;
    }
    min_angle = -gSky_image_underground;
    angle_range = gSky_image_height;
    nbands = 18;
    for (band = 0; band < nbands; band++) {
        vertex = 4 * band + 8;
        pModel->vertices[vertex].p.v[1] = sinf(BrAngleToRadian(min_angle + angle_range * band / nbands)) * sky_distance;
        pModel->vertices[vertex].p.v[2] = -cosf(BrAngleToRadian(min_angle + angle_range * band / nbands)) * sky_distance;
    }
    min_angle = gSky_image_height - gSky_image_underground;
    angle_range = half_diag_fov + BR_ANGLE_DEG(90) - (gSky_image_height - gSky_image_underground);
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

// FUNCTION: CARMA2_HW 0x00447250
void C2_HOOK_FASTCALL MungeForwardSky(void) {

    MungeSkyModel(gCamera, gForward_sky_model);
    gHas_sky_texture = gProgram_state.default_depth_effect.sky_texture != NULL;
}

// FUNCTION: CARMA2_HW 0x00446a10
void C2_HOOK_FASTCALL AssertYons(void) {
    br_camera* camera_ptr;
    int i;

    for (i = 0; i < REC2_ASIZE(gCamera_list); i++) {
        camera_ptr = gCamera_list[i]->type_data;
        camera_ptr->yon_z = gYon_multiplier * gCamera_yon;
    }
}

// FUNCTION: CARMA2_HW 0x00447280
void C2_HOOK_FASTCALL MungeRearviewSky(void) {

}

// FUNCTION: CARMA2_HW 0x00447290
void C2_HOOK_FASTCALL SkyTextureChanged(void) {

    BrMaterialUpdate(gHorizon_material, BR_MATU_COLOURMAP);
}

// FUNCTION: CARMA2_HW 0x004472b0
void C2_HOOK_FASTCALL SetSkyColour(br_colour c) {

    gHorizon_material->colour = c;
}

// FUNCTION: CARMA2_HW 0x004451a0
void C2_HOOK_FASTCALL FogAccordingToGPSCDE(br_material* pMaterial) {
    int end;
    int start;

    end = gProgram_state.current_depth_effect.end;
    start = gProgram_state.current_depth_effect.start;

    switch (gProgram_state.current_depth_effect.type) {
    case eDepth_effect_none:
        pMaterial->flags = pMaterial->flags & ~BR_MATF_FOG_LOCAL;
        break;
    case eDepth_effect_darkness:
        pMaterial->fog_min = gCamera_yon * powf(-0.1f * (float)start, 10.0f);
        pMaterial->fog_max = gCamera_yon * powf(0.1f * (float)end, 10.0f);
        pMaterial->fog_colour = 0;
        pMaterial->flags |= BR_MATF_FOG_LOCAL;
        break;
    case eDepth_effect_fog:
        pMaterial->fog_min = gCamera_yon * powf(-0.1f * (float)start, 10.0f);
        pMaterial->fog_max = gCamera_yon * powf(0.1f * (float)end, 10.0f);
        pMaterial->fog_colour = BR_COLOUR_RGB(0xf8, 0xf8, 0xf8);
        pMaterial->flags |= BR_MATF_FOG_LOCAL;
        break;
    case eDepth_effect_colour:
        pMaterial->fog_min = gCamera_yon * powf(-0.1f * (float)start, 10.0f);
        pMaterial->fog_max = powf(0.1f * end, 10.0f) * gCamera_yon;
        pMaterial->fog_colour = BR_COLOUR_RGB(
                gProgram_state.current_depth_effect.colour.red,
                gProgram_state.current_depth_effect.colour.green,
                gProgram_state.current_depth_effect.colour.blue);
        pMaterial->flags |= BR_MATF_FOG_LOCAL;
        break;
    }
    BrMaterialUpdate(pMaterial, BR_MATU_ALL);
}

// FUNCTION: CARMA2_HW 0x00447220
void C2_HOOK_FASTCALL ChangeDepthEffect(void) {

    gProgram_state.current_depth_effect.type = gProgram_state.default_depth_effect.type;
    gProgram_state.current_depth_effect.sky_texture = gProgram_state.default_depth_effect.sky_texture;
    gProgram_state.current_depth_effect.start = gProgram_state.default_depth_effect.start;
    gProgram_state.current_depth_effect.end = gProgram_state.default_depth_effect.end;
}

void C2_HOOK_FASTCALL DoHorizon(br_pixelmap* pRender_buffer, br_pixelmap* pDepth_buffer, br_actor* pCamera, br_matrix34* pCamera_to_world) {
    br_angle yaw;
    br_actor* actor;

    yaw = BrRadianToAngle(atan2(pCamera_to_world->m[2][0], pCamera_to_world->m[2][2]));
    if (gProgram_state.cockpit_on || (gAction_replay_mode && gAction_replay_camera_mode != kActionReplayCameraMode_Standard)
            || gAction_replay_camera_mode == kActionReplayCameraMode_Internal || gAdapt_sky_model_for_cockpit) {

        if (gRendering_mirror) {
            actor = gRearview_sky_actor;
        } else {
            actor = gForward_sky_actor;
            if (ACTOR_CAMERA(gCamera)->field_of_view != gOld_fov || ACTOR_CAMERA(gCamera)->yon_z != gOld_yon) {
                gOld_fov = ACTOR_CAMERA(gCamera)->field_of_view;
                gOld_yon = ACTOR_CAMERA(gCamera)->yon_z;
                MungeSkyModel(gCamera, gForward_sky_model);
            }
        }
        BrMatrix34RotateY(&actor->t.t.mat, yaw);
        BrVector3Copy(&actor->t.t.translate.t, (br_vector3*)pCamera_to_world->m[3]);
        gHorizon_material->map_transform.m[0][0] = 1.f;
        gHorizon_material->map_transform.m[0][1] = 0.f;
        gHorizon_material->map_transform.m[1][0] = 0.f;
        gHorizon_material->map_transform.m[1][1] = 1.f;
        gHorizon_material->map_transform.m[2][0] = -BrFixedToFloat(yaw) / BrFixedToFloat(gSky_image_width);
        gHorizon_material->map_transform.m[2][1] = 0.f;
        BrMaterialUpdate(gHorizon_material, BR_MATU_ALL);
        actor->render_style = BR_RSTYLE_FACES;
        BrZbSceneRenderAdd(actor);
        actor->render_style = BR_RSTYLE_NONE;
    }
}

// FUNCTION: CARMA2_HW 0x00445cb0
void C2_HOOK_FASTCALL DepthEffectSky(br_pixelmap* pRender_buffer, br_pixelmap* pDepth_buffer, br_actor* pCamera, br_matrix34* pCamera_to_world) {

    DoHorizon(pRender_buffer, pDepth_buffer, pCamera, pCamera_to_world);
}

// FUNCTION: CARMA2_HW 0x00446680
void C2_HOOK_FASTCALL DoWobbleCamera(br_actor* pCamera) {
    float f_time;

    // GLOBAL: CARMA2_HW 0x00591194
    static float mag00 = 0.02f;
    // GLOBAL: CARMA2_HW 0x00591198
    static  float mag01 = 0.02f;
    // GLOBAL: CARMA2_HW 0x0059119c
    static float mag02 = 0.02f;
    // GLOBAL: CARMA2_HW 0x005911a0
    static float mag10 = 0.15f;
    // GLOBAL: CARMA2_HW 0x005911a4
    static float mag11 = 0.05f;
    // GLOBAL: CARMA2_HW 0x005911a8
    static float mag12 = 0.02f;
    // GLOBAL: CARMA2_HW 0x0067c4e4
    static float mag20;
    // GLOBAL: CARMA2_HW 0x0067c4e8
    static float mag21;
    // GLOBAL: CARMA2_HW 0x0067c4ec
    static float mag22;
    // GLOBAL: CARMA2_HW 0x005911ac
    static float period00 = 3000.f;
    // GLOBAL: CARMA2_HW 0x005911b0
    static float period01 = 3000.f;
    // GLOBAL: CARMA2_HW 0x005911b4
    static float period02 = 4000.f;
    // GLOBAL: CARMA2_HW 0x005911b8
    static float period10 = 2200.f;
    // GLOBAL: CARMA2_HW 0x005911bc
    static float period11 = 3300.f;
    // GLOBAL: CARMA2_HW 0x005911c0
    static float period12 = 3100.f;
    // GLOBAL: CARMA2_HW 0x005911c4
    static float period20 = 2800.f;
    // GLOBAL: CARMA2_HW 0x005911c8
    static float period21 = 2500.f;
    // GLOBAL: CARMA2_HW 0x005911cc
    static float period22 = 3900.f;

    f_time = (float)PDGetTotalTime();
    pCamera->t.t.mat.m[0][0] += FastScalarSin((int)fmod(f_time / period00 * 360.f, 360.f)) * mag00;
    pCamera->t.t.mat.m[0][1] += FastScalarSin((int)fmod(f_time / period01 * 360.f, 360.f)) * mag01;
    pCamera->t.t.mat.m[0][2] += FastScalarSin((int)fmod(f_time / period02 * 360.f, 360.f)) * mag02;
    pCamera->t.t.mat.m[1][0] += FastScalarSin((int)fmod(f_time / period10 * 360.f, 360.f)) * mag10;
    pCamera->t.t.mat.m[1][1] += FastScalarSin((int)fmod(f_time / period11 * 360.f, 360.f)) * mag11;
    pCamera->t.t.mat.m[1][2] += FastScalarSin((int)fmod(f_time / period12 * 360.f, 360.f)) * mag12;
    pCamera->t.t.mat.m[2][0] += FastScalarSin((int)fmod(f_time / period20 * 360.f, 360.f)) * mag20;
    pCamera->t.t.mat.m[2][1] += FastScalarSin((int)fmod(f_time / period21 * 360.f, 360.f)) * mag21;
    pCamera->t.t.mat.m[2][2] += FastScalarSin((int)fmod(f_time / period22 * 360.f, 360.f)) * mag22;
}

void C2_HOOK_FASTCALL DoDrugWobbleCamera(br_actor* pCamera) {
    float f_time;
    // GLOBAL: CARMA2_HW 0x005911d0
    static float mag00 = 0.03f;
    // GLOBAL: CARMA2_HW 0x005911d4
    static float mag01 = 0.03f;
    // GLOBAL: CARMA2_HW 0x005911d8
    static float mag02 = 0.03f;
    // GLOBAL: CARMA2_HW 0x005911dc
    static float mag10 = 0.2f;
    // GLOBAL: CARMA2_HW 0x005911e0
    static float mag11 = 0.07f;
    // GLOBAL: CARMA2_HW 0x005911e4
    static float mag12 = 0.03f;
    // GLOBAL: CARMA2_HW 0x005911e8
    static float mag20 = 0.02f;
    // GLOBAL: CARMA2_HW 0x005911ec
    static float mag21 = 0.03f;
    // GLOBAL: CARMA2_HW 0x005911f0
    static float mag22 = 0.01f;
    // GLOBAL: CARMA2_HW 0x005911f4
    static float period00 = 550.f;
    // GLOBAL: CARMA2_HW 0x005911f8
    static float period01 = 700.f;
    // GLOBAL: CARMA2_HW 0x005911fc
    static float period02 = 200.f;
    // GLOBAL: CARMA2_HW 0x00591200
    static float period10 = 100.f;
    // GLOBAL: CARMA2_HW 0x00591204
    static float period11 = 1300.f;
    // GLOBAL: CARMA2_HW 0x00591208
    static float period12 = 500.f;
    // GLOBAL: CARMA2_HW 0x0059120c
    static float period20 = 800.f;
    // GLOBAL: CARMA2_HW 0x00591210
    static float period21 = 1500.f;
    // GLOBAL: CARMA2_HW 0x00591214
    static float period22 = 300.f;

    f_time = (float)PDGetTotalTime();
    pCamera->t.t.mat.m[0][0] += FastScalarSin((int)fmod(f_time / period00 * 360.f, 360.f)) * mag00;
    pCamera->t.t.mat.m[0][1] += FastScalarSin((int)fmod(f_time / period01 * 360.f, 360.f)) * mag01;
    pCamera->t.t.mat.m[0][2] += FastScalarSin((int)fmod(f_time / period02 * 360.f, 360.f)) * mag02;
    pCamera->t.t.mat.m[1][0] += FastScalarSin((int)fmod(f_time / period10 * 360.f, 360.f)) * mag10;
    pCamera->t.t.mat.m[1][1] += FastScalarSin((int)fmod(f_time / period11 * 360.f, 360.f)) * mag11;
    pCamera->t.t.mat.m[1][2] += FastScalarSin((int)fmod(f_time / period12 * 360.f, 360.f)) * mag12;
    pCamera->t.t.mat.m[2][0] += FastScalarSin((int)fmod(f_time / period20 * 360.f, 360.f)) * mag20;
    pCamera->t.t.mat.m[2][1] += FastScalarSin((int)fmod(f_time / period21 * 360.f, 360.f)) * mag21;
    pCamera->t.t.mat.m[2][2] += FastScalarSin((int)fmod(f_time / period22 * 360.f, 360.f)) * mag22;
}

int C2_HOOK_FASTCALL DoSubAquaCam(void) {

    if (InWater(gHud_tinted1)) {
        TurnTintedPolyOn(gHud_tinted1);
        return 1;
    } else {
        TurnTintedPolyOff(gHud_tinted1);
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x00446340
void C2_HOOK_FASTCALL DoSpecialCameraEffect(br_actor* pCamera, br_matrix34* pCamera_to_world) {

    UpdateTintedPolys();
    if (gOn_drugs || !gAction_replay_mode) {
        TurnTintedPolyOn(gHud_tinted2);
        DoDrugWobbleCamera(pCamera);
    } else {
        TurnTintedPolyOff(gHud_tinted2);
        gLast_camera_special_volume = FindSpecialVolume((br_vector3*)pCamera_to_world->m[3], gLast_camera_special_volume, 0);
        if (gLast_camera_special_volume != NULL) {
            if (gLast_camera_special_volume->camera_special_effect_index == 0) {
                DoWobbleCamera(pCamera);
            }
        }
        if (gHas_sky_texture && gSky_on) {
            if ((gProgram_state.current_depth_effect.sky_texture != NULL) == (gLast_camera_special_volume != NULL && gLast_camera_special_volume->sky_col == -2)) {
                ToggleSkyQuietly();
            }
        }
    }
    SetTintedPolyRefMaterial(gHud_tinted1, (br_vector3*)pCamera_to_world->m[3]);
    if (DoSubAquaCam()) {
        DoWobbleCamera(pCamera);
    }
}
