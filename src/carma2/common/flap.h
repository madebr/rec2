#ifndef REC2_FLAP_H
#define REC2_FLAP_H

#include "c2_hooks.h"

#include "rec2_types.h"

tCollision_info* C2_HOOK_FASTCALL DetachBit(tCar_spec* pCar, br_actor* pActor, br_bounds3* pBounds);

tCollision_info* C2_HOOK_FASTCALL SemiDetachBit(tCar_spec* pCar, br_actor* pActor, float pArg3, int* pArg4, br_vector3* pArg5, br_bounds3* pArg6, br_vector3* pArg7, br_vector3* pArg8);

int C2_HOOK_FASTCALL GetSDBJointPosAndBounds(br_vector3* pP1, br_vector3* pP2, br_vector3* pP33, br_bounds3* pBounds, br_actor* pActor);

void C2_HOOK_FASTCALL SendSemiDetachBit(tCar_spec* pCar, br_actor* pActor, float pArg3, undefined4* pArg4);

int C2_HOOK_FASTCALL BitIsInBentPartOfCar(br_actor* pActor, float pArg2, float pArg3);

void C2_HOOK_FASTCALL DoDetaching(void);

void C2_HOOK_FASTCALL SetBitForDetachment(br_actor* pActor, tCar_spec* pCar, float pArg3, int pArg4);

void C2_HOOK_FASTCALL MakeModelMaterialsDoubleSided(br_model* pModel);

intptr_t C2_HOOK_FASTCALL FlapBit(br_actor* pActor, br_matrix34* pMat, void* pContext);

void C2_HOOK_FASTCALL FlapBits(tCar_spec* pCar);

void C2_HOOK_FASTCALL StartFlapping(br_actor* pActor, tCar_spec* pCar, tU8 pBits, float pArg4);

void C2_HOOK_FASTCALL DoFlapping(void);

int C2_HOOK_FASTCALL BitObjectIsSufficientlyOutsideCarObjectToDetach(tCollision_info* pObject);

void C2_HOOK_FASTCALL SendFullyDetachBit(tCar_spec* pCar, tCollision_info* pObject);

void C2_HOOK_FASTCALL MungeDetachLists(tCar_crush_spec* pCar_crush);

void C2_HOOK_FASTCALL FullyDetachBit(tCar_spec *pCar, tCollision_info* pObject);

void C2_HOOK_FASTCALL DoFullyDetaching(void);

void C2_HOOK_FASTCALL BendCar(tCar_spec* pCar, br_angle pAngle_x, br_angle pAngle_y, br_angle pAngle_z, const br_vector3* pArg5, float pArg6, int pArg7);

void C2_HOOK_FASTCALL DoBending(void);

#endif
