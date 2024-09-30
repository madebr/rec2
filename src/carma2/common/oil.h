#ifndef REC2_OIL_H
#define REC2_OIL_H

#include "rec2_types.h"

#include "c2_hooks.h"

C2_HOOK_VARIABLE_DECLARE(int, gNext_oil_pixie);

void C2_HOOK_FASTCALL InitOilSpills(void);

void C2_HOOK_FASTCALL ResetOilSpills(void);

void C2_HOOK_FASTCALL SetInitialOilStuff(tOil_spill_info* pOil, br_model* pModel);

void C2_HOOK_FASTCALL MungeOilsHeightAeGround(tOil_spill_info* pThe_spill);

int C2_HOOK_FASTCALL OKToSpillOil(tOil_spill_info* pOil);

void C2_HOOK_FASTCALL MungeOilsHeightAboveGround(tOil_spill_info* pOil);

void C2_HOOK_FASTCALL ProcessOilSpills(tU32 pFrame_period);

#endif //REC2_OIL_H
