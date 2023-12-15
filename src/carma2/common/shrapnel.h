#ifndef REC2_SHRAPNEL_H
#define REC2_SHRAPNEL_H

#include "rec2_types.h"

#include "c2_hooks.h"
#include "c2_stdio.h"

void C2_HOOK_FASTCALL ReadShrapnel(FILE* pF, tShrapnel_spec* pShrapnel_spec, int* pShrapnel_count);

#endif //REC2_SHRAPNEL_H
