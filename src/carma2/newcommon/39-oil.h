#ifndef GUARD_39_OIL_H
#define GUARD_39_OIL_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern const char* gOil_pixie_names[1];
extern br_pixelmap* gOil_pixies[1];
extern tOil_spill_info gOily_spills[32];
extern br_material* oily_material;
extern int gNext_oil_pixie;

extern void C2_HOOK_FASTCALL InitOilSpills(void);

extern void C2_HOOK_FASTCALL ResetOilSpills(void);

// QueueOilSpill2

// QueueOilSpill

// OKToSpillOil

// MungeOilsHeightAboveGround

// MungeIndexedOilsHeightAboveGround

extern void C2_HOOK_FASTCALL SetInitialOilStuff(tOil_spill_info* pOil, br_model* pModel);

// ProcessOilSpills

extern int C2_HOOK_FASTCALL GetOilSpillCount(void);

extern void C2_HOOK_FASTCALL GetOilSpillDetails(int pIndex, br_actor** pActor, br_scalar* pSize);

// PointInSpill

// GetOilFrictionFactors

// AdjustOilSpill

// ReceivedOilSpill

extern void C2_HOOK_FASTCALL RemoveCarsOilSlicks(tCar_spec* pCar);

#endif // GUARD_39_OIL_H
