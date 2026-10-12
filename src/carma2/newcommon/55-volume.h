#ifndef GUARD_55_VOLUME_H
#define GUARD_55_VOLUME_H

#include "rec2_types.h"
#include "c2_hooks.h"

#include <stdio.h>

extern tSpecial_volume gDefault_default_water_spec_vol;
extern tSpecial_volume* gDefault_water_spec_vol_real;

extern tSpecial_volume* C2_HOOK_FASTCALL GetDefaultSpecialVolumeForWater(void);

// FindSpecialVolume

// MungeSpecialVolume

extern void C2_HOOK_FASTCALL ResetCarSpecialVolume(tPhysics_object* pObject);

extern void C2_HOOK_FASTCALL TestAutoSpecialVolume(tPhysics_object* pObject);

extern void C2_HOOK_FASTCALL ParseSpecialVolume(FILE* pF, tSpecial_volume* pSpec, char* pScreen_name_str, int soundfx);

// SaveSpecialVolumes

// FindInverseAndWorldBox

// ReadSpecialVolumes

#endif // GUARD_55_VOLUME_H
