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

extern void C2_HOOK_FASTCALL QueueOilSpill2(tCar_spec* pCar, br_actor* pCar_actor, br_model* pCar_model, tU32 pTime_delay, br_scalar pMin_size, br_scalar pMax_size, br_scalar pMin_grow_rate, br_scalar pMax_grow_rate, br_scalar pInitial_size, br_scalar pField_0x48, br_scalar pField_0x4c, br_scalar pField_0x40, br_scalar pField_0x44, br_material* pMaterial, br_material* pField_0x50);

extern void C2_HOOK_FASTCALL QueueOilSpill(tCar_spec* pCar);

// OKToSpillOil

extern void C2_HOOK_FASTCALL MungeOilsHeightAboveGround(tOil_spill_info* pOil);

extern void C2_HOOK_FASTCALL MungeIndexedOilsHeightAboveGround(int pIndex);

extern void C2_HOOK_FASTCALL SetInitialOilStuff(tOil_spill_info* pOil, br_model* pModel);

// ProcessOilSpills

extern int C2_HOOK_FASTCALL GetOilSpillCount(void);

extern void C2_HOOK_FASTCALL GetOilSpillDetails(int pIndex, br_actor** pActor, br_scalar* pSize);

extern int C2_HOOK_FASTCALL PointInSpill(br_vector3* pPoint, int pIndex);

extern void C2_HOOK_FASTCALL GetOilFrictionFactors(tCar_spec* pCar, br_scalar* pFactor_1, br_scalar* pFactor_2, br_scalar* pFactor_3, br_scalar* pFactor_4);

extern void C2_HOOK_FASTCALL AdjustOilSpill(int pIndex, br_matrix34* pMat, br_scalar pFull_size, br_scalar pGrow_rate, tU32 pSpill_time, tU32 pStop_time, tCar_spec* pCar, br_vector3* pOriginal_pos, br_pixelmap* pPixelmap);

// ReceivedOilSpill

extern void C2_HOOK_FASTCALL RemoveCarsOilSlicks(tCar_spec* pCar);

#endif // GUARD_39_OIL_H
