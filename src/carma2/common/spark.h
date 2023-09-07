#ifndef REC2_SPARK_H
#define REC2_SPARK_H

#include "rec2_types.h"
#include "brender/brender.h"

#include "c2_stdio.h"

#include "c2_hooks.h"

C2_HOOK_VARIABLE_DECLARE(int, gSmoke_on);
C2_HOOK_VARIABLE_DECLARE_ARRAY(int, gShade_list, 16);
C2_HOOK_VARIABLE_DECLARE(int*, gShade_table); /* FIXME: rename to gDust_table*/
C2_HOOK_VARIABLE_DECLARE(int, gNum_dust_tables);
C2_HOOK_VARIABLE_DECLARE_ARRAY(br_model*, gShrapnel_model, 2);
C2_HOOK_VARIABLE_DECLARE(br_material*, gBlack_material);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tShrapnel, gShrapnel, 30);
C2_HOOK_VARIABLE_DECLARE(int, gColumn_flags);
C2_HOOK_VARIABLE_DECLARE(br_model*, gLollipop_model);
C2_HOOK_VARIABLE_DECLARE_ARRAY(br_pixelmap*, gFlame_map, 20);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tSmoke_column, gSmoke_column, 10);
C2_HOOK_VARIABLE_DECLARE(int, gSplash_flags);
C2_HOOK_VARIABLE_DECLARE(br_model*, gSplash_model);
C2_HOOK_VARIABLE_DECLARE(int, gNum_splash_types);
C2_HOOK_VARIABLE_DECLARE_ARRAY(br_material*, gSplash_material, 20);
C2_HOOK_VARIABLE_DECLARE_ARRAY(tSplash, gSplash, 20);

void C2_HOOK_FASTCALL SetSmokeOn(int pSmoke_on);

int C2_HOOK_FASTCALL GetSmokeOn(void);

void C2_HOOK_FASTCALL GenerateSmokeShades(void);

void C2_HOOK_FASTCALL GetSmokeShadeTables(FILE* f);

void C2_HOOK_FASTCALL ModelScale(br_model* pModel, float pScale);

void C2_HOOK_FASTCALL LoadInShrapnel(void);

void C2_HOOK_FASTCALL InitShrapnel(void);

void C2_HOOK_FASTCALL InitFlame(void);

void C2_HOOK_FASTCALL LoadInKevStuff(FILE* pF);

#endif //REC2_SPARK_H
