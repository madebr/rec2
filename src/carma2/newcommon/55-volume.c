#include "55-volume.h"

#include "08-loading1.h"
#include "69-sound.h"
#include "globvars.h"

#include "c2_string.h"

#include "rec2_macros.h"

// GLOBAL: CARMA2_HW 0x00660268
const char* gSoundType_choices[2] = {
    "SATURATED",
    "SCATTERED"
};

// GetDefaultSpecialVolumeForWater

// FindSpecialVolume

// MungeSpecialVolume

// ResetCarSpecialVolume

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
