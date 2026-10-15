#include "50-fog.h"

// GLOBAL: CARMA2_HW 0x006a6d38
int gMaterials_to_adapt_count;

// GLOBAL: CARMA2_HW 0x006a8298
int gAllow_material_adapt;

// FUNCTION: CARMA2_HW 0x004ea760
void C2_HOOK_FASTCALL InitFogificateMaterials(void) {

    gMaterials_to_adapt_count = 0;
}

// FogificateMaterials

// AutoFogificateMaterials

// AddFogificateMaterial

// FUNCTION: CARMA2_HW 0x004ea840
void C2_HOOK_FASTCALL EnableAutoFogification(void) {
    gAllow_material_adapt = 1;
}

// FUNCTION: CARMA2_HW 0x004ea850
void C2_HOOK_FASTCALL DisableAutoFogification(void) {
    gAllow_material_adapt = 0;
}

// STUB: CARMA2_HW 0x004ea860
void C2_HOOK_FASTCALL RemoveMaterialFromFogification(br_material* pMaterial) {
    NOT_IMPLEMENTED();
}

