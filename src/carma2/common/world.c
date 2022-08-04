#include "world.h"

#include "loading.h"
#include "sound.h"

#include <brender/brender.h>
#include "rec2_macros.h"

#include <string.h>

C2_HOOK_VARIABLE_IMPLEMENT_ARRAY_INIT(const char*, gSoundType_Choices, 2, 0x00660268, {"SATURATED", "SCATTERED"});
C2_HOOK_VARIABLE_IMPLEMENT_INIT(tCar_texturing_level, gCar_texturing_level, 0x00591374, eCTL_full);

tCar_texturing_level C2_HOOK_FASTCALL GetCarTexturingLevel(void) {

    return C2V(gCar_texturing_level);
}
C2_HOOK_FUNCTION(0x00447640, GetCarTexturingLevel)

void (C2_HOOK_FASTCALL * SetCarTexturingLevel_original)(tCar_texturing_level pLevel);
void C2_HOOK_FASTCALL SetCarTexturingLevel(tCar_texturing_level pLevel) {
#if defined(C2_HOOKS_ENABLED)
    SetCarTexturingLevel_original(pLevel);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x00447650, SetCarTexturingLevel, SetCarTexturingLevel_original)

void C2_HOOK_FASTCALL ParseSpecialVolume(tTWTFILE* pF, tSpecial_volume* pSpec, char* pScreen_name_str, int soundfx) {
    char s[256];

    REC2_BUG_ON(sizeof(tSpecial_volume) != 220);
    REC2_BUG_ON(sizeof(tSpecial_volume_soundfx_data) != 36);
    REC2_BUG_ON(offsetof(tSpecial_volume, gravity_multiplier) != 136);

    pSpec->gravity_multiplier = GetAFloat(pF);
    if (pSpec->gravity_multiplier < 0 && !C2V(gRusselsFannies)) {
        pSpec->gravity_multiplier = 1.f;

    }
    pSpec->viscosity_multiplier = GetAFloat(pF);
    pSpec->car_damage_per_ms = GetAScalar(pF);
    pSpec->ped_damage_per_ms = GetAScalar(pF);
    pSpec->camera_special_effect_index = GetAnInt(pF);
    pSpec->sky_col = GetAnInt(pF);

    GetAString(pF, s);
    if (pScreen_name_str != NULL) {
        strcpy(pScreen_name_str, s);
    } else {
        pSpec->screen_material = BrMaterialFind(s);
    }

    pSpec->entry_noise = GetAnInt(pF);
    pSpec->exit_noise = GetAnInt(pF);
    pSpec->engine_noise_index = GetAnInt(pF);
    pSpec->material_modifier_index = GetAnInt(pF);
    if (soundfx) {
        pSpec->soundfx_type = GetALineAndInterpretCommand(pF, C2V(gSoundType_Choices), REC2_ASIZE(C2V(gSoundType_Choices)));
        if (pSpec->soundfx_type != kSoundFx_None) {
            ParseSoundFxDetails(pF, &pSpec->soundfx_data);
        }
    } else {
        pSpec->soundfx_type = kSoundFx_None;
    }
}

C2_HOOK_FUNCTION(0x004ff8d0, ParseSpecialVolume)
