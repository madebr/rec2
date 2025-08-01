#ifndef REC2_FLAP_H
#define REC2_FLAP_H

#include "c2_hooks.h"

#include "rec2_types.h"

void C2_HOOK_FASTCALL DoDetaching(void);

void C2_HOOK_FASTCALL SetBitForDetachment(br_actor* pActor, tCar_spec* pCar, float pArg3, int pArg4);

void C2_HOOK_FASTCALL MakeModelMaterialsDoubleSided(br_model* pModel);

void C2_HOOK_FASTCALL DoFlapping(void);

void C2_HOOK_FASTCALL DoFullyDetaching(void);

void C2_HOOK_FASTCALL DoBending(void);

#endif
