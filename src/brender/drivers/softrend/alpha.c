#include "alpha.h"

void C2_HOOK_CDECL SurfaceAlpha(br_soft_renderer* self, br_vector3* p, br_vector2* map, br_vector3* n, br_colour colour, br_scalar *comp) {

    comp[C_A] = self->state.cache.comp_scales[C_A] * (BR_ALPHA(colour) / 256.f) + self->state.cache.comp_offsets[C_A];
}
C2_HOOK_FUNCTION(0x0054b110, SurfaceAlpha)
