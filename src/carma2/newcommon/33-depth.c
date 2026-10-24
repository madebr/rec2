#include "33-depth.h"

#include "00-car.h"
#include "15-displays.h"
#include "17-world2.h"
#include "37-brucetrk.h"
#include "41-utility.h"
#include "globvars.h"
#include "globvrkm.h"
#include "rec2_macros.h"

#include "c2_stdio.h"

// GLOBAL: CARMA2_HW 0x00591188
int gSky_on = 1;

// GLOBAL: CARMA2_HW 0x0079ec1c
br_pixelmap* gSwap_sky_texture;

// GLOBAL: CARMA2_HW 0x0067c4e0
br_material* gHorizon_material;

// GLOBAL: CARMA2_HW 0x0067c4a0
br_model* gForward_sky_model;

// GLOBAL: CARMA2_HW 0x0074cee8
br_material* gMaterial[2];

// GLOBAL: CARMA2_HW 0x0059118c
int gDepth_cueing_on = 1;

// GLOBAL: CARMA2_HW 0x00591190
tDepth_effect_type gSwap_depth_effect_type = eDepth_effect_none;

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

// GLOBAL: CARMA2_HW 0x0067c4a8
int gHas_sky_texture;

// GLOBAL: CARMA2_HW 0x0079ec20
br_pixelmap* gDepth_shade_table;

static void C2_HOOK_FASTCALL FrobFog(void);

// Log2

// CalculateWrappingMultiplier

br_scalar C2_HOOK_FASTCALL DepthCueingShiftToDistance(int pDistance) {

    return gCamera_yon * (br_scalar)pow(10.0, 0.1 * (double)pDistance);
}

// FUNCTION: CARMA2_HW 0x004451a0
void C2_HOOK_FASTCALL FogAccordingToGPSCDE(br_material* pMaterial) {
    int start;
    int end;
    int red;
    int green;
    int blue;

    red = gProgram_state.current_depth_effect.colour.red;
    green = gProgram_state.current_depth_effect.colour.green;
    blue = gProgram_state.current_depth_effect.colour.blue;
    start = gProgram_state.current_depth_effect.start;
    end = gProgram_state.current_depth_effect.end;

    switch (gProgram_state.current_depth_effect.type) {
    case eDepth_effect_none:
        pMaterial->flags &= ~BR_MATF_FOG_LOCAL;
        break;
    case eDepth_effect_darkness:
        pMaterial->fog_min = DepthCueingShiftToDistance(-start);
        pMaterial->fog_max = DepthCueingShiftToDistance(end);
        pMaterial->fog_colour = 0;
        pMaterial->flags |= BR_MATF_FOG_LOCAL;
        break;
    case eDepth_effect_fog:
        pMaterial->fog_min = DepthCueingShiftToDistance(-start);
        pMaterial->fog_max = DepthCueingShiftToDistance(end);
        pMaterial->fog_colour = BR_COLOUR_RGB(0xf8, 0xf8, 0xf8);
        pMaterial->flags |= BR_MATF_FOG_LOCAL;
        break;
    case eDepth_effect_colour:
        pMaterial->fog_min = DepthCueingShiftToDistance(-start);
        pMaterial->fog_max = DepthCueingShiftToDistance(end);
        pMaterial->fog_colour = BR_COLOUR_RGB(red, green, blue);
        pMaterial->flags |= BR_MATF_FOG_LOCAL;
        break;
    }
    BrMaterialUpdate(pMaterial, BR_MATU_ALL);
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
    for (i = 0; i < (int)REC2_ASIZE(gMaterial); i++) {
        FogAccordingToGPSCDE(gMaterial[i]);
    }
    for (i = 0; i < (int)REC2_ASIZE(gCurrent_race.material_modifiers); i++) {
        if (gCurrent_race.material_modifiers[i].skid_mark_material != NULL) {
            FogAccordingToGPSCDE(gCurrent_race.material_modifiers[i].skid_mark_material);
        }
    }
    FogAccordingToGPSCDE(gDefault_track_material);
}

