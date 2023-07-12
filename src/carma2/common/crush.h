#ifndef REC2_CRUSH_H
#define REC2_CRUSH_H

#include "c2_hooks.h"

#include "c2_stdio.h"

#include "rec2_types.h"

C2_HOOK_VARIABLE_DECLARE(br_vector3, gBatty_gravity);

void C2_HOOK_FASTCALL InitCrush(void);

void C2_HOOK_FASTCALL LoadGeneralCrushSettings(FILE* file);

#endif // REC2_CRUSH_H
