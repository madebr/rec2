#ifndef REC2_OIL_H
#define REC2_OIL_H

#include "rec2_types.h"

#include "c2_hooks.h"

void C2_HOOK_FASTCALL InitOilSpills(void);

void C2_HOOK_FASTCALL ResetOilSpills(void);

void C2_HOOK_FASTCALL ProcessOilSpills(tU32 pFrame_period);

#endif //REC2_OIL_H
