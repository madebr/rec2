/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "alpha.h"

// FUNCTION: CARMA2_HW 0x0054b110
// FUNCTION: CARMA2_SW 0x005467b0
void C2_HOOK_CDECL SurfaceAlpha(br_soft_renderer* self, br_vector3* p, br_vector2* map, br_vector3* n, br_colour colour, br_scalar *comp) {

    comp[C_A] = self->state.cache.comp_scales[C_A] * (BR_ALPHA(colour) / 256.f) + self->state.cache.comp_offsets[C_A];
}
