#ifndef REC2_SPARK_H
#define REC2_SPARK_H

#include "brender/brender.h"

#include "c2_stdio.h"

#include "c2_hooks.h"

C2_HOOK_VARIABLE_DECLARE(int, gSmoke_on);
C2_HOOK_VARIABLE_DECLARE_ARRAY(int, gShade_list, 16);
C2_HOOK_VARIABLE_DECLARE(int*, gShade_table); /* FIXME: rename to gDust_table*/
C2_HOOK_VARIABLE_DECLARE(int, gNum_dust_tables);
C2_HOOK_VARIABLE_DECLARE_ARRAY(br_model*, gShrapnel_model, 2);
C2_HOOK_VARIABLE_DECLARE(br_material*, gBlack_material);

void C2_HOOK_FASTCALL SetSmokeOn(int pSmoke_on);

int C2_HOOK_FASTCALL GetSmokeOn(void);

void C2_HOOK_FASTCALL GenerateSmokeShades(void);

void C2_HOOK_FASTCALL GetSmokeShadeTables(FILE* f);

void C2_HOOK_FASTCALL ModelScale(br_model* pModel, float pScale);

void C2_HOOK_FASTCALL LoadInShrapnel(void);

void C2_HOOK_FASTCALL LoadInKevStuff(FILE* pF);

#endif //REC2_SPARK_H
