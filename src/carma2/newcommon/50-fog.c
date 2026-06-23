#include "50-fog.h"

// GLOBAL: CARMA2_HW 0x006a6d38
int gMaterials_to_adapt_count;

// FUNCTION: CARMA2_HW 0x004ea760
void C2_HOOK_FASTCALL InitFogificateMaterials(void) {

    gMaterials_to_adapt_count = 0;
}

// FogificateMaterials

// AutoFogificateMaterials

// AddFogificateMaterial

// EnableAutoFogification

// STUB: CARMA2_HW 0x004ea850
void C2_HOOK_FASTCALL DisableAutoFogification(void) {
    NOT_IMPLEMENTED();
}

// STUB: CARMA2_HW 0x004ea860
void C2_HOOK_FASTCALL RemoveMaterialFromFogification(br_material* pMaterial) {
    NOT_IMPLEMENTED();
}

