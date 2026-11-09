/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_55_VOLUME_H
#define GUARD_55_VOLUME_H

#include "rec2_types.h"
#include "c2_hooks.h"

#include <stdio.h>

extern tSpecial_volume gDefault_default_water_spec_vol;
extern tSpecial_volume* gDefault_water_spec_vol_real;
extern const char* gSpecial_effects_boundary_choices[4];

extern tSpecial_volume* C2_HOOK_FASTCALL GetDefaultSpecialVolumeForWater(void);

extern tSpecial_volume* C2_HOOK_FASTCALL FindSpecialVolume(br_vector3* pP, tSpecial_volume* pLast_vol, int pArg3);

extern void C2_HOOK_FASTCALL MungeSpecialVolume(tPhysics_object* pObject);

extern void C2_HOOK_FASTCALL ResetCarSpecialVolume(tPhysics_object* pObject);

extern void C2_HOOK_FASTCALL TestAutoSpecialVolume(tPhysics_object* pObject);

extern void C2_HOOK_FASTCALL ParseSpecialVolume(FILE* pF, tSpecial_volume* pSpec, char* pScreen_name_str, int soundfx);

extern void C2_HOOK_FASTCALL SaveSpecialVolumes(void);

extern void C2_HOOK_FASTCALL FindInverseAndWorlBox(tSpecial_volume* pSpec);

extern void C2_HOOK_FASTCALL ReadSpecialVolumes(FILE* pF);

#endif // GUARD_55_VOLUME_H
