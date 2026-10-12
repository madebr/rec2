#include "55-volume.h"

#include "08-loading1.h"
#include "00-car.h"
#include "34-finteray.h"
#include "69-sound.h"
#include "globvars.h"

#include "c2_string.h"

#include "rec2_macros.h"

// GLOBAL: CARMA2_HW 0x00761b80
tSpecial_volume gDefault_default_water_spec_vol;

// GLOBAL: CARMA2_HW 0x00761f44
tSpecial_volume* gDefault_water_spec_vol_real;

// GLOBAL: CARMA2_HW 0x00660268
const char* gSoundType_choices[2] = {
    "SATURATED",
    "SCATTERED"
};

// FUNCTION: CARMA2_HW 0x004ff110
tSpecial_volume* C2_HOOK_FASTCALL GetDefaultSpecialVolumeForWater(void) {
    return gDefault_water_spec_vol_real;
}

// FindSpecialVolume

// MungeSpecialVolume

/* The original scans the complete identifier even though only emptiness is used. */
static int C2_HOOK_CDECL VolumeIdentifierHasLength(const char* pIdentifier) {
    const volatile char* end = pIdentifier;

    while (*end != '\0') {
        end++;
    }
    return end != pIdentifier;
}

static int C2_HOOK_CDECL VolumeBelowSurfaceLimit(const br_scalar* pDistance) {
    tU32 bits;

    memcpy(&bits, pDistance, 4);
    return (bits & 0x7fffffffu) > 0x7f800000u || *pDistance < 100.f;
}

// FUNCTION: CARMA2_HW 0x004ff530
void C2_HOOK_FASTCALL ResetCarSpecialVolume(tPhysics_object* pObject) {
    br_vector3 direction;
    br_vector3 normal;
    br_scalar distance;
    br_material* material;
    const char* identifier;
    tSpecial_volume* volume = NULL;

    direction.v[0] = 0.f;
    direction.v[1] = 200.f;
    direction.v[2] = 0.f;
    DisablePlingMaterials();
    FindFace(&pObject->actor->t.t.translate.t, &direction, &normal, &distance, &material);
    EnablePlingMaterials();
    if (VolumeBelowSurfaceLimit(&distance) && material != NULL) {
        identifier = material->identifier;
        if (identifier != NULL && VolumeIdentifierHasLength(identifier)) {
            if (identifier[0] == '!' || identifier[0] == '#') {
                volume = gDefault_water_spec_vol_real;
            }
        }
    }
    pObject->auto_special_volume = volume;
    pObject->water_depth_factor = 1.f;
}

// TestAutoSpecialVolume

// FUNCTION: CARMA2_HW 0x004ff8d0
void C2_HOOK_FASTCALL ParseSpecialVolume(FILE* pF, tSpecial_volume* pSpec, char* pScreen_name_str, int soundfx) {
    char s[256];

    /* gravity multiplier */
    pSpec->gravity_multiplier = GetAFloat(pF);
    if (pSpec->gravity_multiplier < 0 && !gRussels_fannies) {
        pSpec->gravity_multiplier = 1.f;
    }
    /* viscosity multiplier */
    pSpec->viscosity_multiplier = GetAFloat(pF);
    /* Car damage per millisecond */
    pSpec->car_damage_per_ms = GetAScalar(pF);
    /* Pedestrian damage per millisecond */
    pSpec->ped_damage_per_ms = GetAScalar(pF);
    /* camera effect index */
    pSpec->camera_special_effect_index = GetAnInt(pF);
    /* Sky colour */
    pSpec->sky_col = GetAnInt(pF);

    /* Windscreen material to use */
    GetAString(pF, s);
    if (pScreen_name_str != NULL) {
        strcpy(pScreen_name_str, s);
    } else {
        pSpec->screen_pixelmap = BrMapFind(s);
    }

    /* Sound ID of entry noise */
    pSpec->entry_noise = GetAnInt(pF);
    /* Sound ID of exit noise */
    pSpec->exit_noise = GetAnInt(pF);
    /* Engine noise index */
    pSpec->engine_noise_index = GetAnInt(pF);
    /* material index */
    pSpec->material_modifier_index = GetAnInt(pF);
    if (soundfx) {
        pSpec->soundfx_type = GetALineAndInterpretCommand(pF, gSoundType_choices, REC2_ASIZE(gSoundType_choices));
        if (pSpec->soundfx_type != kSoundFx_None) {
            ReadSoundSpec(pF, &pSpec->soundfx_data);
        }
    } else {
        pSpec->soundfx_type = kSoundFx_None;
    }
}

// SaveSpecialVolumes

// FindInverseAndWorldBox

// ReadSpecialVolumes