// FUNCTION: CARMA2_HW 0x00445500
void C2_HOOK_FASTCALL MungeSkyVs(br_model* pModel, br_material* pMaterial) {
    int i;
    float tex_y;
    float f_height;
    int height;
    int vertex_idx;

    if (pMaterial->colour_map == NULL) {
        return;
    }
    if (pMaterial->flags & BR_MATF_MAP_INTERPOLATION) {
        height = pMaterial->colour_map->height - 2;
        tex_y = 1.0f / (float)pMaterial->colour_map->height;
    } else {
        height = pMaterial->colour_map->height;
        tex_y = 0.0f;
    }
    for (i = 0; i < 12; i++) {
        pModel->vertices[i].map.v[1] = 1.0f - tex_y;
    }
    for (i = 80; i < 88; i++) {
        pModel->vertices[i].map.v[1] = tex_y;
    }
    f_height = (float)height;
    vertex_idx = 12;
    for (i = 0; i < 17; vertex_idx+=4, i++) {
        int j;

        pModel->vertices[vertex_idx].map.v[1] = tex_y + (float)(17 - i) * f_height / 18.0f / pMaterial->colour_map->height;
        for (j = 1; j < 4; j++) {
            pModel->vertices[vertex_idx + j].map.v[1] = pModel->vertices[vertex_idx].map.v[1];
        }
    }
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

// Tan

// EdgeU

// CreateHorizonModel

// LoadDepthTable

// STUB: CARMA2_HW 0x00445620
void C2_HOOK_FASTCALL InitDepthEffects(void) {
    NOT_IMPLEMENTED();
}

// DoHorizon

// DoDepthCue

// DoFog

// DepthEffect

// DepthEffectSky

// DoWobbleCamera

// DoDrugWobbleCamera

// DoSubaquaCam

// DoSpecialCameraEffect

// FUNCTION: CARMA2_HW 0x00446890
// FUNCTION: CARMA2_SW 0x00446910
void C2_HOOK_FASTCALL ReduceDepthStart(void) {
    char s[256];

    if (gProgram_state.current_depth_effect.start > 3) {
        gProgram_state.current_depth_effect.start--;
    }
    sprintf(s, "Depth start reduced to %d", gProgram_state.current_depth_effect.start);
    NewTextHeadupSlot(4, 0, 500, -1, s);
    gProgram_state.default_depth_effect.start = gProgram_state.current_depth_effect.start;
}

// FUNCTION: CARMA2_HW 0x00446a10
// FUNCTION: CARMA2_SW 0x00446a90
void C2_HOOK_FASTCALL AssertYons(void) {
    br_camera* camera_ptr;
    int i;
    br_scalar yon_multiplier;

    for (i = 0; i < (int)REC2_ASIZE(gCamera_list); ++i) {
        camera_ptr = gCamera_list[i]->type_data;
        yon_multiplier = gYon_multiplier;
        camera_ptr->yon_z = yon_multiplier * gCamera_yon;
    }
}

// FUNCTION: CARMA2_HW 0x00446a40
// FUNCTION: CARMA2_SW 0x00446ac0
void C2_HOOK_FASTCALL IncreaseYon(void) {
    char s[256];

    gCamera_yon += 5.f;
    AssertYons();
    sprintf(s, GetMiscString(eMiscString_yon_increased_to_D), (int)((br_camera*)gCamera_list[1]->type_data)->yon_z);
    NewTextHeadupSlot(4, 0, 2000, -4, s);
}

// DecreaseYon

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

// FUNCTION: CARMA2_HW 0x00446e80
void C2_HOOK_FASTCALL ToggleSkyQuietly(void) {
    br_pixelmap* temp;

    temp = gProgram_state.current_depth_effect.sky_texture;
    gProgram_state.default_depth_effect.sky_texture = gSwap_sky_texture;
    gSwap_sky_texture = temp;
    gProgram_state.current_depth_effect.sky_texture = gProgram_state.default_depth_effect.sky_texture;
    if (gHorizon_material != NULL && gProgram_state.default_depth_effect.sky_texture != NULL) {
        gHorizon_material->colour_map = gProgram_state.current_depth_effect.sky_texture;
        BrMaterialUpdate(gHorizon_material, BR_MATU_ALL);
        MungeSkyVs(gForward_sky_model, gHorizon_material);
    }
}

// ToggleSky

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
    temp_red = gProgram_state.current_depth_effect.colour.red;
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

// ToggleDepthCueing

// FUNCTION: CARMA2_HW 0x00447220
void C2_HOOK_FASTCALL ChangeDepthEffect(void) {
    gProgram_state.current_depth_effect.type = gProgram_state.default_depth_effect.type;
    gProgram_state.current_depth_effect.sky_texture = gProgram_state.default_depth_effect.sky_texture;
    gProgram_state.current_depth_effect.start = gProgram_state.default_depth_effect.start;
    gProgram_state.current_depth_effect.end = gProgram_state.default_depth_effect.end;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x00445e20
void C2_HOOK_FASTCALL MungeSkyModel(br_actor* pCamera, br_model* pModel) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00447250
void C2_HOOK_FASTCALL MungeForwardSky(void) {
    MungeSkyModel(gCamera, gForward_sky_model);
    gHas_sky_texture = gProgram_state.default_depth_effect.sky_texture != NULL;
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

