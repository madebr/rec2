#include "33-depth.h"

#include "globvars.h"

// GLOBAL: CARMA2_HW 0x00591188
int gSky_on = 1;

// GLOBAL: CARMA2_HW 0x0079ec1c
br_pixelmap* gSwap_sky_texture;

// GLOBAL: CARMA2_HW 0x0067c4e0
br_material* gHorizon_material;

// GLOBAL: CARMA2_HW 0x0067c4a0
br_model* gForward_sky_model;

// Log2

// CalculateWrappingMultiplier

br_scalar C2_HOOK_FASTCALL DepthCueingShiftToDistance(int pDistance) {

    return (br_scalar)gCamera_yon * pow(10.0, 0.1 * (double)pDistance);
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

// FogCars

// FrobFog


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

// InstantDepthChange

// Tan

// EdgeU

// MungeSkyModel

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

// AssertYons

// IncreaseYon

// DecreaseYon

// STUB: CARMA2_HW 0x00446b70
void C2_HOOK_STDCALL SetYon(br_scalar pYon) {
    NOT_IMPLEMENTED();
}

// GetYon

// GetSkyTextureOn

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

// GetDepthCueingOn

// STUB: CARMA2_HW 0x00446fa0
void C2_HOOK_FASTCALL SetDepthCueingOn(int pOn) {
    NOT_IMPLEMENTED();
}

// ToggleDepthCueingQuietly

// ToggleDepthCueing

// ChangeDepthEffect

// MungeForwardSky

// MungeRearviewSky

// SkyTextureChanged

// SetSkyColour

