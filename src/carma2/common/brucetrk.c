#include "brucetrk.h"

C2_HOOK_VARIABLE_IMPLEMENT_INIT(br_scalar, gYon_factor, 0x00655e60, 0.25f);

br_scalar C2_HOOK_STDCALL GetYonFactor(void) {

    return C2V(gYon_factor);
}
C2_HOOK_FUNCTION(0x0040dfe0, GetYonFactor)

void C2_HOOK_STDCALL SetYonFactor(br_scalar pNew) {

    C2V(gYon_factor) = pNew;
}
C2_HOOK_FUNCTION(0x0040dff0, SetYonFactor)

C2_HOOK_VARIABLE_IMPLEMENT(br_actor*, gMr_blendy, 0x00679264);

void (C2_HOOK_FASTCALL * StripBlendedFaces_original)(br_actor* pActor, br_model* pModel);
void C2_HOOK_FASTCALL StripBlendedFaces(br_actor* pActor, br_model* pModel) {

#if defined(C2_HOOKS_ENABLED)
    StripBlendedFaces_original(pActor, pModel);
#else
#error "Not implemented"
#endif
}
C2_HOOK_FUNCTION_ORIGINAL(0x0040d530, StripBlendedFaces, StripBlendedFaces_original)
